#include "user/node.h"

Node::Node()
{
    parent = nullptr;
}

void Node::add_child(Node* child)
{
    children.push_back(child);
    child->reparent(this);
}

void Node::remove_child(Node* child)
{
    for(int i = 0; i < children.size(); i++)
    {
        if(children[i] == child)
        {
            children.erase(children.begin() + i);
            child->reparent(nullptr);
            return;
        }
    }
}

void Node::reparent(Node* new_parent)
{
    if(parent)
    {
        parent->remove_child(this);
    }

    if(new_parent)
    {
        new_parent->add_child(this);
    }
}

void Node::free()
{
    if(parent)
    {
        parent->remove_child(this);
    }

    for(int i = 0; i < children.size(); i++)
    {
        children[i]->free();
    }
}

void Node::start()
{
    // Override this method with start logic
}

void Node::update()
{
    // Override this method with update logic
}
