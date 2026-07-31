cmake_minimum_required(VERSION 3.24)
project(AirmonlinkComposerNative VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(airmon_enable_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()
endfunction()

add_library(airmon_core
  src/core/score_model.cpp
  src/core/coordinate_mapper.cpp
  src/core/note_input_controller.cpp
)
target_include_directories(airmon_core PUBLIC src)
airmon_enable_warnings(airmon_core)

enable_testing()
add_executable(airmon_core_tests tests/core_tests.cpp)
target_link_libraries(airmon_core_tests PRIVATE airmon_core)
airmon_enable_warnings(airmon_core_tests)
add_test(NAME airmon_core_tests COMMAND airmon_core_tests)

option(AIRMON_BUILD_QT_APP "Build the Qt 6 desktop application" ON)
if(AIRMON_BUILD_QT_APP)
  find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Gui Test)
  qt_standard_project_setup()

  qt_add_executable(AirmonlinkComposer
    src/app/main.cpp
    src/app/main_window.cpp
    src/app/score_view.cpp
  )
  target_link_libraries(AirmonlinkComposer PRIVATE airmon_core Qt6::Widgets Qt6::Gui)
  airmon_enable_warnings(AirmonlinkComposer)

  set(AIRMON_GIT_SHA "$ENV{GITHUB_SHA}")
  if(NOT AIRMON_GIT_SHA)
    find_package(Git QUIET)
    if(Git_FOUND)
      execute_process(COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE AIRMON_GIT_SHA OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    endif()
  endif()
  if(NOT AIRMON_GIT_SHA)
    set(AIRMON_GIT_SHA "unknown")
  endif()
  target_compile_definitions(AirmonlinkComposer PRIVATE
    AIRMON_APP_VERSION="${PROJECT_VERSION}"
    AIRMON_GIT_SHA="${AIRMON_GIT_SHA}"
  )

  qt_add_executable(airmon_qt_app_tests
    tests/qt_app_tests.cpp
    src/app/main_window.cpp
    src/app/score_view.cpp
  )
  target_link_libraries(airmon_qt_app_tests PRIVATE airmon_core Qt6::Widgets Qt6::Gui Qt6::Test)
  airmon_enable_warnings(airmon_qt_app_tests)
  add_test(NAME airmon_qt_app_tests COMMAND airmon_qt_app_tests)

  install(TARGETS AirmonlinkComposer RUNTIME DESTINATION .)
endif()
