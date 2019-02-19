// $Id: listmap.h,v 1.14 2018-02-20 14:59:38-08 - - $

#ifndef __LISTMAP_H__
#define __LISTMAP_H__

#include "xless.h"
#include "xpair.h"

//listmap uses a node head and node back that are both markers.
//They hold no value and only indicate the start and end of a
//list. Kind of like bread to a sandwich
template <typename Key, typename Value, class Less=xless<Key>>
class listmap {
   public:
      using key_type = Key;
      using mapped_type = Value;
      using value_type = xpair<const key_type, mapped_type>;
   private:
      Less less;
      struct node;
      node front;
      node mapend;
      node* head; //These two probably aren't needed, but i had
      node* back; //already based functions off of head at this
      struct link { //point
         node* next{};
         node* prev{};
         link(){};
         link (node* next, node* prev): next(next), prev(prev){}
      };
      struct node: link {
         value_type value{};
         node(){};
         node (node* next, node* prev, const value_type&);
      };
      node* anchor() { return static_cast<node*> (&anchor_); }
      link anchor_ {anchor(), anchor()};
   public:
      class iterator;
      listmap(){};
      listmap (const listmap&);
      listmap& operator= (const listmap&);
      ~listmap();
      iterator insert (const value_type&);
      iterator find (const key_type&);
      iterator erase (iterator position);
      //Method to print map. I know it shouldn't really be defined
      //here, but in terms of making the program function, it was
      //easier to do it here.
      void print(){
        node* iter = head->next;
        while(iter != back){
                cout << iter->value.first << " = " << 
                iter->value.second << endl;
                iter = iter->next;
        }
      }
      //Method that handles = value input printing
      void print_value(string that){
        node* iter = head->next;
        while(iter != back){
        if(iter->value.second == that){
                cout << iter->value.first << " = "
                << iter->value.second << endl;
        }
        iter = iter->next;
        }
      }
      iterator begin() { return head; }
      iterator end() { return back; }
      bool empty() const { return begin() == end(); }
      //Initializes the map
      void initialize(){
      front.next = &mapend;
      front.prev = &mapend;
      mapend.next = &front;
      mapend.prev = &front;
      head = &front;
      back = &mapend;
      }
};


template <typename Key, typename Value, class Less>
class listmap<Key,Value,Less>::iterator {
   private:
      friend class listmap<Key,Value,Less>;
      listmap<Key,Value,Less>::node* where {nullptr};
      iterator (node* where): where(where){};
   public:
      iterator(){}
      value_type& operator*();
      value_type* operator->();
      iterator& operator++(); //++itor
      iterator& operator--(); //--itor
      void erase();
      bool operator== (const iterator&) const;
      bool operator!= (const iterator&) const;
};

#include "listmap.tcc"
#endif

