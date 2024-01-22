// C++ Aufgabe Treemap V5 part 1 - treemap class, uses treempap_node
// Autor: Hartmut Schirmacher, BHT Berlin

#pragma once

// other includes
#include <memory>
#include <iostream>
#include <utility>
#include <tuple>
#include "treemap_node.h"
#include "treemap_iterator.h"

// forward declarations

namespace my
{
    template <typename K, typename T>
    class treemap;
}

template <typename KK, typename TT>
void swap(my::treemap<KK, TT> &lhs, my::treemap<KK, TT> &rhs);

namespace my
{

    /*
     * class treemap<K,T>
     * represents an associative container (dictionary) with unique keys
     * implemented by a binary search tree
     * - no balancing, no remove/erase operations
     * - no separate comparison operators, relies on K::operator==(), K::operator<(), etc.
     */
    template <typename K, typename T>
    class treemap
    {

    public:
        // public type aliases
        using key_type = K;
        using mapped_type = T;
        using value_type = std::pair<K, T>;
        using iterator = my::treemap_iterator<K, T>;

    public:
        // construct empty map

        treemap()
            : root_(), count_(0)
        {
        }

        // copyconstructor
        treemap(const treemap &other) : root_(copy_recursive(other.root_)), count_(other.count_) {}

        // number of keys in map
        size_t size() const;

        // how often is the element contained in the map?
        // (for this type of container, can only return 0 or 1)
        size_t count(const K &) const;

        // random read/write access to value by key
        T &operator[](const K &);

        // delete all (key,value) pairs in map
        void clear();

        // used for copy&move - declared in global namespace, not in my::
        template <typename KK, typename TT>
        friend void ::swap(treemap<KK, TT> &, treemap<KK, TT> &);

        // declaration assignment operator
        treemap<K, T> &operator=(treemap other);

        iterator begin();

        // iterator end();
        iterator end() const;
        iterator find(const K &) const;
        std::pair<iterator, bool> insert(const K &, const T &);
        std::pair<iterator, bool> insert_or_assign(const K &, const T &);

    protected:
        // the node type is only used internally - do not show publicly!
        using node = my::treemap_node<K, T>;    // from treemap_node.h
        using node_ptr = std::shared_ptr<node>; // for passing around pointers to nodes internally (!)

        // class attributes
        node_ptr root_;
        size_t count_;

        // add a new (key, value) pait into the tree
        // returns pair, consisting of:
        // - pointer to node containing the (key, value) pair
        // - bool
        //   - true if element was inserted;
        //   - false if key was already in map (will not overwrite existing value)
        std::pair<node_ptr, bool> insert_(const K &, const T &);

        // find element with specific key. returns nullptr if not found.
        node_ptr find_(const K &) const;
    };

    template <typename K, typename T>
    typename treemap<K, T>::iterator treemap<K, T>::find(const K &key) const
    {
        node_ptr found_node = find_(key);
        if (found_node != nullptr)
        {
            return iterator(found_node);
        }
        else
        {
            return end();
        }
    }

    template <typename K, typename T>
    typename treemap<K, T>::iterator treemap<K, T>::end() const
    {
        // Iterator that Points to end of tree with weakpointer to root
        return iterator(nullptr, root_);
    }

    template <typename K, typename T>
    typename treemap<K, T>::iterator treemap<K, T>::begin()
    {
        // test if tree is empty - in case return end()(nullptr)
        if (root_ == nullptr)
        {
            return end();
        }

        // find min none with recursive method find_min
        node_ptr min_node = root_->find_min();

        // Iterator that points on min_node
        return iterator(min_node);
    }

    template <typename K, typename T>
    void
    treemap<K, T>::clear()
    {
        // root = nullptr da alle smartpointer destruktor aufrufen
        root_ = nullptr;
        count_ = 0;
    }

    // random write access to value by key
    // if key is not in map, insert new (key, T())
    template <typename K, typename T>
    T &
    treemap<K, T>::operator[](const K &key)
    {
        // Versuchen Sie, den Schlüssel zu finden
        node_ptr node = find_(key);

        // Wenn der Schlüssel nicht gefunden wurde, wird an ein neuer Knoten 
        if (!node)
        {
            std::tie(node, std::ignore) = insert_(key, T());
        }

        // Geben Sie den Wert des gefundenen oder eingefügten Knotens zurück
        return node->value_.second;
    }

    // number of elements in map (nodes in tree)
    template <typename K, typename T>
    size_t treemap<K, T>::size() const
    {

        return count_;
    }

    // add a new element into the tree
    // returns:
    // - pointer to element
    // - true if element was inserted; false if key was already in map
    template <typename K, typename T>
    std::pair<typename treemap<K, T>::node_ptr, bool>
    treemap<K, T>::insert_(const K &key, const T &mapped)
    {
        // Wenn root nllprt, erstellen eines neues knotens und zähler erhöhen
        if (!root_)
        {
            root_ = std::make_shared<node>(key, mapped);
            count_++;
            return std::make_pair(root_, true);
        }
        // Ansonsten rekursive insert Methode des Knotens
        else
        {
            count_++;
            return root_->insert(key, mapped);
        }
    }
    // find element with specific key. returns end() if not found.
    template <typename K, typename T>
    typename treemap<K, T>::node_ptr
    treemap<K, T>::find_(const K &key) const
    {
        node_ptr current = root_;

        while (current != nullptr)
        {
            if (key < current->value_.first)
            {
                current = current->left_;
            }
            else if (key > current->value_.first)
            {
                current = current->right_;
            }
            else
            {
                return current;
            }
        }

        return nullptr;
    }

    // how often is the element contained in the map?
    template <typename K, typename T>
    
    size_t treemap<K, T>::count(const K &key) const
    {
        return find_(key) == nullptr ? 0 : 1;
    }

    template <typename K, typename T>
    std::pair<typename treemap<K, T>::iterator, bool> treemap<K, T>::insert(const K &key, const T &value)
    {
        auto existing_node = find_(key);
        if (existing_node != nullptr)
        {
            // falls key vorhanden wird nichts eingefühgt
            return std::make_pair(iterator(existing_node), false);
        }
        // Einfügen eines neuen elements
        auto insert_result = insert_(key, value);
        return std::make_pair(iterator(insert_result.first), insert_result.second);
    }

    template <typename K, typename T>
    std::pair<typename treemap<K, T>::iterator, bool> treemap<K, T>::insert_or_assign(const K &key, const T &value)
    {
        auto existing_node = find_(key);
        if (existing_node != nullptr)
        {
            // falls der schlüssel gefunden wurde wird der wert aktualisiert
            existing_node->value_.second = value;
            // gibt trotzdem false zurück da ja kein neuer knoten erzeugt wurde sondern nur value überschrieben
            return std::make_pair(iterator(existing_node), false);
        }
        // Einfügen eines neuen elements
        auto insert_result = insert_(key, value);
        return std::make_pair(iterator(insert_result.first), insert_result.second);
    }


    // copy recursive
template <typename K, typename T>
static std::shared_ptr<treemap_node<K, T>> copy_recursive(const std::shared_ptr<treemap_node<K, T>> &original_node)
{
    
    if (!original_node)
    {
        return nullptr;
    }

    // Erstellen eines neuen Knotens, der eine Kopie des aktuellen Knotens ist.
    // erstellen neuer knoten, kopie des aktuellen knotens
    auto new_node = std::make_shared<treemap_node<K, T>>(original_node->value_.first, original_node->value_.second);
    
    
    // rekursiver aufruf zur deepcopy des linken teilbazmns
    new_node->left_ = copy_recursive(original_node->left_);

    // setzen des pointers auf den erltern knoten
    if (new_node->left_)
    {
        new_node->left_->up_ = new_node;
    }

   // rekursiver aufruf zur deepcopy des rechten teilbazmns
    new_node->right_ = copy_recursive(original_node->right_);

    // setzen des pointers auf den erltern knoten
    if (new_node->right_)
    {
        new_node->right_->up_ = new_node;
    }


    return new_node;
}



    template <typename K, typename T>
    treemap<K, T> &treemap<K, T>::operator=(treemap rhs)
    {
        swap(*this, rhs);
        return *this;
    }

} // namespace my

// swap contents of two trees
// this is defined in the global namespace, for reasons... (see StackOverflow)
template <typename KK, typename TT>
void swap(my::treemap<KK, TT> &lhs, my::treemap<KK, TT> &rhs)
{
    std::swap(lhs.root_, rhs.root_);
    std::swap(lhs.count_, rhs.count_);
}
