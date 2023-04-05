cmake_minimum_required(VERSION 3.0.2)
project(sensor)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  wittenstein_msgs
  rosbag
  ourSensor_msgs
)

# Generate messages in the 'msg' folder
# add_message_files(
#   FILES
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs
# )

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )



catkin_package(
  #INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp rospy std_msgs wittenstein_msgs ourSensor_msgs
  #DEPENDS system_lib

)
catkin_install_python(PROGRAMS src/accel_pub.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  ${catkin_INCLUDE_DIRS}
  include
)

	
#add_executable(wittenstein_main src/main.cpp src/HexFT.cpp)
#target_link_libraries(wittenstein_main ${catkin_LIBRARIES})

# OUTDATED
#add_executable(active_data src/data_call.cpp src/HexFT.cpp)
#add_dependencies(active_data ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries(active_data
#  ${catkin_LIBRARIES}
#)

 add_executable(get_force src/get_force.cpp src/HexFT.cpp)
 add_dependencies(get_force ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
 target_link_libraries(get_force
   ${catkin_LIBRARIES}
 )

 add_executable(force_control src/force_control.cpp src/HexFT.cpp)
 add_dependencies(force_control ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
 target_link_libraries(force_control
   ${catkin_LIBRARIES}
 )

# OUTDATED
#add_executable(data_force src/data_force.cpp src/HexFT.cpp)
#add_dependencies(data_force ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#target_link_libraries(data_force
#  ${catkin_LIBRARIES}
#)

#  add_executable(active_position src/data_call_position_control.cpp src/HexFT.cpp)
#  add_dependencies(active_position ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#  target_link_libraries(active_position
#    ${catkin_LIBRARIES}
#  )

#  add_executable(impulse_coll src/impulse_coll.cpp src/HexFT.cpp)
#  add_dependencies(impulse_coll ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#  target_link_libraries(impulse_coll
#    ${catkin_LIBRARIES}
#  )

 add_executable(force_pub src/force_pub.cpp src/HexFT.cpp)
 add_dependencies(force_pub ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
 target_link_libraries(force_pub
   ${catkin_LIBRARIES}
 )
 
 
add_executable(my_test src/test.cpp src/HexFT.cpp)
 add_dependencies(my_test ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
 target_link_libraries(my_test
   ${catkin_LIBRARIES}
 )

add_executable(accel_sub src/accel_sub.cpp)
add_dependencies(accel_sub ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(accel_sub
   ${catkin_LIBRARIES}
 )

add_executable(hall_calibrate src/hall_calibrate.cpp)
add_dependencies(hall_calibrate ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(hall_calibrate
   ${catkin_LIBRARIES}
 )

add_executable(experiment src/experiment.cpp)
add_dependencies(experiment ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(experiment
   ${catkin_LIBRARIES}
 )

add_executable(contact_ur src/contact_slide.cpp)
add_dependencies(contact_ur ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(contact_ur
   ${catkin_LIBRARIES}
 )

