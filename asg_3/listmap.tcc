// $Id: listmap.tcc,v 1.11 2018-01-25 14:19:14-08 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
}

//Insert completed

//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   //Searches for key to see if it exists and overwrites value
   if(find(pair.first) != back){
        find(pair.first)->second = pair.second;
        return find(pair.first);
   }
   //Checks if list is empty and inserts it between head and anchor
   if(head->next == back){
        node* newnode = new node(back,head,pair);
        head->next = newnode;
        back->prev = newnode;
        return newnode; 
   }
   node* it = head->next;
   //Inserts new node lexicohraphically
   while(it != back){
//std::string(pair.first)<std::string(it->value.first)
        if(less(pair.first,it->value.first)==true){
                node* newnode = new node(it,it->prev,pair);
                it->prev->next = newnode;
                it->prev = newnode;
                return newnode;
        }
        it = it->next;
        if(it == back){
                node* largest = new node(back,back->prev,pair);
                back->prev->next = largest;
                back->prev = largest;
                return largest;
        }
   }
   return end();
}

// Find completed
//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
  DEBUGF ('l', that);
   node* it = head->next;
   while(it != back){
        if(it->value.first == that){
                return it;
        }
        it = it->next;
   }
   return end();
}

//Erase completed
//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   DEBUGF ('l', &*position);
   position.where->prev->next = position.where->next;
   position.where->next->prev = position.where->prev;
   iterator it = position.where->next;
   delete position.where;
   return it;
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   DEBUGF ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   DEBUGF ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   DEBUGF ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   DEBUGF ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

