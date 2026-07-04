#include "Node.h"
using namespace std;
Node::Node(int freq) : frequency(freq) {}
Node::~Node() {}
int Node::getFrequency() const { return frequency; }

LeafNode::LeafNode(char ch, int freq) : Node(freq), data(ch) {}
bool LeafNode::isLeaf() const { return true; }
char LeafNode::getData() const { return data; }

InternalNode::InternalNode(Node* l, Node* r) 
    : Node(l->getFrequency() + r->getFrequency()), left(l), right(r) {}

InternalNode::~InternalNode() {
    delete left;
    delete right;
}
bool InternalNode::isLeaf() const { return false; }
Node* InternalNode::getLeft() const { return left; }
Node* InternalNode::getRight() const { return right; }
