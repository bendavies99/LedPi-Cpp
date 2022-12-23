cmake_minimum_required(VERSION 3.0.0)
project(ledpi VERSION 0.1.0)

file(COPY config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)

if(!WIN32)
  add_definitions("-std=c++14")
endif()

if(WIN32)
add_compile_definitions(_WIN32_WINNT=0x0601)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_install(PATH_OR_REFERENCE ..
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB_RECURSE source_list "src/*.cpp" "src/*.hpp" "src/**/*.cpp" "src/**/*.hpp")

add_executable(ledpi ${source_list})
add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
# add_compile_definitions(PRODUCTION_MODE)
add_compile_definitions(REMOTE_DEBUG_DISPLAY)
target_include_directories(ledpi PRIVATE vendor)
include_directories(vendor)
target_link_libraries(ledpi ${CONAN_LIBS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
