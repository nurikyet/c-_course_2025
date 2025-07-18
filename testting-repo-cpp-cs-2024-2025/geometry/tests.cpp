/** @author yaishenka
    @date 26.11.2022 */
#include <gtest/gtest.h>
#include "test_fixtures.hpp"

TEST_F(PointTest, PointTest1) {
  ConstructTest(0,0,-1,-1,1,1,2,2);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest1) {
  ConstructTest(0,-1,0,1,0,0,0,1,0,0,0,2);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest1) {
  ConstructTest(0,0,1,1,2,2,3,2,1,0,1,1);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(LineTest, LineTest1) {
  ConstructTest(0,0,0,1,1,1,2,1,1,0,-1);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(CircleTest, CircleTest1) {
  ConstructTest(0,0,5,0,0,5,5,5,5,5);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(PointTest, PointTest2) {
  ConstructTest(1,6,3,7,5,8,3,7);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(PointTest, PointTest3) {
  ConstructTest(4,2,4,2,4,5,4,5);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(PointTest, PointTest4) {
  ConstructTest(4,3,4,2,4,5,4,6);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(PointTest, PointTest5) {
  ConstructTest(4,6,4,2,4,5,4,9);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(PointTest, PointTest6) {
  ConstructTest(-7,-2,-5,-2,-9,-2,-11,-2);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(PointTest, PointTest7) {
  ConstructTest(-11,-2,-5,-2,-9,-2,-15,-2);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(PointTest, PointTest8) {
  ConstructTest(19,5,1,1,9001,2001,9019,2005);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(PointTest, PointTest9) {
  ConstructTest(-19,-5,-1,-1,-9002,-2001,-9020,-2005);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(PointTest, PointTest10) {
  ConstructTest(4,0,4,2,4,5,4,3);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest2) {
  ConstructTest(3,7,5,8,1,6,3,8,5,9,7,10);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest3) {
  ConstructTest(4,2,4,5,4,2,4,5,4,5,4,8);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest4) {
  ConstructTest(4,2,4,5,4,3,4,5,4,4,4,7);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest5) {
  ConstructTest(4,2,4,5,4,6,4,5,4,1,4,4);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest6) {
  ConstructTest(-5,-2,-9,-2,-7,-2,-5,-2,-3,-2,-7,-2);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest7) {
  ConstructTest(-5,-2,-9,-2,-11,-2,-5,-2,1,-2,-3,-2);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest8) {
  ConstructTest(1,1,9001,2001,19,5,1,2001,-17,1997,8983,3997);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest9) {
  ConstructTest(-1,-1,-9002,-2001,-19,-5,-1,-2001,17,-1997,-8984,-3997);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest10) {
  ConstructTest(4,2,4,5,4,0,4,5,4,7,4,10);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest11) {
  ConstructTest(5,1,2,6,1,1,7,8,11,8,8,13);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest12) {
  ConstructTest(3,3,5,6,5,6,3,3,1,0,3,3);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest13) {
  ConstructTest(1,1,7,4,5,3,3,2,-1,0,5,3);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest14) {
  ConstructTest(1,1,7,4,3,2,5,3,3,2,9,5);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest15) {
  ConstructTest(1,1,7,4,5,3,7,4,3,2,9,5);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest16) {
  ConstructTest(1,1,7,4,3,2,3,2,1,1,7,4);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest17) {
  ConstructTest(3,2,3,2,5,3,1,1,-1,0,-1,0);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest18) {
  ConstructTest(0,0,2,4,3,6,2,4,-1,-2,1,2);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest19) {
  ConstructTest(-1,-2,0,0,2,4,0,0,-3,-6,-2,-4);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest20) {
  ConstructTest(0,0,2,4,3,6,4,8,1,2,3,6);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest21) {
  ConstructTest(0,0,4,4,2,2,5,2,3,0,7,4);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest22) {
  ConstructTest(5,2,2,2,4,4,0,0,1,-2,-2,-2);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest23) {
  ConstructTest(3,-3,0,0,0,0,4,4,7,1,4,4);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest24) {
  ConstructTest(0,0,4,4,4,7,4,4,0,-3,4,1);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest25) {
  ConstructTest(0,0,4,4,6,6,6,3,0,-3,4,1);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest26) {
  ConstructTest(2,1,2,4,2,5,2,8,2,4,2,7);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest27) {
  ConstructTest(5,2,0,0,1,4,3,-4,7,-6,2,-8);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest28) {
  ConstructTest(-9725,-8992,9812,9925,-9999,7011,8122,-9248,8396,-25251,27933,-6334);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest29) {
  ConstructTest(-10000,-10000,-9999,10000,10000,-10000,9999,10000,-10001,10000,-10000,30000);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest30) {
  ConstructTest(-9999,-1,10000,1,0,0,0,1,-9999,0,10000,2);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest31) {
  ConstructTest(1,1,3,3,3,1,4,4,2,4,4,6);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(SegmentTest, SegmentTest32) {
  ConstructTest(1,2,1,2,1,2,1,2,1,2,1,2);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest2) {
  ConstructTest(3,7,5,8,1,6,3,8,5,9,2,1);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(RayTest, RayTest3) {
  ConstructTest(4,2,4,5,4,2,4,5,4,5,0,3);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest4) {
  ConstructTest(4,2,4,5,4,3,4,5,4,4,0,3);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest5) {
  ConstructTest(-5,-2,-9,-2,-5,-2,-5,-2,-5,-2,-4,0);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest6) {
  ConstructTest(-5,-2,-9,-2,-7,-2,-5,-2,-3,-2,-4,0);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest7) {
  ConstructTest(-5,-2,-9,-2,-11,-2,-5,-2,1,-2,-4,0);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest8) {
  ConstructTest(4,2,4,5,0,0,4,5,8,7,0,3);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest9) {
  ConstructTest(1,1,9001,2001,19,5,1,2001,-17,1997,9000,2000);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(RayTest, RayTest10) {
  ConstructTest(4,2,4,5,4,0,4,5,4,7,0,3);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(LineTest, LineTest2) {
  ConstructTest(3,7,5,8,1,6,3,8,1,-2,13);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(LineTest, LineTest3) {
  ConstructTest(4,2,4,5,4,2,4,5,1,0,-4);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(LineTest, LineTest4) {
  ConstructTest(4,2,4,5,4,3,4,5,1,0,-4);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(LineTest, LineTest5) {
  ConstructTest(4,2,4,5,0,0,4,5,1,0,-8);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(LineTest, LineTest6) {
  ConstructTest(9999,9999,10000,10000,-10000,-10000,9999,10000,1,-1,1);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest2) {
  ConstructTest(0,0,5,9,8,-5,3,-14,-5,5);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest3) {
  ConstructTest(0,0,5,2,5,5,2,3,-3,5);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest4) {
  ConstructTest(0,0,5,-1,5,1,5,2,0,5);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest5) {
  ConstructTest(0,0,5,0,5,1,5,1,0,5);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest6) {
  ConstructTest(0,0,1,-1,-1,1,1,2,2,1);
  TestContainsPoint(false);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest7) {
  ConstructTest(0,0,5,0,5,5,0,5,-5,5);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest8) {
  ConstructTest(0,0,5,0,5,4,1,4,-4,5);
  TestContainsPoint(true);
  TestCrossesSegment(true);
  TestMove();
}

TEST_F(CircleTest, CircleTest9) {
  ConstructTest(0,0,5,0,4,4,0,4,-4,5);
  TestContainsPoint(true);
  TestCrossesSegment(false);
  TestMove();
}

TEST_F(CircleTest, CircleTest10) {
  ConstructTest(10000,-10000,10000,-10000,0,-10000,-10000,10000,-20000,10000);
  TestContainsPoint(false);
  TestCrossesSegment(false);
  TestMove();
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}