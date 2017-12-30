cmake_minimum_required(VERSION 3.9)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()
project(secreth CXX)

add_compile_options(-std:c++latest)

find_package(fmt REQUIRED)

link_libraries(fmt::fmt)

add_library(sh-obj STATIC environment.cpp matrix.cpp)

add_library(sh-engine SHARED engine.cpp engine.def)
target_link_libraries(sh-engine PRIVATE sh-obj)

add_executable(sh-interpreter main.cpp)
target_link_libraries(sh-interpreter PRIVATE sh-obj)

add_dependencies(sh-interpreter sh-engine)

if(MSVC)
  get_target_property(_srcs sh-obj SOURCES)

  set_source_files_properties(pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
  list(FILTER _srcs EXCLUDE REGEX "pch\\.cpp$")

  if(NOT CMAKE_GENERATOR MATCHES "Visual Studio .*")
    set_property(SOURCE pch.cpp APPEND PROPERTY OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
    set_property(SOURCE ${_srcs} APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
  endif()

  target_sources(sh-obj PRIVATE pch.cpp)
  target_compile_options(sh-obj PRIVATE /Yupch.h /Zm200)
endif()
