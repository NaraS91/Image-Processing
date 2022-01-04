#ifndef DISJOINT_SET_CPP
#define DISJOINT_SET_CPP
#include <vector>

namespace utils {
  template<typename T>
  struct Node{
    T elem;
    size_t rank;
    size_t size;
    Node *parent;
  };

  template <typename T>
  class Set{
    public:
      Set(T elem);
      ~Set();
      Node<T> *Find();
      void Union(Set other);
      size_t Size();

    private:
      Node<T> *_representative;
  };
}

#endif
