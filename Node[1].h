#ifndef NODE_H
#define NODE_H

class Node {
protected:
    int frequency;

public:
    Node(int freq);
    virtual ~Node();

    virtual bool isLeaf() const = 0;
    int getFrequency() const;
};

class LeafNode : public Node {
private:
    char data;

public:
    LeafNode(char ch, int freq);
    bool isLeaf() const ;
    char getData() const;
};

class InternalNode : public Node {
private:
    Node* left;
    Node* right;

public:
    InternalNode(Node* l, Node* r);
    ~InternalNode();

    bool isLeaf() const ;
    Node* getLeft() const;
    Node* getRight() const;
};

// Functor for Priority Queue to sort Node pointers correctly
struct NodePtrCompare {
    bool operator()(const Node* lhs, const Node* rhs) const {
        return lhs->getFrequency() > rhs->getFrequency();
    }
};

#endif
