#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP
#include <vector>

namespace utils {
  template<typename T>
  struct Node {
      T elem;
      size_t rank;
      size_t size;
      Node* parent;
  };

  template<typename T>
  bool operator==(const Node<T> &lhs, const Node<T> &rhs)
  {
      return lhs.elem == rhs.elem && lhs.parent == rhs.parent;
  }

  template <typename T>
  class Set{
    public:
      Set(T elem) {
          _representative = new Node<T>;
          _representative->size = 1;
          _representative->elem = elem;
          _representative->parent = NULL;
          _representative->rank = 0;
      }

      Set() {
          _representative = NULL;
      }

      ~Set() {
          delete _representative;
      }

      Node<T> *Find() {
          Node<T>* representative = _representative;
          while (representative->parent != NULL) {
              representative = representative->parent;
          }

          Node<T>* child = _representative;
          while (child->parent != NULL) {
              auto next = child->parent;
              child->parent = representative;
              child = next;
          }

          return representative;
      }

      void Union(Set other) {
          Node<T>* x = Find();
          Node<T>* y = other.Find();

          if (x == y)
              return;

          if (x->rank > y->rank) {
              Node<T>* z = x;
              x = y;
              y = z;
          }

          x->parent = y;

          y->size += x->size;

          if (x->rank == y->rank)
              y->rank++;
      }

      size_t Size() {
          Node<T>* rep = Find();
          return rep->size;
      }

    private:
      Node<T> *_representative;
  };
}
#endif
