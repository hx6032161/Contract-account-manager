cmake_minimum_required(VERSION 3.5)
project(fault_common)


# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Werror -Wextra -Wpedantic)
endif()


# publish fault interface to alarm source
set(platform_common_INCLUDE_DIR ${zdrive_ROOT_DIR}/platform/common/include/fault)

# find dependencies
find_package(interfaces REQUIRED)
find_package(sdk_msgs REQUIRED)

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
INCLUDE_DIRECTORIES(${rclcpp_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${interfaces_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${sdk_msgs_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${std_msgs_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(../../../../../../common/log)
INCLUDE_DIRECTORIES(../../../common/protocol/include)
INCLUDE_DIRECTORIES(${platform_common_INCLUDE_DIR})

include_directories(${glog_INCLUDE_DIR})
link_directories(${glog_LIBRARIES_DIR})

include_directories(${gflags_INCLUDE_DIR})
link_directories(${gflags_LIBRARIES_DIR})

file( GLOB_RECURSE ${PROJECT_NAME}_files *.cpp ../utils/*.cpp) 
message(STATUS "==================================")
message(STATUS ${${PROJECT_NAME}_files})

add_library(fault_common STATIC ${${PROJECT_NAME}_files} )
target_include_directories(fault_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include 
                                            ${CMAKE_CURRENT_SOURCE_DIR}/../../include/utils 
                                            ${CMAKE_CURRENT_SOURCE_DIR}/../../include/fault )

target_link_libraries(${PROJECT_NAME} 
  ${rclcpp_LIBRARIES}
  ${std_msgs_LIBRARIES}
  ${interfaces_LIBRARIES}
  ${sdk_msgs_LIBRARIES}
)

add_library(fault_client SHARED ${${PROJECT_NAME}_files})

target_link_directories(${PROJECT_NAME} PUBLIC ${zdrive_PLATFORM_LIBDIRS})
target_link_libraries(fault_client ${zdrive_PLATFORM_LIBRARIES} fault_common)

install(TARGETS 
    fault_client
    ARCHIVE DESTINATION ${platform_INSTALL_DIR}/om/fault/lib/ COMPONENT platform_release
    LIBRARY DESTINATION ${platform_INSTALL_DIR}/om/fault/lib/ COMPONENT platform_release
    RUNTIME DESTINATION ${platform_INSTALL_DIR}/om/fault/bin/ COMPONENT platform_release
)

# publish to /zdrive/platform/common/private_lib/x86_64/libfault_client/lib
#install(TARGETS 
#   fault_client
#   ARCHIVE DESTINATION ${fault_client_LIBRARIES_DIR} COMPONENT platform_release
#   LIBRARY DESTINATION ${fault_client_LIBRARIES_DIR} COMPONENT platform_release
#)