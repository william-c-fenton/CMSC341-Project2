// test0.cpp is a simple test program

#include <iostream>
#include <vector>
#include <cassert>
#include "raq.h"

using namespace std;

int main() {
  /*vector<float> data = { -42, 50, -19, 6, 18, 18, 44, 4, 16, 28, 0, -30, -46,
  50, -40, -31, -7, 48, 1, 16, -15, 34, -32, -41, -41, -12, 43, 35, 43, 14, 5, 18, -68, 3, -99, 25,
  83, 44, 31, -22, 4, 1, -45, -33, 0, 19 };*/
  vector<float> data = { 34, 16, 58, -24, 53,
      7, 97, 92, -12, 45,
      9, 0, -1, 20, 77 };
  RAQ raq(data);
  BlockRAQ braq(data);

  for (unsigned int j = 1; j < data.size(); j++) {
    for (unsigned int i = 0; i < j; i++) {

      float y = raq.query(i, j);
      float z = braq.query(i, j);

      if (y != z) {
        cout << "(" << i << ", " << j << "): "
          << y << " " << z << endl;
      }
    }
  }

  return 0;
}