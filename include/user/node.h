#ifndef NODE_H
#define NODE_H
#include <vector>

class Node {
    public:
        Node();
        std::vector<Node*> children;
        Node* parent;
        void add_child(Node* child);
        void remove_child(Node* child);
        void reparent(Node* new_parent);
        void free();

        void start();
        void update();
};
#endif