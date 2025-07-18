#include "geometry.hpp"

int Vector::operator*(const Vector& other) const {
  return x_coord_ * other.x_coord_ + y_coord_ * other.y_coord_;
}

int Vector::operator^(const Vector& other) const {
  return x_coord_ * other.y_coord_ - y_coord_ * other.x_coord_;
}

Vector Vector::operator+(const Vector& other) const {
  return Vector(x_coord_ + other.x_coord_, y_coord_ + other.y_coord_);
}

Vector& Vector::operator+=(const Vector& other) {
  x_coord_ += other.x_coord_;
  y_coord_ += other.y_coord_;
  return *this;
}

Vector Vector::operator-(const Vector& other) const {
  return Vector(x_coord_ - other.x_coord_, y_coord_ - other.y_coord_);
}

Vector& Vector::operator-=(const Vector& other) {
  x_coord_ -= other.x_coord_;
  y_coord_ -= other.y_coord_;
  return *this;
}

Vector Vector::operator-() const { return Vector(-x_coord_, -y_coord_); }

Vector Vector::operator*(int scalar) const {
  return Vector(x_coord_ * scalar, y_coord_ * scalar);
}

Vector& Vector::operator*=(int scalar) {
  x_coord_ *= scalar;
  y_coord_ *= scalar;
  return *this;
}

Vector operator*(int scalar, const Vector& vector) {
  return Vector(vector.x_coord_ * scalar, vector.y_coord_ * scalar);
}

void Point::Move(const Vector& vector) {
  x_coord_ += vector.GetX();
  y_coord_ += vector.GetY();
}

bool Point::ContainsPoint(const Point& point) const {
  return x_coord_ == point.GetX() && y_coord_ == point.GetY();
}

bool Point::CrossSegment(const Segment& segment) const {
  int64_t dif_x = segment.GetB().GetX() - segment.GetA().GetX();
  int64_t dif_y = segment.GetB().GetY() - segment.GetA().GetY();

  int64_t point_x_diff = GetX() - segment.GetA().GetX();
  int64_t point_y_diff = GetY() - segment.GetA().GetY();

  int64_t orient_area = dif_x * point_y_diff - dif_y * point_x_diff;
  if (orient_area != 0) {
    return false;
  }

  int64_t min_x = std::min(segment.GetA().GetX(), segment.GetB().GetX());
  int64_t max_x = std::max(segment.GetA().GetX(), segment.GetB().GetX());
  int64_t min_y = std::min(segment.GetA().GetY(), segment.GetB().GetY());
  int64_t max_y = std::max(segment.GetA().GetY(), segment.GetB().GetY());

  return (min_x <= GetX() && GetX() <= max_x) &&
         (min_y <= GetY() && GetY() <= max_y);
}

Point* Point::Clone() const { return new Point(*this); }

void Segment::Move(const Vector& vector) {
  p1_.Move(vector);
  p2_.Move(vector);
}

bool Segment::ContainsPoint(const Point& point) const {
  int64_t orient_area =
      (GetB().GetX() - GetA().GetX()) * (point.GetY() - GetA().GetY()) -
      (GetB().GetY() - GetA().GetY()) * (point.GetX() - GetA().GetX());

  if (orient_area != 0) {
    return false;
  }

  int64_t min_x = std::min(GetA().GetX(), GetB().GetX());
  int64_t max_x = std::max(GetA().GetX(), GetB().GetX());
  int64_t min_y = std::min(GetA().GetY(), GetB().GetY());
  int64_t max_y = std::max(GetA().GetY(), GetB().GetY());

  return (min_x <= point.GetX() && point.GetX() <= max_x) &&
         (min_y <= point.GetY() && point.GetY() <= max_y);
}

Segment* Segment::Clone() const { return new Segment(*this); }

bool OnSegment(const Point& point1, const Point& point2, const Point& point3) {
  return (point2.GetX() <= std::max(point1.GetX(), point3.GetX()) &&
          point2.GetX() >= std::min(point1.GetX(), point3.GetX()) &&
          point2.GetY() <= std::max(point1.GetY(), point3.GetY()) &&
          point2.GetY() >= std::min(point1.GetY(), point3.GetY()));
}

int Orientation(const Point& point1, const Point& point2, const Point& point3) {
  int val = (point2.GetY() - point1.GetY()) * (point3.GetX() - point2.GetX()) -
            (point2.GetX() - point1.GetX()) * (point3.GetY() - point2.GetY());
  if (val == 0) {
    return 0;
  }
  return (val > 0) ? 1 : 2;
}

bool Segment::CrossSegment(const Segment& segment) const {
  const Point& first_seg_start = this->GetA();
  const Point& first_seg_end = this->GetB();
  const Point& second_seg_start = segment.GetA();
  const Point& second_seg_end = segment.GetB();

  int orientation_1 =
      Orientation(first_seg_start, first_seg_end, second_seg_start);
  int orientation_2 =
      Orientation(first_seg_start, first_seg_end, second_seg_end);
  int orientation_3 =
      Orientation(second_seg_start, second_seg_end, first_seg_start);
  int orientation_4 =
      Orientation(second_seg_start, second_seg_end, first_seg_end);

  if (orientation_1 != orientation_2 && orientation_3 != orientation_4) {
    return true;
  }

  if (orientation_1 == 0 &&
      OnSegment(first_seg_start, second_seg_start, first_seg_end)) {
    return true;
  }
  if (orientation_2 == 0 &&
      OnSegment(first_seg_start, second_seg_end, first_seg_end)) {
    return true;
  }
  if (orientation_3 == 0 &&
      OnSegment(second_seg_start, first_seg_start, second_seg_end)) {
    return true;
  }
  return (orientation_4 == 0 &&
          OnSegment(second_seg_start, first_seg_end, second_seg_end));
}

void Line::Move(const Vector& vector) {
  point1_.Move(vector);
  point2_.Move(vector);
}

bool Line::ContainsPoint(const Point& point) const {
  return (point2_.GetY() - point1_.GetY()) * (point.GetX() - point1_.GetX()) ==
         (point.GetY() - point1_.GetY()) * (point2_.GetX() - point1_.GetX());
}

bool Line::CrossSegment(const Segment& segment) const {
  double coefficient_a = GetA();
  double coefficient_b = GetB();
  double coefficient_c = GetC();

  Point segment_start_point = segment.GetA();
  Point segment_end_point = segment.GetB();

  double value_at_start_point = coefficient_a * segment_start_point.GetX() +
                                coefficient_b * segment_start_point.GetY() +
                                coefficient_c;
  double value_at_end_point = coefficient_a * segment_end_point.GetX() +
                              coefficient_b * segment_end_point.GetY() +
                              coefficient_c;

  return (value_at_start_point * value_at_end_point <= 0);
}

Line* Line::Clone() const { return new Line(*this); }

void Ray::Move(const Vector& vector) { origin_ = origin_ + vector; }

bool Ray::ContainsPoint(const Point& point) const {
  Vector vector =
      Vector(point.GetX() - origin_.GetX(), point.GetY() - origin_.GetY());

  int64_t cross_product =
      direction_.GetX() * vector.GetY() - direction_.GetY() * vector.GetX();

  if (cross_product != 0) {
    return false;
  }

  int64_t dot_product =
      direction_.GetX() * vector.GetX() + direction_.GetY() * vector.GetY();
  return dot_product >= 0;
}

bool Ray::CrossSegment(const Segment& segment) const {
  if (ContainsPoint(segment.GetA()) || ContainsPoint(segment.GetB())) {
    return true;
  }

  Vector segment_vector(segment.GetB().GetX() - segment.GetA().GetX(),
                        segment.GetB().GetY() - segment.GetA().GetY());

  int orientation_start_segment_a =
      Orientation(GetA(), segment.GetA(), segment.GetB());
  int orientation_ray_start_point_segment_a =
      Orientation(GetA(), GetA() + GetVector(), segment.GetA());

  int orientation_ray_start_segment_start =
      Orientation(GetA(), GetA() + GetVector(), segment.GetA());
  int orientation_ray_start_segment_end =
      Orientation(GetA(), GetA() + GetVector(), segment.GetB());
  int orientation_segment_start_ray_start =
      Orientation(segment.GetA(), segment.GetB(), GetA());
  int orientation_segment_end_ray_start =
      Orientation(segment.GetA(), segment.GetB(), GetA() + GetVector());

  return (orientation_ray_start_segment_start !=
              orientation_ray_start_segment_end &&
          orientation_segment_start_ray_start !=
              orientation_segment_end_ray_start);
}

Ray* Ray::Clone() const {
  Ray* new_ray = new Ray(*this);
  return new_ray;
}

void Circle::Move(const Vector& vector) { center_.Move(vector); }

bool Circle::ContainsPoint(const Point& point) const {
  int64_t delta_x = point.GetX() - center_.GetX();
  int64_t delta_y = point.GetY() - center_.GetY();
  return ((size_t)(delta_x * delta_x + delta_y * delta_y) <= radius_ * radius_);
}

bool Circle::CrossSegment(const Segment& segment) const {
  double start_x = segment.GetA().GetX();
  double start_y = segment.GetA().GetY();
  double end_x = segment.GetB().GetX();
  double end_y = segment.GetB().GetY();

  double center_x = center_.GetX();
  double center_y = center_.GetY();

  double delta_x = end_x - start_x;
  double delta_y = end_y - start_y;

  double coefficient_a = delta_x * delta_x + delta_y * delta_y;
  double coefficient_b =
      2 * (delta_x * (start_x - center_x) + delta_y * (start_y - center_y));
  double coefficient_c = (start_x - center_x) * (start_x - center_x) +
                         (start_y - center_y) * (start_y - center_y) -
                         radius_ * radius_;

  double discriminant =
      coefficient_b * coefficient_b - 4 * coefficient_a * coefficient_c;

  if (discriminant < 0) {
    return false;
  }

  discriminant = sqrt(discriminant);
  double parameter_t1 = (-coefficient_b + discriminant) / (2 * coefficient_a);
  double parameter_t2 = (-coefficient_b - discriminant) / (2 * coefficient_a);

  return (parameter_t1 >= 0 && parameter_t1 <= 1) ||
         (parameter_t2 >= 0 && parameter_t2 <= 1);
}

Circle* Circle::Clone() const { return new Circle(*this); }