CMAKE_MINIMUM_REQUIRED(VERSION 3.17.0)

project(winMarks CXX)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
configure_file(
    WinMarks.rc.in
    WinMarks.rc
    )
add_executable(${PROJECT_NAME} WIN32 main.cpp NotifyIcon.cpp resource.h ${CMAKE_CURRENT_BINARY_DIR}/WinMarks.rc)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    )

set_target_properties(${PROJECT_NAME} PROPERTIES VS_GLOBAL_ROOTNAMESPACE "${PROJECT_NAME}") 
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_GLOBAL_ApplicationIcon "${CMAKE_CURRENT_SOURCE_DIR}/res/Icon_32x32.ico")

if(MSVC)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(BUILD_SHARED_LIBS TRUE)
  # Trust me, Microsoft docs suck. 
  # target_compile_options(${PROJECT_NAME} PUBLIC "/Fa${CMAKE_CURRENT_SOURCE_DIR}/main" /FA)
endif()
