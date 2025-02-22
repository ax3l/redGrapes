cmake_minimum_required(VERSION 3.10.0)

include(CMakeFindDependencyMacro)

project(redGrapes VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost 1.62.0 REQUIRED COMPONENTS context)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

add_library(redGrapes
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/resource/resource.cpp
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/dispatch/thread/execute.cpp
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/scheduler/event.cpp
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/scheduler/event_ptr.cpp
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/task/property/graph.cpp
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/task/allocator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/task/task_space.cpp
  ${CMAKE_CURRENT_LIST_DIR}/redGrapes/redGrapes.cpp
)

target_compile_features(redGrapes PUBLIC
    cxx_std_14
)

target_include_directories(redGrapes PUBLIC
    $<BUILD_INTERFACE:${redGrapes_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${redGrapes_INSTALL_PREFIX}>
)

target_link_libraries(redGrapes PUBLIC ${Boost_LIBRARIES})
target_link_libraries(redGrapes PUBLIC fmt::fmt)
target_link_libraries(redGrapes PUBLIC spdlog::spdlog)

set(redGrapes_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}")
set(redGrapes_INCLUDE_DIRS ${redGrapes_INCLUDE_DIRS} "${CMAKE_CURRENT_LIST_DIR}/share/thirdParty/akrzemi/optional/include")
set(redGrapes_INCLUDE_DIRS ${redGrapes_INCLUDE_DIRS} "${CMAKE_CURRENT_LIST_DIR}/share/thirdParty/cameron314/concurrentqueue/include")
set(redGrapes_LIBRARIES ${Boost_LIBRARIES} fmt::fmt spdlog::spdlog)

target_include_directories(redGrapes PUBLIC ${redGrapes_INCLUDE_DIRS})

