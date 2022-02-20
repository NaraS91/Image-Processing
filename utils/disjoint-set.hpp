#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP
#include <vector>
#include <iostream>

namespace utils {
  class Set{
    public:
      Set(int size) {
        _size = size;
        _elems = new int[size];
        _ranks = new int[size];
        _subSizes = new int[size];
        initSet();
      }

      ~Set(){
        delete []_elems;
        _elems = nullptr;
        delete []_ranks;
        _ranks = nullptr;
        delete []_subSizes;
        _subSizes = nullptr;
      }

      int Find(int elem) {
        int curr = elem;

        while (_elems[curr] != curr) {
          curr = _elems[curr];
        }

        int representative = curr;
        curr = elem;
        while (curr != representative) {
          int next = _elems[curr];
          _elems[curr] = representative;
          curr = next;
        }

        return representative;
      }

      void Union(int x, int y) {
        int rep_x = Find(x);
        int rep_y = Find(y);

        if (rep_x == rep_y)
            return;

        if (_ranks[rep_x] > _ranks[rep_y]) {
          rep_x = rep_x + rep_y;
          rep_y = rep_x - rep_y;
          rep_x = rep_x - rep_y;
        }


        _elems[rep_x] = rep_y;
        _subSizes[rep_y] += _subSizes[rep_x];

        if (_ranks[rep_x] == _ranks[rep_y])
          _ranks[rep_y]++;
      }

      unsigned Size(int i) {
        return _subSizes[Find(i)];
      }

    private:
      void initSet() {
        for (int i = 0; i < _size; i++) {
          _elems[i] = i;
          _subSizes[i] = 1;
        }
      }

      int* _elems;
      int* _ranks;
      int* _subSizes;
      int _size;
  };
}
#endif
