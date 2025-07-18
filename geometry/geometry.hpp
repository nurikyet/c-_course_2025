#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

class Point;
class Segment;

class Vector {
 public:
  Vector() : x_coord_(0), y_coord_(0) {}

  Vector(int first, int second) : x_coord_(first), y_coord_(second) {}

  int GetX() const { return x_coord_; }

  int GetY() const { return y_coord_; }

  int operator*(const Vector& other_vector) const;
  int operator^(const Vector& other_vector) const;
  Vector operator+(const Vector& other_vector) const;
  Vector& operator+=(const Vector& other_vector);
  Vector operator-(const Vector& other_vector) const;
  Vector& operator-=(const Vector& other_vector);
  Vector operator-() const;
  Vector operator*(int scalar) const;
  Vector& operator*=(int scalar);
  friend Vector operator*(int scalar, const Vector& vec);

 private:
  int x_coord_, y_coord_;
};

class IShape {
 public:
  virtual ~IShape() {}

  virtual void Move(const Vector& offset) = 0;
  virtual bool ContainsPoint(const Point& point) const = 0;
  virtual bool CrossSegment(const Segment& segment) const = 0;
  virtual IShape* Clone() const = 0;
};

class Point : public IShape {
 public:
  Point(int64_t first, int64_t second) : x_coord_(first), y_coord_(second) {}

  int64_t GetX() const { return x_coord_; }
  int64_t GetY() const { return y_coord_; }

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  Point* Clone() const override;

  Vector operator-(const Point& other_point) const {
    return Vector(GetX() - other_point.x_coord_, GetY() - other_point.y_coord_);
  }

  Point operator+(const Vector& vector) const {
    return Point(GetX() + vector.GetX(), GetY() + vector.GetY());
  }

  Point& operator+=(const Vector& vector) {
    x_coord_ += vector.GetX();
    y_coord_ += vector.GetY();
    return *this;
  }

 private:
  int64_t x_coord_;
  int64_t y_coord_;
};

class Segment : public IShape {
 public:
  Segment(const Point& point1, const Point& point2)
      : p1_(point1), p2_(point2) {}

  const Point& GetA() const { return p1_; }
  const Point& GetB() const { return p2_; }

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  Segment* Clone() const override;

 private:
  Point p1_;
  Point p2_;
};

class Line : public IShape {
 public:
  Line(const Point& point1, const Point& point2)
      : point1_(point1), point2_(point2) {}

  double GetA() const { return point2_.GetY() - point1_.GetY(); }

  double GetB() const { return point1_.GetX() - point2_.GetX(); }

  double GetC() const {
    return point2_.GetX() * point1_.GetY() - point1_.GetX() * point2_.GetY();
  }

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  Line* Clone() const override;

 private:
  Point point1_;
  Point point2_;
};

class Ray : public IShape {
 public:
  Ray(const Point& origin, const Point& second_point)
      : origin_(origin), direction_(second_point - origin) {}

  const Point& GetA() const { return origin_; }
  const Vector& GetVector() const { return direction_; }

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  Ray* Clone() const override;

 private:
  Point origin_;
  Vector direction_;
};

class Circle : public IShape {
 public:
  Circle(const Point& center, size_t radius)
      : center_(center), radius_(radius) {}

  const Point& GetCentre() const { return center_; }
  size_t GetRadius() const { return radius_; }

  void Move(const Vector& vector) override;
  bool ContainsPoint(const Point& point) const override;
  bool CrossSegment(const Segment& segment) const override;
  Circle* Clone() const override;

 private:
  Point center_;
  size_t radius_;
};