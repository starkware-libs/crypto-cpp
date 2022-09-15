add_custom_command(
    OUTPUT libcrypto${CMAKE_SHARED_LIBRARY_SUFFIX}
    COMMAND
        ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/../libcrypto_c_exports${CMAKE_SHARED_LIBRARY_SUFFIX}
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS crypto_c_exports
)

add_custom_target(js_test ALL
    DEPENDS libcrypto${CMAKE_SHARED_LIBRARY_SUFFIX}
)

add_test(
    NAME js_test
    COMMAND npm test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
