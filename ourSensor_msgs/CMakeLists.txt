#000000cmake_minimum_required(VERSION 2.8.3)
#include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)
cmake_minimum_required(VERSION 2.8.3)
project(ourSensor_msgs)
find_package(catkin REQUIRED COMPONENTS roslib
message_generation std_msgs sensor_msgs )

add_message_files(
  FILES
  Accel.msg
  
)

generate_messages(DEPENDENCIES std_msgs sensor_msgs)

catkin_package(
  DEPENDS
  CATKIN_DEPENDS roslib message_runtime std_msgs sensor_msgs
  INCLUDE_DIRS
  LIBRARIES
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)


#add_dependencies(tactile_servo_msgs ${catkin_EXPORTED_TARGETS})

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

#rosbuild_init()

#set the default path for built executables to the "bin" directory
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})
