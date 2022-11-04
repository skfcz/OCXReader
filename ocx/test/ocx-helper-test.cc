#include "ocx/internal/ocx-helper.h"

#include <gtest/gtest.h>

TEST(OCXHelperTest, ParseKnotVector) {
  // Provide test cases in following format:
  // {
  //    {
  //      knotVectorToParse: "0 1 2 2 3",
  //      number of knots in knotVectorToParse: 5,
  //      number of unique knots in knotVectorToParse: 4,
  //      multiplicity vector {1, 1, 2, 1}
  //    },
  // }
  std::vector<std::tuple<std::string, int, int, std::vector<int>>> testCases = {
      {"0 0 7.625 13.625 21.25 27.25 27.25", 7, 5, {2, 1, 1, 1, 2}},
      {"0 0 1 1 ", 4, 2, {2, 2}}};

  for (auto [knotVector, numKnots, numKnotsUnique, expectedMults] : testCases) {
    KnotMults knots = OCXHelper::ParseKnotVector(knotVector, numKnots);

    EXPECT_EQ(knots.IsNull, false)
        << "Expected knots to be parsed successfully, but got knots.IsNull = "
        << knots.IsNull;

    EXPECT_EQ(numKnotsUnique, knots.knots.Length())
        << "Expected knot length to be " << numKnotsUnique << " , but got "
        << knots.knots.Length();

    for (int i = 0; i < knots.knots.Length(); i++) {
      EXPECT_EQ(expectedMults[i], knots.mults[i + 1])
          << "Expected multiplicity at index " << i << " to be "
          << expectedMults[i] << ", but got " << knots.mults[i + 1];
    }
  }
}

TEST(OCXHelperTest, ParseKnotVectorIsNull) {
  std::string knotVector = "0 1";

  KnotMults knots = OCXHelper::ParseKnotVector(knotVector, 1);

  EXPECT_EQ(knots.IsNull, true)
      << "Expected knots to NOT be parsed successfully, but got knots.IsNull = "
      << knots.IsNull;
}

TEST(OCXHelperTest, ParseControlPoints) {}
