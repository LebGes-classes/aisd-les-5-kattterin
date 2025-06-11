#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>

using namespace std;

template <typename T>
struct BinomialNode {
    T value;
    int priority;
    vector<BinomialNode*> children;
    int degree;

    BinomialNode(T val, int prio) : value(val), priority(prio), degree(0) {}
};

template <typename T>
class BinomialHeap {
private:
    vector<BinomialNode<T>*> trees;

    BinomialNode<T>* mergeTrees(BinomialNode<T>* tree1, BinomialNode<T>* tree2) {
        if (tree1->priority > tree2->priority) {
            swap(tree1, tree2);
        }
        tree1->children.push_back(tree2);
        tree1->degree++;
        return tree1;
    }

    void mergeHeaps(vector<BinomialNode<T>*>& heap1, vector<BinomialNode<T>*>& heap2) {
        vector<BinomialNode<T>*> newHeap;
        auto it1 = heap1.begin();
        auto it2 = heap2.begin();
        BinomialNode<T>* carry = nullptr;

        while (it1 != heap1.end() || it2 != heap2.end() || carry != nullptr) {
            BinomialNode<T>* tree1 = (it1 != heap1.end()) ? *it1 : nullptr;
            BinomialNode<T>* tree2 = (it2 != heap2.end()) ? *it2 : nullptr;

            if (carry) {
                if (tree1 && tree1->degree == carry->degree) {
                    carry = mergeTrees(tree1, carry);
                    it1++;
                } else if (tree2 && tree2->degree == carry->degree) {
                    carry = mergeTrees(tree2, carry);
                    it2++;
                } else {
                    newHeap.push_back(carry);
                    carry = nullptr;
                }
            } else {
                if (tree1 && tree2 && tree1->degree == tree2->degree) {
                    carry = mergeTrees(tree1, tree2);
                    it1++;
                    it2++;
                } else {
                    if (tree1 && (!tree2 || tree1->degree < tree2->degree)) {
                        newHeap.push_back(tree1);
                        it1++;
                    } else if (tree2) {
                        newHeap.push_back(tree2);
                        it2++;
                    }
                }
            }
        }

        heap1 = newHeap;
    }

    BinomialNode<T>* findMinNode() {
        if (trees.empty()) return nullptr;

        BinomialNode<T>* minNode = trees[0];
        for (auto tree : trees) {
            if (tree->priority < minNode->priority) {
                minNode = tree;
            }
        }
        return minNode;
    }

public:
    void enqueue(T value, int priority) {
        BinomialHeap<T> tempHeap;
        tempHeap.trees.push_back(new BinomialNode<T>(value, priority));
        mergeHeaps(trees, tempHeap.trees);
    }

    T dequeue() {
        if (trees.empty()) {
            throw runtime_error("Heap is empty");
        }

        BinomialNode<T>* minNode = findMinNode();
        T minValue = minNode->value;

        // Remove minNode from trees
        trees.erase(remove(trees.begin(), trees.end(), minNode), trees.end());

        // Create new heap from minNode's children
        BinomialHeap<T> tempHeap;
        for (auto child : minNode->children) {
            tempHeap.trees.push_back(child);
        }
        reverse(tempHeap.trees.begin(), tempHeap.trees.end());

        // Merge with remaining trees
        mergeHeaps(trees, tempHeap.trees);

        delete minNode;
        return minValue;
    }

    T peek() {
        if (trees.empty()) {
            throw runtime_error("Heap is empty");
        }
        return findMinNode()->value;
    }

    void decreasePriority(T value, int newPriority) {
        vector<pair<T, int>> elements;
        while (!trees.empty()) {
            T val = dequeue();
            elements.emplace_back(val, val == value ? newPriority : val);
        }
        for (auto& elem : elements) {
            enqueue(elem.first, elem.second);
        }
    }

    void merge(BinomialHeap<T>& other) {
        mergeHeaps(trees, other.trees);
        other.trees.clear();
    }

    bool isEmpty() const {
        return trees.empty();
    }

    ~BinomialHeap() {
        for (auto tree : trees) {
            deleteTree(tree);
        }
    }

private:
    void deleteTree(BinomialNode<T>* node) {
        if (!node) return;
        for (auto child : node->children) {
            deleteTree(child);
        }
        delete node;
    }
};