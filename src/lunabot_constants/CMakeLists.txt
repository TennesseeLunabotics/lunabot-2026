cmake_minimum_required(VERSION 3.10)
project(lunabot_constants)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

# Install the shared C++ headers.
install(
  DIRECTORY include/
  DESTINATION include
)

# Install the shared Python package.
ament_python_install_package(${PROJECT_NAME})

# Make the include directory available to dependent CMake packages.
ament_export_include_directories(include)

ament_package()