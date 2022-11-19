#include "ocx/internal/ocx-helper.h"

#include <gtest/gtest.h>

#include <BSplCLib.hxx>

TEST(OCXHelperTest, ParseKnotVector) {
  struct TestCase {
    std::string testId;
    std::string knotVectorToParse;
    int numberOfKnots;
    int numberOfUniqueKnots;
    int degree;
    std::vector<int> multiplicityVector;
  };
  std::vector<TestCase> testCases = {
      {"test1", "0 0 7.625 13.625 21.25 27.25 27.25", 7, 5, 3, {2, 1, 1, 1, 2}},
      {"test2", "0 0 1 1 ", 4, 2, 3, {2, 2}},
      {"test3", "-1 -0.999999999 -0.5 0 0 ", 5, 3, 3, {2, 1, 2}}};

  for (auto [testId, knotVector, numKnots, numKnotsUnique, degree,
             expectedMults] : testCases) {
    std::cout << "Start test case: " << testId << std::endl;
    ocx::KnotMults knots =
        ocx::OCXHelper::ParseKnotVector(knotVector, numKnots, nullptr);

    // Check if knot vector got parsed successfully
    EXPECT_EQ(knots.IsNull, false)
        << "Expected knots to be parsed successfully, but got knots.IsNull = "
        << knots.IsNull;

    // Check if number of unique knots is correct
    EXPECT_EQ(numKnotsUnique, knots.knots.Length())
        << "Expected knot length to be " << numKnotsUnique << " , but got "
        << knots.knots.Length();

    // Poles.Length() = numKnots - Degree - 1 == Sum(Mults(i)) - Degree - 1 >= 2
    int multsSum = BSplCLib::NbPoles(degree, false, knots.mults);
    EXPECT_EQ(numKnots - degree - 1, multsSum)
        << "Expected mults length to be " << numKnots - degree - 1
        << " , but got " << multsSum;

    // Check if multiplicity vector is correct
    for (int i = 0; i < knots.knots.Length(); i++) {
      EXPECT_EQ(expectedMults[i], knots.mults[i + 1])
          << "Expected multiplicity at index " << i << " to be "
          << expectedMults[i] << ", but got " << knots.mults[i + 1];
    }
  }
}

TEST(OCXHelperTest, ParseKnotVectorIsNull) {
  std::string knotVector = "0 1";

  ocx::KnotMults knots =
      ocx::OCXHelper::ParseKnotVector(knotVector, 1, nullptr);

  EXPECT_EQ(knots.IsNull, true)
      << "Expected knots to NOT be parsed successfully, but got knots.IsNull = "
      << knots.IsNull;
}

TEST(OCXHelperTest, ParseControlPoints) {}
