cmake_minimum_required(VERSION 3.5)
project(om_fault_test)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD 17)

set(platform_common_INCLUDE_DIR ${zdrive_ROOT_DIR}/platform/common/include/fault)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Werror -Wextra -Wpedantic)
endif()

# find dependencies

find_package(interfaces REQUIRED)
find_package(sdk_msgs REQUIRED)

# Eigen3
# find_package(Eigen3 REQUIRED)
# include_directories(${eigen_INCLUDE_DIR})
# ros2 related
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
INCLUDE_DIRECTORIES(${rclcpp_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${interfaces_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${std_msgs_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${sdk_msgs_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${platform_common_INCLUDE_DIR})

message(STATUS "~~~~~~~~~~~~~~~om_fault_test~~~~~~~~~~~~~~~~~~~~~~~~~~~" )
message(STATUS ${rclcpp_INCLUDE_DIR})

INCLUDE_DIRECTORIES(../../../../common/log)
INCLUDE_DIRECTORIES(../common/protocol/include)

include_directories(${glog_INCLUDE_DIR})
link_directories(${glog_LIBRARIES_DIR})

include_directories(${gflags_INCLUDE_DIR})
link_directories(${gflags_LIBRARIES_DIR})

include_directories(${protobuf_INCLUDE_DIR})
link_directories(${protobuf_LIBRARIES_DIR})

link_directories(${ros2_LIBRARIES_DIR})


FILE( GLOB_RECURSE ${PROJECT_NAME}_files *.cpp) 
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/fault_test.cpp ${${PROJECT_NAME}_files})

target_link_libraries(${PROJECT_NAME} 
  ${rclcpp_LIBRARIES}
  ${std_msgs_LIBRARIES}
  ${interfaces_LIBRARIES}
  ${sdk_msgs_LIBRARIES}
  ${fault_client_LIBRARIES_DIR}/libfault_client.so
  pthread common
  #spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS 
  ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
)
