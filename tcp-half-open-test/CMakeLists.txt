
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

PROJECT (tcpconnect)

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Check that one of the officially supported compilers is used.
IF (MSVC)
    MESSAGE (STATUS "Supported compiler used: MSVC")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    MESSAGE (STATUS "Supported compiler used: GCC")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    MESSAGE (STATUS "Supported compiler used: Clang")
ELSE ()
    MESSAGE ("Unsupported compiler used: ${CMAKE_CXX_COMPILER_ID}")
ENDIF (MSVC)

SET (tcpconnect_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp_connect.cpp
)

ADD_EXECUTABLE (tcpconnect
                ${tcpconnect_SRCS}
)

# Add debug postfix for clarity.
SET_TARGET_PROPERTIES (tcpconnect PROPERTIES DEBUG_POSTFIX "d")
