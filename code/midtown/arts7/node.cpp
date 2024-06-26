/*
    Open1560 - An Open Source Re-Implementation of Midtown Madness 1 Beta
    Copyright (C) 2020 Brick

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

define_dummy_symbol(arts7_node);

#include "node.h"

#include "data7/callback.h"
#include "data7/metadefine.h"
#include "data7/str.h"
#include "data7/timer.h"
#include "memory/allocator.h"
#include "stream/sparser.h"
#include "stream/stream.h"

asNode::~asNode()
{
    if (parent_node_)
        parent_node_->RemoveChild(this);

    RemoveAllChildren();
}

void asNode::Update()
{
    if (DebugMemory & ARTS_DEBUG_UPDATEMEM)
        ALLOCATOR.SanityCheck();

    if (DebugMemory & ARTS_DEBUG_UPDATE)
    {
        if (const char* msg = VerifyTree())
            Quitf("Before update: %s", msg);
    }

#ifdef ARTS_DEV_BUILD
    if (TimingCount)
    {
        for (asNode* n = child_node_; n; n = n->next_node_)
        {
            if (n->IsNodeActive())
            {
                Timer t;
                n->Update();
                n->update_time_ += t.Time();
            }
        }
    }
    else
#endif
    {
        for (asNode* n = child_node_; n; n = n->next_node_)
        {
            if (n->IsNodeActive())
                n->Update();
        }
    }

    if (DebugMemory & ARTS_DEBUG_UPDATE)
    {
        if (const char* msg = VerifyTree())
            Quitf("After update: %s", msg);
    }
}

void asNode::Reset()
{
    for (asNode* n = child_node_; n; n = n->next_node_)
        n->Reset();
}

void asNode::ResChange(i32 width, i32 height)
{
    for (asNode* n = child_node_; n; n = n->next_node_)
        n->ResChange(width, height);
}

void asNode::UpdatePaused()
{}

void asNode::Load()
{
    const char* name = node_name_.get();

    if (!name || !std::strncmp(name, "_", 1))
        name = "default";

    string type = string(arts_formatf<128>(".%s", GetNodeType()));

    string path;
    path.SaveName(name, 0, "tune", type);

    Load(path.get());
}

void asNode::Save()
{
    const char* name = node_name_.get();

    if (!name || !std::strncmp(name, "_", 1))
        name = "default";

    string type = string(arts_formatf<128>(".%s", GetNodeType()));

    string path;
    path.SaveName(name, 0, "tune", type);

    Save(path.get());
}

#ifdef ARTS_DEV_BUILD
void asNode::AddWidgets(Bank* bank)
{
    current_bank_ = bank;

    bank->AddToggle("Active", &node_flags_, NODE_FLAG_ACTIVE);

    bank->AddButton("Save", [this] { Save(); });
    bank->AddButton("Load", [this] { Load(); });

    asCullable::AddWidgets(bank);
}

void asNode::OpenWidgets(char* /*arg1*/, bkWindow* /*arg2*/)
{}

void asNode::CloseWidgets()
{
    if (current_bank_)
        current_bank_->Off();
}

void asNode::AddButton(Bank* /*arg1*/, i32& /*arg2*/)
{}
#endif

b32 asNode::AddChild(asNode* child)
{
    if (!child)
    {
        Errorf("asNode::AddChild() - N=0");
        return false;
    }

    if (child->parent_node_)
    {
        Errorf("asNode::AddChilld() - Node already parented");
        return false;
    }

    child->parent_node_ = this;
    child->next_node_ = nullptr;

    asNode** last = &child_node_;

    while (*last)
        last = &(*last)->next_node_;

    *last = child;

    return true;
}

void asNode::AdoptChild(Ptr<asNode> child)
{
    child->SetNodeFlag(NODE_FLAG_OWNED);

    if (!AddChild(child.release()))
        Quitf("asNode::AdoptChild() - Failed to adopt child");
}

asNode* asNode::GetChild(i32 index)
{
    asNode* child = child_node_;

    for (i32 i = 1; i < index; ++i)
    {
        if (!child)
            break;

        child = child->next_node_;
    }

    return child;
}

asNode* asNode::GetLastChild()
{
    if (!parent_node_)
    {
        Errorf("asNode::GetLastChild() - Need ParentNode set.");

        return nullptr;
    }

    asNode* n = parent_node_->child_node_;

    if (n == this)
    {
        n = nullptr;
    }
    else
    {
        for (; n; n = n->next_node_)
        {
            if (n->next_node_ == this)
                break;
        }
    }

    return n;
}

const char* asNode::GetNodeType()
{
    return GetClass()->GetName();
}

asNode* asNode::GetParent(MetaClass* cls)
{
    asNode* n = parent_node_;

    for (; n; n = n->parent_node_)
    {
        if (n->GetClass()->IsSubclassOf(cls))
            break;
    }

    return n;
}

b32 asNode::InsertChild(i32 index, asNode* child)
{
    if (!child)
    {
        Errorf("asNode::InsertChild() - N==0");
        return false;
    }

    asNode** n = &child_node_;

    for (i32 i = 1; i != index; ++i)
    {
        if (*n == nullptr)
        {
            Errorf("asNode::InsertChild() - %d is out of range", index);
            return false;
        }

        n = &(*n)->next_node_;
    }

    child->parent_node_ = this;
    child->next_node_ = std::exchange(*n, child);

    return true;
}

b32 asNode::Load(const char* path)
{
    Ptr<Stream> input {arts_fopen(path, "r")};

    if (!input)
    {
        Errorf("asNode::Load(%s) failed.", path);
        return false;
    }

    StreamMiniParser parser {path, as_owner std::move(input)};

    GetClass()->Load(&parser, this);

    if (parser.GetErrorCount() != 0)
    {
        SetNodeFlag(NODE_FLAG_LOAD_ERROR);
        return false;
    }

    ClearNodeFlag(NODE_FLAG_LOAD_ERROR);
    return true;
}

i32 asNode::NumChildren()
{
    i32 count = 0;

    for (asNode* n = child_node_; n; n = n->next_node_)
        ++count;

    return count;
}

#ifdef ARTS_DEV_BUILD
void asNode::PerfReport(Stream* output, i32 indent)
{
    f32 self_update = update_time_;

    if (self_update != 0.0f)
    {
        for (asNode* n = child_node_; n; n = n->next_node_)
            self_update -= n->update_time_;
    }

    for (i32 i = 0; i < indent; ++i)
        arts_fprintf(output, "  ");

    arts_fprintf(output, "%s %s %.7f/%.7f\n", GetNodeType(), GetNodeName(), self_update, update_time_);

    for (asNode* n = child_node_; n; n = n->next_node_)
        n->PerfReport(output, indent + 1);
}
#endif

void asNode::RemoveAllChildren()
{
    for (asNode *n = std::exchange(child_node_, nullptr), *next = nullptr; n; n = next)
    {
        next = std::exchange(n->next_node_, nullptr);
        n->parent_node_ = nullptr;

        if (n->TestNodeFlag(NODE_FLAG_OWNED))
            delete n;
    }
}

b32 asNode::RemoveChild(asNode* child)
{
    for (asNode** n = &child_node_; *n; n = &(*n)->next_node_)
    {
        if (*n == child)
        {
            *n = child->next_node_;
            child->next_node_ = nullptr;
            child->parent_node_ = nullptr;
            return true;
        }
    }

    return false;
}

b32 asNode::RemoveChild(i32 index)
{
    if (!child_node_)
    {
        Errorf("asNode::RemoveChild() - No children!");
        return false;
    }

    asNode** n = &child_node_;

    for (i32 i = 1; i != index; ++i)
    {
        if (*n == nullptr)
        {
            Errorf("asNode::RemoveChild() - %d is out of range", index);
            return false;
        }

        n = &(*n)->next_node_;
    }

    asNode* child = *n;

    child->parent_node_ = nullptr;
    *n = std::exchange(child->next_node_, nullptr);

    return true;
}

#ifdef ARTS_DEV_BUILD
void asNode::ResetTime()
{
    update_time_ = 0.0f;

    for (asNode* n = child_node_; n; n = n->next_node_)
        n->ResetTime();
}
#endif

b32 asNode::Save(const char* path)
{
    Ptr<Stream> output {arts_fopen(path, "w")};

    if (!output)
    {
        Errorf("asNode::Save(%s) failed.", path);
        return false;
    }

    StreamMiniParser parser {path, as_owner std::move(output)};

    GetClass()->Save(&parser, this);

    return true;
}

void asNode::SetName(const char* name)
{
    node_name_ = name;
}

void asNode::SwitchTo(i32 idx)
{
    idx = std::clamp(idx, -1, NumChildren());

    i32 i = 1;
    for (asNode* n = child_node_; n; n = n->next_node_, ++i)
    {
        if (idx == -1 || idx == i)
            n->ActivateNode();
        else
            n->DeactivateNode();
    }
}

META_DEFINE_CHILD("asNode", asNode, asCullable)
{
    META_FIELD("Flags", node_flags_);
    META_FIELD("NextNode", next_node_);
    META_FIELD("ChildNode", child_node_);
    META_FIELD("ParentNode", parent_node_);
    META_FIELD("NodeName", node_name_);
}
