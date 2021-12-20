add_library(starkex order.cc)
target_link_libraries(starkex crypto)

add_executable(order_test order_test.cc)
target_link_libraries(order_test starkex gtest gtest_main gmock pthread)
add_test(order_test order_test)
