#include "ocx/internal/ocx-helper.h"

#include <gtest/gtest.h>

TEST(OCXHelperTest, ParseKnotVector) {
  std::string knotVectorS = "0 0 7.625 13.625 21.25 27.25 27.25";

  KnotMults knots = OCXHelper::ParseKnotVector(knotVectorS, 7);

  EXPECT_EQ(knots.IsNull, false)
      << "Expected knots to be parsed successfully, but got knots.IsNull = "
      << knots.IsNull;

  EXPECT_EQ(5, knots.knots.Length())
      << "Expected knot length to be 5, but got " << knots.knots.Length();

  std::vector<int> expectedMults = {2, 1, 1, 1, 2};
  for (int i = 0; i < knots.knots.Length(); i++) {
    EXPECT_EQ(expectedMults[i], knots.mults[i + 1])
        << "Expected multiplicity at index " << i << " to be "
        << expectedMults[i] << ", but got " << knots.mults[i + 1];
  }
}

TEST(OCXHelperTest, ParseKnotVectorIsNull) {
  std::string knotVectorS = "0 0 7.625 13.625 21.25 27.25 27.25";

  KnotMults knots = OCXHelper::ParseKnotVector(knotVectorS, 6);

  EXPECT_EQ(knots.IsNull, true)
      << "Expected knots to NOT be parsed successfully, but got knots.IsNull = "
      << knots.IsNull;
}

TEST(OCXHelperTest, ParseControlPoints) {}
