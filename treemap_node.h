// C++ Aufgabe Treemap V5 part 1 - treemap_node, alias treemap::node
// Autor: Hartmut Schirmacher, BHT Berlin

#pragma once

#include <memory>

namespace my
{

    // this is the template for a node in the treemap's tree
    // please note that the node does not need to know anything about the treemap itself (or about the iterator, later)
    template <typename K, typename T>
    class treemap_node
        : public std::enable_shared_from_this<treemap_node<K, T>>
    {

    public:
        // aloas to shorten things
        using node = treemap_node<K, T>;
        using node_ptr = std::shared_ptr<node>;

        // public attributes
        std::pair<K, T> value_;
        std::weak_ptr<node> up_;
        node_ptr left_, right_;

        treemap_node(K key, T mapped, node_ptr up)
            : value_(std::make_pair(key, mapped)), up_(up), left_(), right_()
        {
        }

        // eingefügt für rooot
        treemap_node(K key, T mapped)
            : value_(std::make_pair(key, mapped))
        {
        }

        // try to insert new (key,mapped) node in tree, return (new node, true)
        // if key already in tree, do not overwrite, just return (existing node, false)
        std::pair<node_ptr, bool> insert(K key, T mapped)
        {
            // Wenn der Schlüssel kleiner als der aktuelle Schlüssel ist
            if (key < value_.first)
            {
                // Wenn kein linker Knoten existiert, wird ein neuer knoten erstellt
                if (!left_)
                {
                    left_ = std::make_shared<node>(key, mapped, this->shared_from_this());
                    return std::make_pair(left_, true);
                }
                // Ansonsten wird rekursion fortgesetzt
                else
                {
                    return left_->insert(key, mapped);
                }
            }
            // Wenn der Schlüssel größer als der aktuelle Schlüssel ist
            else if (key > value_.first)
            {
                // Wenn kein rechter Knoten existiert, wird eine neuer knoten erstellt
                if (!right_)
                {
                    right_ = std::make_shared<node>(key, mapped, this->shared_from_this());
                    return std::make_pair(right_, true);
                }
                // Ansonsten fahren Sie mit der Rekursion fort
                else
                {
                    // Ansonsten wird rekursion fortgesetzt
                    return right_->insert(key, mapped);
                }
            }
            // Wenn der Schlüssel bereits existiert, wird der aktuelle knoten zurück gegeben
            else
            {
                return std::make_pair(this->shared_from_this(), false);
            }
        }
        // rekursive methode zum finden des knoten mit dem kleinsten wert
        node_ptr find_min()
        {
            if (left_)
            {
                return left_->find_min();
            }
            else
            {
                return this->shared_from_this();
            }
        }

    }; // class treemap_node

} // my::