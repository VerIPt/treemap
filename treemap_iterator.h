// C++ Aufgabe Treemap V5.2 - treemap_iterator, alias treemap::iterator
// Autor: Hartmut Schirmacher, Berliner Hochschule für Technik

#pragma once

// an iterator references a treemap_node, so it must know about it
// please note that the iterator does *not* need to know the treemap itself! (except for the "friend" stateent below)
#include "treemap_node.h"
#include "treemap_node.h"
#include <iostream>
using namespace std;

namespace my
{

    // forward declaration of treemap, just in case you want to keep a pointer to a treemap or such
    template <typename K, typename T>
    class treemap;

    // iterator: references a node within the tree
    template <typename K, typename T>
    class treemap_iterator
    {
    protected:
        // treemap is a friend, can call protected constructor
        // template<typename KK, typename TT>
        friend class treemap<K, T>;
        friend class treemap_node<K, T>;


        // weakpointer änderung für non owning stuff
        using node_ptr = std::weak_ptr<treemap_node<K, T>>;


        // construct iterator referencing a speciic node
        // - only treemap shall be allowed to do so
        treemap_iterator(node_ptr node)
            : node_(node) {}


        // Konstruktor der einen nullpointer akzeptiert und Root nimmt
        treemap_iterator(std::nullptr_t, node_ptr root)
            : node_(), root_(root) {}


        // non-owning reference to the actual node
        node_ptr node_;
        // non_owning reference to the previous node
        node_ptr previous_node_;
        // non_owning reference to the root node
        node_ptr root_;

    public:
        // type aliases, should be exactly the same as for treemap itself
        using key_type = K;
        using mapped_type = T;
        using value_type = std::pair<K, T>;
        using node = my::treemap_node<K, T>; // from treemap_node.h

        // access data of referenced map element (node)
        value_type &operator*()
        {
            {
                auto locked_node = node_.lock();
                assert(locked_node != nullptr); // node != null
                return locked_node->value_;
            }
        }
        value_type *operator->()
        {
            auto locked_node = node_.lock();
            assert(locked_node != nullptr); // node != null
            return &(locked_node->value_);
        }

        // two iterators are equal if they point to the same node
        bool operator==(const treemap_iterator &rhs) const
        {
            auto rhs_locked_node = rhs.node_.lock();
            auto locked_node = node_.lock();
            return locked_node == rhs_locked_node;
        }

        bool operator!=(const treemap_iterator &rhs) const
        {
            auto rhs_locked_node = rhs.node_.lock();
            auto locked_node = node_.lock();
            return locked_node != rhs_locked_node;
        }

        // next element in map, pre-increment
        // note: must modify self!
        treemap_iterator &operator++()
        {
            auto locked_node = node_.lock();
            previous_node_ = node_;

            // end oder root bei leerem bauem anwednung verboooten!!!
            assert(locked_node != nullptr);

            // Wenn es einen rechten Teilbaum gibt, rechts und dann so weit wie möglich nach links
            if (locked_node->right_)
            {
                locked_node = locked_node->right_;
                // falls links dann ganz nach unten
                while (locked_node->left_)
                {
                    locked_node = locked_node->left_;
                }
            }
            else
            {

                auto parent = locked_node->up_.lock();
                // solange nach oben wie es ein elternknoten und wir von rechts kommen
                while (parent && locked_node == parent->right_)
                {
                    locked_node = parent;
                    parent = parent->up_.lock();
                }
                locked_node = parent;
            }

            node_ = locked_node;
            return *this;
        }

        // prev element in map, pre-decrement
        // note: must modify self!
        treemap_iterator &operator--()
        {
            auto locked_node = node_.lock();

            if (!locked_node)
            {
                auto previous_lock = previous_node_.lock();

                // Wenn der Iterator bei end() ist, finde das größte Element
                if (previous_lock)
                {
                    // falls der ++operator verwendet wurde kann mit previous zurück gegangen werden
                    locked_node = previous_lock;
                }

                else
                {
                    // falls mit end() -> viva la root und dann größtes element suchen
                    locked_node = root_.lock();
                    while (locked_node && locked_node->right_)
                    {
                        locked_node = locked_node->right_;
                    }
                }
            }
            else
            {
                // Wenn es einen linken Teilbaum gibt, gehe nach links und dann so weit wie möglich nach rechts
                if (locked_node->left_)
                {
                    locked_node = locked_node->left_;
                    while (locked_node->right_)
                    {
                        locked_node = locked_node->right_;
                    }
                }
                else
                {
                    // Gehe nach oben, bis wir von rechts kommen
                    auto parent = locked_node->up_.lock();
                    while (parent && locked_node == parent->left_)
                    {
                        locked_node = parent;
                        parent = parent->up_.lock();
                    }
                    locked_node = parent;
                }
            }

            node_ = locked_node;
            return *this;
        }

    }; // class iterator

} // my::
