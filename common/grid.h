#ifndef COMMON_GRID_H_
#define COMMON_GRID_H_

#include "common/declarations.h"

struct Point {
 int x;
 int y;

 Point() {}
 Point(int x, int y) : x(x), y(y) {}
 Point operator+ (const Point& rhs) const { return Point(x + rhs.x, y + rhs.y); }
 Point operator- (const Point& rhs) const { return Point(x - rhs.x, y - rhs.y); }
 bool operator< (const Point& rhs) const {
   return std::tie(x, y) < std::tie(rhs.x, rhs.y);
 }
 bool operator== (const Point& rhs) const {
   return std::tie(x, y) == std::tie(rhs.x, rhs.y);
 }

 // Rotates point around origin 90 degrees left
 void RotateLeft() { y = -y; std::swap(x, y); }

 // Rotates point around origin 90 degrees right
 void RotateRight() { x = -x; std::swap(x, y); }
};

template <typename T>
class Grid {
 public:
  Grid(int x, int y, T val) : x_(x), y_(y), grid_(y, vector<T>(x, val)) {}
  Grid(int x, int y) : x_(x), y_(y), grid_(y, vector<T>(x)) {}
  T& operator[] (const Point& p) { return grid_[p.y][p.x]; }
  const T& at(const Point& p) const { return grid_[p.y][p.x]; }
  bool Inside(const Point& p) const { return p.x >= 0 && p.x < x_ && p.y >= 0 && p.y < y_; }
 private:
  int x_, y_;
  vector<vector<T>> grid_;
};

#endif  // COMMON_MATH2D_H_
