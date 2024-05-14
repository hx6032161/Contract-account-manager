# 对于业务部分的编译，先通过符号链接的方式链接到原来的位置保证编译通过，后续按照SDK的方式提供
# file(MAKE_DIRECTORY "${zdrive_ROOT_DIR}/common/log")
# file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/include/zlog.h" "${zdrive_ROOT_DIR}/common/log/zlog.h" SYMBOLIC)

# 对于SDK方式，需要把头文件安装到头文件目录
file(GLOB zlog_headers ${CMAKE_CURRENT_SOURCE_DIR}/zlog/*)
install(FILES ${zlog_headers}
  DESTINATION ${platform_INSTALL_DIR}/include/zlog
  COMPONENT platform_sdk
)
