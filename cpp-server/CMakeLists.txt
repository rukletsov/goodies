
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.11)

PROJECT (CppServer)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

SET (CMAKE_CXX_STANDARD 17)

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

# Download and unpack googletest at configure time.
CONFIGURE_FILE (CMakeLists.txt.in googletest-download/CMakeLists.txt)
EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
IF (result)
  MESSAGE (FATAL_ERROR "CMake step for googletest failed: ${result}")
ENDIF()
EXECUTE_PROCESS (COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
IF (result)
  MESSAGE (FATAL_ERROR "Build step for googletest failed: ${result}")
ENDIF ()

# Prevent overriding the parent project's compiler/linker
# settings on Windows.
SET (gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
ADD_SUBDIRECTORY (${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

SET (CppServer_Lib_SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/src/socket.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/threadpool.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp
)


SET (CppServer_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

SET (CppServer_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/latch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/socket.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/threadpool.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/worker.hpp
)

SET (CppServer_Test_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/worker_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/threadpool_test.cpp
)

# Enable C++17 for Clang on MacOS X.
IF (APPLE AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
ENDIF (APPLE AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))

INCLUDE_DIRECTORIES (${CMAKE_CURRENT_SOURCE_DIR}/src)

ADD_EXECUTABLE (CppServer
                ${CppServer_Lib_SRCS}
                ${CppServer_SRCS}
                ${CppServer_HDRS}
)

TARGET_LINK_LIBRARIES (CppServer
                       ${Boost_LIBRARIES}
)

ADD_EXECUTABLE (CppServerTest
                ${CppServer_Lib_SRCS}
                ${CppServer_HDRS}
                ${CppServer_Test_SRCS})

TARGET_LINK_LIBRARIES (CppServerTest
                       ${Boost_LIBRARIES}
                       gtest_main
)

SET_TARGET_PROPERTIES (CppServer PROPERTIES DEBUG_POSTFIX "d")
SET_TARGET_PROPERTIES (CppServerTest PROPERTIES DEBUG_POSTFIX "d")

# Supress MSVC-specific compiler and linker warnings.
IF (MSVC)
    # Supress secure warnings (C4996) for MSVC compiler.
    SET_TARGET_PROPERTIES (CppServer PROPERTIES COMPILE_FLAGS
                           "/D _CRT_SECURE_NO_DEPRECATE /D _SCL_SECURE_NO_WARNINGS")
ENDIF (MSVC)
