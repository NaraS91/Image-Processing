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
    Set(T elem){
      _representative = new Node<T>;
      _representative->elem = elem;
      _representative->parent = NULL;
      _representative->rank = 0;
    }

    Node<T> *Find(){
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

    void Union(Set other){
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

  private:
    Node<T> *_representative;
};