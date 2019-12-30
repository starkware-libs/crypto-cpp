#ifndef STARKWARE_UTILS_TEST_UTILS_H_
#define STARKWARE_UTILS_TEST_UTILS_H_

#include <string>

#include "gmock/gmock.h"

#include "starkware/utils/error_handling.h"

namespace starkware {

// Below implementation is an altered version of EXPECT_THROW from GTest.
#define EXPECT_ASSERT(statement, expected_matcher)                        \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_                                           \
  if (::testing::internal::ConstCharPtr gtest_msg = "") {                 \
    bool gtest_caught_expected = false;                                   \
    std::string message__;                                                \
    try {                                                                 \
      GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement);          \
    } catch (const StarkwareException& e) {                               \
      gtest_caught_expected = true;                                       \
      message__ = e.what();                                               \
    } catch (...) {                                                       \
      gtest_msg.value = "Expected: " #statement                           \
                        " throws an exception of type StarkwareException" \
                        ".\n  Actual: it throws a different type.";       \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__);         \
    }                                                                     \
    if (gtest_caught_expected) {                                          \
      EXPECT_THAT(message__, expected_matcher);                           \
    } else {                                                              \
      gtest_msg.value = "Expected: " #statement                           \
                        " throws an exception of type StarkwareException" \
                        ".\n  Actual: it throws nothing.";                \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__);         \
    }                                                                     \
  } else {                                                                \
    GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__)                 \
        : GTEST_NONFATAL_FAILURE_(gtest_msg.value);                       \
  }

}  // namespace starkware

#endif  // STARKWARE_UTILS_TEST_UTILS_H_
