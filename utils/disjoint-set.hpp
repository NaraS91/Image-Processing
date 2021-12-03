#ifndef DISJOINT_SET_CPP
#define DISJOINT_SET_CPP
#include <vector>


template<typename T>
struct Node{
  T elem;
  size_t rank;
  Node *parent;
};

template <typename T>
class Set{
  public:
    Set(T elem);
    Node<T> *Find();
    void Union(Set other);

  private:
    Node<T> *_representative;
};

#endif
