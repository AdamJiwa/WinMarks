CMAKE_MINIMUM_REQUIRED(VERSION 3.17.0)

project(winMarks CXX)

add_executable(${PROJECT_NAME} main.cpp)

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(BUILD_SHARED_LIBS TRUE)
  # Trust me, Microsoft docs suck. 
  target_compile_options(${PROJECT_NAME} PUBLIC "/Fa${CMAKE_CURRENT_SOURCE_DIR}/main" /FA)
endif()
