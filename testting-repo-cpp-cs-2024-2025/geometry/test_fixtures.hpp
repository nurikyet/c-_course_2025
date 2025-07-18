/**
 * @file test_fixtures.hpp
 * @author yaishenka
 * @date 26.11.2022
 */
#include "geometry.hpp"
#include <gtest/gtest.h>
#pragma once

class GeometryTest : public ::testing::Test {
 public:
  ~GeometryTest() override {
    delete shape_;
    delete moved_shape_;
    delete p1_;
    delete p2_;
  }
 protected:
  void TestContainsPoint(bool expected) {
    ASSERT_EQ(shape_->ContainsPoint(*p1_), expected);
  }

  void TestCrossesSegment(bool expected) {
    auto segment = Segment(*p1_, *p2_);
    ASSERT_EQ(shape_->CrossSegment(segment), expected);
  }

  void SetupMoveTest() {
    auto vector = *p2_ - *p1_;
    moved_shape_ = shape_->Clone();
    moved_shape_->Move(vector);
  }

  virtual void TestMove() = 0;

  static bool ArePointsEqual(const Point& p1, const Point& p2) {
    return p1.GetX() == p2.GetX() && p1.GetY() == p2.GetY();
  }

  static void AreVectorsSimilar(const Vector& v1, const Vector& v2) {
    if (v1.GetX() == 0 || v2.GetX() == 0) {
      ASSERT_DOUBLE_EQ(v1.GetX(), v2.GetX());
      return;
    }

    if (v1.GetY() == 0 || v2.GetY() == 0) {
      ASSERT_DOUBLE_EQ(v1.GetY(), v2.GetY());
      return;
    }

    ASSERT_DOUBLE_EQ(static_cast<double>(v1.GetX()) / static_cast<double>(v2.GetX()),
                     static_cast<double>(v1.GetY()) / static_cast<double>(v2.GetY()));

  }

  IShape* shape_ = nullptr;
  IShape* moved_shape_ = nullptr;
  Point* p1_ = nullptr;
  Point* p2_ = nullptr;
};

class PointTest : public GeometryTest {
 public:
  ~PointTest() override {
    delete expected_moved_point_;
  }
 protected:
  void ConstructTest(
      int64_t point_x,
      int64_t point_y,
      int64_t p1_x,
      int64_t p1_y,
      int64_t p2_x,
      int64_t p2_y,
      int64_t expected_x,
      int64_t expected_y) {
    shape_ = new Point(point_x, point_y);
    p1_ = new Point(p1_x, p1_y);
    p2_ = new Point(p2_x, p2_y);
    expected_moved_point_ = new Point(expected_x, expected_y);
  }

  void TestMove() override {
    SetupMoveTest();
    auto moved_point = dynamic_cast<Point*>(moved_shape_);
    ASSERT_TRUE(ArePointsEqual(*moved_point, *expected_moved_point_));
  }

  Point* expected_moved_point_;
};

class SegmentTest : public GeometryTest {
 public:
  ~SegmentTest() override {
    delete expected_moved_segment_;
  }
 protected:
  void ConstructTest(
      int64_t segment_p1_x,
      int64_t segment_p1_y,
      int64_t segment_p2_x,
      int64_t segment_p2_y,
      int64_t p1_x,
      int64_t p1_y,
      int64_t p2_x,
      int64_t p2_y,
      int64_t segment_expected_p1_x,
      int64_t segment_expected_p1_y,
      int64_t segment_expected_p2_x,
      int64_t segment_expected_p2_y) {
    shape_ = new Segment(Point(segment_p1_x, segment_p1_y),
                         Point(segment_p2_x, segment_p2_y));
    p1_ = new Point(p1_x, p1_y);
    p2_ = new Point(p2_x, p2_y);
    expected_moved_segment_ = new Segment(
        Point(segment_expected_p1_x, segment_expected_p1_y),
        Point(segment_expected_p2_x, segment_expected_p2_y)
    );
  }

  static bool AreSegmentsEqual(const Segment& s1, const Segment& s2) {
    return ArePointsEqual(s1.GetA(), s2.GetA())
        && ArePointsEqual(s1.GetB(), s2.GetB());
  }

  void TestMove() override {
    SetupMoveTest();
    auto moved_segment = dynamic_cast<Segment*>(moved_shape_);
    ASSERT_TRUE(AreSegmentsEqual(*moved_segment, *expected_moved_segment_));
  }

  Segment* expected_moved_segment_;
};

class RayTest: public GeometryTest {
 public:
  ~RayTest() override {
    delete expected_moved_ray_point_;
    delete expected_moved_ray_vector_;
  }
 protected:
  void ConstructTest(
      int64_t ray_p1_x,
      int64_t ray_p1_y,
      int64_t ray_p2_x,
      int64_t ray_p2_y,
      int64_t p1_x,
      int64_t p1_y,
      int64_t p2_x,
      int64_t p2_y,
      int64_t ray_expected_p_x,
      int64_t ray_expected_p_y,
      int64_t ray_expected_v_x,
      int64_t ray_expected_v_y) {
    shape_ = new Ray(Point(ray_p1_x, ray_p1_y),
                     Point(ray_p2_x, ray_p2_y));
    p1_ = new Point(p1_x, p1_y);
    p2_ = new Point(p2_x, p2_y);
    expected_moved_ray_point_ = new Point(ray_expected_p_x, ray_expected_p_y);
    expected_moved_ray_vector_ = new Vector(ray_expected_v_x, ray_expected_v_y);
  }


  void TestMove() override {
    SetupMoveTest();
    auto moved_ray = dynamic_cast<Ray*>(moved_shape_);
    ASSERT_TRUE(ArePointsEqual(*expected_moved_ray_point_, moved_ray->GetA()));
    AreVectorsSimilar(*expected_moved_ray_vector_, moved_ray->GetVector());
  }

  Point* expected_moved_ray_point_;
  Vector* expected_moved_ray_vector_;
};

class LineTest: public GeometryTest {
 protected:
  void ConstructTest(
      int64_t line_p1_x,
      int64_t line_p1_y,
      int64_t line_p2_x,
      int64_t line_p2_y,
      int64_t p1_x,
      int64_t p1_y,
      int64_t p2_x,
      int64_t p2_y,
      int64_t line_expected_a,
      int64_t line_expected_b,
      int64_t line_expected_c) {
    shape_ = new Line(Point(line_p1_x, line_p1_y),
                     Point(line_p2_x, line_p2_y));
    p1_ = new Point(p1_x, p1_y);
    p2_ = new Point(p2_x, p2_y);
    line_expected_a_ = line_expected_a;
    line_expected_b_ = line_expected_b;
    line_expected_c_ = line_expected_c;
  }


  void TestMove() override {
    SetupMoveTest();
    auto moved_line = dynamic_cast<Line*>(moved_shape_);
    auto moved_line_a = moved_line->GetA();
    auto moved_line_b = moved_line->GetB();
    auto moved_line_c = moved_line->GetC();

    double a_k = 0;
    double b_k = 0;
    double c_k = 0;

    if (line_expected_a_ == 0 || moved_line_a == 0) {
      a_k = 0;
    } else {
      a_k = static_cast<double>(moved_line_a) / static_cast<double>(line_expected_a_);
    }

    if (line_expected_b_ == 0 || moved_line_b == 0) {
      b_k = 0;
    } else {
      b_k = static_cast<double>(moved_line_b) / static_cast<double>(line_expected_b_);
    }

    if (line_expected_c_ == 0 || moved_line_c == 0) {
      c_k = 0;
    } else {
      c_k = static_cast<double>(moved_line_c) / static_cast<double>(line_expected_c_);
    }

    if (a_k != 0 && b_k != 0) {
      ASSERT_DOUBLE_EQ(a_k, b_k);
    }

    if (a_k != 0 && c_k != 0) {
      ASSERT_DOUBLE_EQ(a_k, c_k);
    }

    if (b_k != 0 && c_k != 0) {
      ASSERT_DOUBLE_EQ(b_k, c_k);
    }
  }

  int64_t line_expected_a_;
  int64_t line_expected_b_;
  int64_t line_expected_c_;
};

class CircleTest : public GeometryTest {
 public:
  ~CircleTest() override {
    delete expected_moved_circle_;
  }
 protected:
  void ConstructTest(
      int64_t point_x,
      int64_t point_y,
      size_t r,
      int64_t p1_x,
      int64_t p1_y,
      int64_t p2_x,
      int64_t p2_y,
      int64_t expected_p_x,
      int64_t expected_p_y,
      size_t expected_r) {
    shape_ = new Circle(Point(point_x, point_y), r);
    p1_ = new Point(p1_x, p1_y);
    p2_ = new Point(p2_x, p2_y);
    expected_moved_circle_ = new Circle(Point(expected_p_x, expected_p_y), expected_r);
  }

  void TestMove() override {
    SetupMoveTest();
    auto moved_circle = dynamic_cast<Circle*>(moved_shape_);
    ASSERT_TRUE(ArePointsEqual(moved_circle->GetCentre(), expected_moved_circle_->GetCentre()));
    ASSERT_DOUBLE_EQ(moved_circle->GetRadius(), expected_moved_circle_->GetRadius());
  }

  Circle* expected_moved_circle_;
};
