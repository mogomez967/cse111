// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   while(anchor() != anchor()->next)
   {
   		this->erase(this->begin());
   }
}

// helper void erase()
template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t, mapped_t, less_t>::iterator::erase()
{
	this->where = nullptr;
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);

   if(this->empty())
   {
   		node *new_node = new node(anchor(), anchor(), pair);
   		anchor()->next = new_node;
   		anchor()->prev = new_node;
   		return new_node;
   }

   iterator itor = this->find(pair.first);
   if(this->end() != itor)
   {
   		itor->second = pair.second;
   		return itor;
   }
   else
   {
   		node *current = anchor()->next;
   		while(current != anchor() && less(current->value.first, pair.first))
   		{
   			current = current->next;
   		}

   		node *sec_node = new node(current, current->prev, pair);
   		current->prev->next = sec_node;
   		current->prev = sec_node;
   		return sec_node;
   }
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   // DEBUGF ('l', that);
   node *itor = anchor()->next;
   while(itor != anchor())
   {
   		if(!less(itor->value.first, that) && !less(that, itor->value.first))
   		{
   			return itor;
   		}
   		itor = itor->next;
   }
   return itor;
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   node *itor = anchor()->next;

   if(itor != anchor())
   {
   		itor->prev->next = itor->next;
   		itor->next->prev = itor->prev;
   		itor->next = nullptr;
   		itor->prev = nullptr;
   		delete itor;
   		position.erase();
   }

   DEBUGF ('l', &*position);
   return iterator();
}
