#include "disjoint-set.hpp"

template <typename T>
Set<T>::Set(T elem){
  _representative = new Node<T>;
  _representative->elem = elem;
  _representative->parent = NULL;
  _representative->rank = 0;
}

template <typename T>
Node<T> *Set<T>::Find(){
  Node<T> *representative = _representative;
  while(representative->parent != NULL) {
    representative = representative->parent;
  }

  Node<T> *child = _representative;
  while(child->parent != NULL){
    auto next = child->parent;
    child->parent = representative;
    child = next;
  }

  return representative;
}

template <typename T>
void Set<T>::Union(Set other){
  Node<T> *x = Find();
  Node<T> *y = other.Find();

  if (x == y)
    return;

  if (x->rank > y->rank) {
    Node<T> *z = x;
    x = y;
    y = z;
  }

  x->parent = y;

  if (x->rank == y->rank)
    y->rank++;
}