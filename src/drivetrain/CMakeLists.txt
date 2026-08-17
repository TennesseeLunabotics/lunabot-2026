cmake_minimum_required(VERSION 3.10)
project(drivetrain VERSION 1.0 LANGUAGES CXX)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
add_library(sparkcan SHARED
    src/PIDController.cpp
    src/SparkBase.cpp
    src/SparkMax.cpp 
    src/SparkFlex.cpp
)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(lunabot_constants REQUIRED)

# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(drivetrain_node src/drivetrain_node.cpp)
target_link_libraries(drivetrain_node sparkcan)
target_include_directories(sparkcan PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(drivetrain_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

ament_target_dependencies(drivetrain_node rclcpp sensor_msgs lunabot_constants)
# Install the library
install(TARGETS sparkcan
    EXPORT sparkcanTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# Install the header files
install(DIRECTORY include/ DESTINATION include)

# Export the targets
install(EXPORT sparkcanTargets
    FILE sparkcanTargets.cmake
    NAMESPACE sparkcan::
    DESTINATION lib/cmake/sparkcan
)

# Create and install the package configuration file
#include(CMakePackageConfigHelpers)
#configure_package_config_file(
#    "${CMAKE_CURRENT_SOURCE_DIR}/sparkcanConfig.cmake.in"
#    "${CMAKE_CURRENT_BINARY_DIR}/sparkcanConfig.cmake"
#    INSTALL_DESTINATION lib/cmake/sparkcan
#)

# Generate the version file for the package
#write_basic_package_version_file(
#    "${CMAKE_CURRENT_BINARY_DIR}/sparkcanConfigVersion.cmake"
#    VERSION ${PROJECT_VERSION}
#    COMPATIBILITY AnyNewerVersion
#)

# Install the config and version files
#install(FILES
#    "${CMAKE_CURRENT_BINARY_DIR}/sparkcanConfig.cmake"
#    "${CMAKE_CURRENT_BINARY_DIR}/sparkcanConfigVersion.cmake"
#    DESTINATION lib/cmake/sparkcan
#)
install(TARGETS drivetrain_node
  EXPORT export_${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
