cmake_minimum_required(VERSION 3.5)
project(om_fault_manager)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD 17)

# publish fault interface to alarm source
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

message(STATUS "~~~~~~~~~~~~~~~om_fault~~~~~~~~~~~~~~~~~~~~~~~~~~~" )
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
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/om_fault.cpp ${${PROJECT_NAME}_files})
target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_INTRA_PROCESS_COMMS=false)

target_link_directories(${PROJECT_NAME} PUBLIC ${zdrive_PLATFORM_LIBDIRS})

target_link_libraries(${PROJECT_NAME} 
  ${rclcpp_LIBRARIES}
  ${std_msgs_LIBRARIES}
  ${interfaces_LIBRARIES}
  ${sdk_msgs_LIBRARIES}
  fault_common
  pthread common
  ${zdrive_PLATFORM_LIBRARIES}
  #spdlog
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS 
  ${PROJECT_NAME}
  ARCHIVE DESTINATION ${platform_INSTALL_DIR}/om/fault/lib/ COMPONENT platform_release
  LIBRARY DESTINATION ${platform_INSTALL_DIR}/om/fault/lib/ COMPONENT platform_release
  RUNTIME DESTINATION ${platform_INSTALL_DIR}/om/fault/bin/ COMPONENT platform_release
)


add_library(${PROJECT_NAME}_node SHARED ${CMAKE_CURRENT_SOURCE_DIR}/om_fault.cpp ${${PROJECT_NAME}_files})
target_compile_definitions(${PROJECT_NAME}_node PUBLIC -DUSE_INTRA_PROCESS_COMMS=true -DMODULE_NAME="${PROJECT_NAME}" -DMODULE_USE_SO)
target_link_directories(${PROJECT_NAME}_node PUBLIC ${zdrive_PLATFORM_LIBDIRS})
target_link_libraries(${PROJECT_NAME}_node 
  ${rclcpp_LIBRARIES}
  ${std_msgs_LIBRARIES}
  ${interfaces_LIBRARIES}
  ${sdk_msgs_LIBRARIES}
  fault_common
  pthread common
  ${zdrive_PLATFORM_LIBRARIES}
  #spdlog
)
target_include_directories(${PROJECT_NAME}_node PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
install(TARGETS 
  ${PROJECT_NAME}_node
  ARCHIVE DESTINATION ${platform_INSTALL_DIR}/om/fault/lib/ COMPONENT platform_release
  LIBRARY DESTINATION ${platform_INSTALL_DIR}/om/fault/lib/ COMPONENT platform_release
  RUNTIME DESTINATION ${platform_INSTALL_DIR}/om/fault/bin/ COMPONENT platform_release
)


install(FILES config/fault_meta_hdmap_info.xml
  DESTINATION ${platform_INSTALL_DIR}/common/conf
  COMPONENT platform_release
)
install(FILES config/fault_meta_percp_info.xml
  DESTINATION ${platform_INSTALL_DIR}/common/conf
  COMPONENT platform_release
)
install(FILES config/fault_meta_platform_info.xml
  DESTINATION ${platform_INSTALL_DIR}/common/conf
  COMPONENT platform_release
)
install(FILES config/fault_meta_pnc_info.xml
  DESTINATION ${platform_INSTALL_DIR}/common/conf
  COMPONENT platform_release
)
install(FILES config/fault_cfg.xml
  DESTINATION ${platform_INSTALL_DIR}/common/conf
  COMPONENT platform_release
)


message(STATUS "~~~~~~~~~~~~~~~om_fault success~~~~~~~~~~~~~~~~~~~~~~~~~~~" )