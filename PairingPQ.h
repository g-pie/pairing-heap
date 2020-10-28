// Project identifier: 19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the 'priority_queue' ADT implemented as a pairing heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure to
            // initialize it here.
            explicit Node(const TYPE &val)
                : elt{ val }, child{ nullptr }, sibling{ nullptr }, previous{ nullptr }
            {}

            // Description: Allows access to the element at that Node's position.
			// There are two versions, getElt() and a dereference operator, use
			// whichever one seems more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data members of this
            // Node class from within the PairingPQ class. (ie: myNode.elt is a legal
            // statement in PairingPQ's add_node() function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            // TODO: Add one extra pointer (parent or previous) as desired.
            Node* previous;
    }; // Node


    // Description: Construct an empty priority_queue with an optional comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
        count = 0;
        root = nullptr;
    } // PairingPQ()


    // Description: Construct a priority_queue out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    // TODO: when you implement this function, uncomment the parameter names.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
        count = 0;
        root = nullptr;        
        while (start != end) {
            push(*start);
            ++start;
        }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ& other) :
        BaseClass{ other.compare } {
        count = 0;
        if (other.size() == 0) {
            root = nullptr;
        }
        else {
            root = other.root;
            std::deque<Node*> dq;
            dq.push_front(other.root);
            while (!dq.empty()) {
                Node* current = dq.front();
                if (current->child != nullptr) {
                    dq.push_back(current->child);
                }
                if (current->sibling != nullptr) {
                    dq.push_back(current->sibling);
                }
                dq.pop_front();
                push(current->elt);
            }
        }
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    // TODO: when you implement this function, uncomment the parameter names.
    PairingPQ& operator=(const PairingPQ& rhs) {
        // TODO: Implement this function.
        PairingPQ temp(rhs);
        std::swap(count, temp.count);
        std::swap(root, temp.root);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        std::deque<Node*> dq;
        dq.push_front(root);
        // check if it's an empty pq
        if (size() != 0) {
            while (!dq.empty()) {
                Node* current = dq.front();
                if (current->child != nullptr) {
                    dq.push_back(current->child);
                }
                if (current->sibling != nullptr) {
                    dq.push_back(current->sibling);
                }
                dq.pop_front();
                current->child = nullptr;
                current->previous = nullptr;
                current->sibling = nullptr;
                delete current;
            }
            count = 0;
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the priority_queue are out of order and
    //              'rebuilds' the priority_queue by fixing the priority_queue invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        std::deque<Node*> dq;
        dq.push_front(root);
        root = nullptr;
        while (!dq.empty()) {
            Node* current = dq.front();
            if (current->child != nullptr) {
                dq.push_back(current->child);
            }
            if (current->sibling != nullptr) {
                dq.push_back(current->sibling);
            }
            dq.pop_front();
            current->previous = nullptr;
            current->sibling = nullptr;
            current->child = nullptr;
            root = meld(root, current, count, 0);
        }
        // TODO: Implement this function.
    } // updatePriorities()


    // Description: Add a new element to the priority_queue. This is almost done,
    //              in that you should implement push functionality in the addNode()
    //              function, and this function should call addNode().
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    virtual void push(const TYPE & val) {
        // TODO: Implement this function.
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element from
    //              the priority_queue.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the priority_queue is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // just root and child
        if (size() == 2) {
            Node* temp = root->child;
            temp->previous = nullptr;
            temp->sibling = nullptr;
            temp->child = nullptr;
            delete root;
            root = temp;
            count = 1;
            return;
        }
        // if it is just the root
        else if (size() == 1) {
            delete root;
            count = 0;
            return;
        }
        else if (size() == 0) {
            root = nullptr;
            count = 0;
            return;
        }
        // if it's not just the root and child
        else if (size() > 1) {
            std::deque<Node*> children;

            children.push_back(root->child);
            delete root;
            root = nullptr;
            // push all the children into the deque
            int i = 0;
            while (children[i]->sibling != nullptr) {
                children.push_back(children[i]->sibling);
                ++i;
            }
            // now meld the children, 2 at a time until deque.size() == 1
            if (children.size() > 1) {
                while (children.size() > 1) {
                    //Node* temp1 = children.front();
                    //Node* temp2 = children[1];
                    children[0]->previous = nullptr;
                    children[0]->sibling = nullptr;
                    children[1]->sibling = nullptr;
                    children[1]->previous = nullptr;
                    Node* temp1 = children[0];
                    Node* temp2 = children[1];
                    children.pop_front();
                    children.pop_front();
                    children.push_back(meld(temp1, temp2, count, 0));
                }
                root = children.front();
            }
            else if (children.size() == 1){
                children.front()->previous = nullptr;
                root = children.front();
            }
            --count;
        }
        
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE & top() const {
        // TODO: Implement this function
        return root->elt;
    } // top()


    // Description: Get the number of elements in the priority_queue.
    // Runtime: O(1)
    virtual std::size_t size() const {
        return count;
    } // size()

    // Description: Return true if the priority_queue is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        return (size() == 0);
    } // empty()


    // Description: Updates the priority of an element already in the priority_queue by
    //              replacing the element refered to by the Node with new_value.
    //              Must maintain priority_queue invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more extreme
    //               (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    // TODO: when you implement this function, uncomment the parameter names.
    void updateElt(Node* node, const TYPE & new_value) {
        // TODO: Implement this function
        Node* current = root;
        std::deque<Node*> dq;
        int counter = 0;
        dq.push_front(current);

        while (counter != int(size())) {
            current = dq.front();
            dq.pop_front();
            if (current->child != nullptr) {
                dq.push_back(current->child);
            }
            if (current->sibling != nullptr) {
                dq.push_back(current->sibling);
            }
            if (current == node) {
                if (current->sibling != nullptr) {
                    current->sibling->previous = current->previous;
                    if (isLeftmost(current)) {
                        current->previous->child = current->sibling;
                    }
                    else {
                        current->previous->sibling = current->sibling;
                    }
                    current->sibling = nullptr;
                    current->previous = nullptr;
                }
                current->elt = new_value;
                if (current != root) {
                    root = meld(root, current, count, 0);
                }
                return;
            }
            ++counter;
        }
    } // updateElt()


    // Description: Add a new element to the priority_queue. Returns a Node* corresponding
    //              to the newly added element.
    // Runtime: O(1)
    // TODO: when you implement this function, uncomment the parameter names.
    // NOTE: Whenever you create a node, and thus return a Node *, you must be sure to
    //       never move or copy/delete that node in the future, until it is eliminated
    //       by the user calling pop().  Remember this when you implement updateElt() and
    //       updatePriorities().
    Node* addNode(const TYPE & val) {
        // TODO: Implement this function
        Node* newNode = new Node(val);
        newNode->elt = val;
        newNode->child = nullptr;
        newNode->sibling = nullptr;
        newNode->previous = nullptr;
        if (empty()) {
            root = newNode;
            ++count;
            return root;
        }
        root = meld(root, newNode, count, 1);
        return newNode;
    } // addNode()


private:
    // TODO: Add any additional member functions or data you require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap papers).
    Node* meld(Node* a, Node* b, unsigned int count1, unsigned int count2) {
        if (a == nullptr) {
            return b;
        }
        if (b == nullptr) {
            return a;
        }
        else if (a == nullptr && b == nullptr) {
            count = 0;
            return nullptr;
        }
        count = count1 + count2;
        // b is higher priority
        if (this->compare(a->elt, b->elt)) {
            a->previous = b;
            if (b->child != nullptr) {
                a->sibling = b->child;
                b->child->previous = a;
            }
            b->child = a;
            b->sibling = nullptr;
            return b;
        }
        // if a is higher priority
        else {
            b->previous = a;
            if (a->child != nullptr) {
                b->sibling = a->child;
                a->child->previous = b;
            }
            a->child = b;
            a->sibling = nullptr;
            return a;
        }
    }
    bool isLeftmost(Node* a) {
        if (a->previous->child == a) {
            return 1;
        }
        else {
            return 0;
        }
    }
    unsigned int count;
    Node* root;
};


#endif // PAIRINGPQ_H
