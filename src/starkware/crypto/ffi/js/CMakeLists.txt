add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/libcrypto
    COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/../libcrypto_c_exports.*
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS crypto_c_exports
)

add_custom_target(js_test ALL
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libcrypto
)

add_test(
    NAME js_test
    COMMAND npm test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
