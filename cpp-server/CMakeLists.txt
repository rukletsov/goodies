
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.4)

PROJECT (cpp-server)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
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

# Locate and enable Boost.
SET (Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE (Boost COMPONENTS system filesystem REQUIRED)
INCLUDE_DIRECTORIES (${Boost_INCLUDE_DIRS})

SET (SandboxProj_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/socket.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/threadpool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp
)

SET (SandboxProj_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/socket.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/threadpool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.hpp
)

# Enable C++11 for Clang on MacOS X.
IF (APPLE AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
ENDIF (APPLE AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))

INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE (SandboxProj
                ${SandboxProj_SRCS}
                ${SandboxProj_HDRS}
)

TARGET_LINK_LIBRARIES (SandboxProj
                       ${Boost_LIBRARIES}
)

SET_TARGET_PROPERTIES (SandboxProj PROPERTIES DEBUG_POSTFIX "d")

# Supress MSVC-specific compiler and linker warnings.
IF (MSVC)
    # Supress secure warnings (C4996) for MSVC compiler.
    SET_TARGET_PROPERTIES (SandboxProj PROPERTIES COMPILE_FLAGS
                           "/D _CRT_SECURE_NO_DEPRECATE /D _SCL_SECURE_NO_WARNINGS")
ENDIF (MSVC)
