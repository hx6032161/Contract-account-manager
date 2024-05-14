set(om_common_INCLUDE_DIR ${zdrive_ROOT_DIR}/platform/modules/om/common/include)

add_subdirectory( zlog )
add_subdirectory( common )
add_subdirectory( fault )
add_subdirectory( log_manager )
add_subdirectory( pack )
#add_subdirectory( topic_freq_mon )
#add_subdirectory( fault_test )
add_subdirectory( zmonitor )
add_subdirectory( zlog_test )
add_subdirectory( log_transfer )
add_subdirectory( cfg_manager)
add_subdirectory( log_manager2.0)
add_subdirectory( unit_tests )
add_subdirectory( bpf_zprof )

add_subdirectory( multi_modules )

add_subdirectory( board )
add_subdirectory( board_demo )

ZkInstallBuildInfo()
