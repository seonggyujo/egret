# Builds egret: a Windows tray app that lists the TCP ports this PC listens
# on and stops the servers behind them, written in C on Win32, Direct2D and
# DirectWrite. Two programs come from the same sources: egret.exe, the tray
# app (a console program in Debug so logs show), and egret.com, the command
# line, which cmd and PowerShell run first when "egret" is typed.
cmake_minimum_required(VERSION 3.20)
project(egret LANGUAGES C RC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Flags shared by every target.
add_library(egret_options INTERFACE)
target_include_directories(egret_options INTERFACE src)
target_compile_definitions(egret_options INTERFACE
  UNICODE _UNICODE WIN32_LEAN_AND_MEAN COBJMACROS
  _WIN32_WINNT=0x0A00 NTDDI_VERSION=0x0A000006)
target_compile_options(egret_options INTERFACE -Wall -Wextra -Wno-unused-parameter
  -finput-charset=UTF-8 -fexec-charset=UTF-8)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(egret_options INTERFACE EGRET_DEV)
endif()

# Everything that needs no window. The command line is built from this alone.
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
  src/base/*.c src/probe/*.c src/docker/*.c src/model/*.c src/scan/*.c
  src/actions/*.c src/app/*.c src/cli/*.c)
add_library(egret_core STATIC ${CORE_SOURCES})
target_link_libraries(egret_core PUBLIC egret_options
  iphlpapi ntdll advapi32 shell32 user32 ole32)

# The tray app.
file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS src/render/*.c src/ui/*.c)
add_executable(egret src/main.c ${UI_SOURCES} res/egret.rc)
target_link_libraries(egret PRIVATE egret_core gdi32 dwmapi comctl32 uuid d2d1 dwrite shcore)

# The command line: egret.com next to egret.exe.
add_executable(egret-cli src/main_cli.c res/egret.rc)
target_link_libraries(egret-cli PRIVATE egret_core)
set_target_properties(egret-cli PROPERTIES OUTPUT_NAME egret SUFFIX ".com")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  set_target_properties(egret PROPERTIES WIN32_EXECUTABLE ON)
  target_link_options(egret PRIVATE -s)
  target_link_options(egret-cli PRIVATE -s)
endif()

# Unit tests for the modules that need no window.
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.c)
add_executable(egret-tests ${TEST_SOURCES})
target_include_directories(egret-tests PRIVATE tests)
target_link_libraries(egret-tests PRIVATE egret_core)
enable_testing()
add_test(NAME unit COMMAND egret-tests)
