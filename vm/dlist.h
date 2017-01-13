#ifndef DLIST_H
#define DLIST_H

template <class Item>
class DListNode {
   public:
     DListNode (Item _item) {item=_item;next=last=NULL;}
     Item item;
     DListNode *next,*last;
};

template <class Item>
class DoubleList {
	public:
		DoubleList();
		~DoubleList();
		DListNode<Item>* Begin() {return first;}
		DListNode<Item>* End() {return last;}
		
		Item Remove(DListNode<Item> *toremove);
		DListNode<Item>* Append(Item newitem);
		DListNode<Item>* Prepend(Item newitem);
		int Length();

		DListNode<Item> *first,*last;
		int len;
};


template <class Item>
DoubleList<Item>::DoubleList() {
	len = 0;
	first = last = NULL;
}

template <class Item>
DoubleList<Item>::~DoubleList() {
	DListNode<Item> *it = first, *nt;
	while (it!=NULL) {
		nt = it->next;
		delete it;
		it = nt;
	}
}

template <class Item>
Item
DoubleList<Item>::Remove(DListNode<Item> *newnode) {
	len--;
	if (first==last && first==newnode)
		first = last = NULL;
	else if (first==newnode) {
		first = newnode->next;
		first->last = NULL;
	} else if (last==newnode) {
		last = newnode->last;
		last->next = NULL;
	} else {
	newnode->last->next = newnode->next;
	newnode->next->last = newnode->last;
	}
	Item r = newnode->item;
	delete newnode;
	return r;
}

template <class Item>
DListNode<Item>*
DoubleList<Item>::Append(Item newitem) {
	len++;
	DListNode<Item> *newnode = new DListNode<Item>(newitem);
	if (last==NULL) 
		return (first = last = newnode);
	last->next = newnode;
	newnode->last = last;
	last = newnode;
	return newnode;
}

template <class Item>
DListNode<Item>*
DoubleList<Item>::Prepend(Item newitem) {
	len++;
	DListNode<Item> *newnode = new DListNode<Item>(newitem);
	if (first==NULL) 
		return (last = first = newnode);
	first->last = newnode;
	newnode->next = first;
	first = newnode;
	return newnode;
}

template <class Item>
int
DoubleList<Item>::Length() {
	return len;
}

#endif
