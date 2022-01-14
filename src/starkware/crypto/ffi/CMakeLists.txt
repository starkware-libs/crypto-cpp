add_subdirectory(js)

add_library(crypto_c_exports SHARED pedersen_hash.cc ecdsa.cc utils.cc)
target_link_libraries(crypto_c_exports crypto)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crypto_lib/crypto_lib.go
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/crypto_lib/ ${CMAKE_CURRENT_BINARY_DIR}/crypto_lib/
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crypto_lib/crypto_lib.go
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/crypto_lib_test.go
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/crypto_lib_test.go ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/crypto_lib_test.go
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ecdsa.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ecdsa.h ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ecdsa.h
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pedersen_hash.h
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/pedersen_hash.h ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pedersen_hash.h
)

add_custom_target(
  CopyGoFiles ALL
  DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/crypto_lib/crypto_lib.go
    ${CMAKE_CURRENT_BINARY_DIR}/crypto_lib_test.go
    ${CMAKE_CURRENT_BINARY_DIR}/ecdsa.h
    ${CMAKE_CURRENT_BINARY_DIR}/pedersen_hash.h
)
