cmake_minimum_required(VERSION 3.24)
project(AirmonlinkComposerNative VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(airmon_core
    src/core/score_model.cpp
    src/core/coordinate_mapper.cpp
    src/core/note_input_controller.cpp
)
target_include_directories(airmon_core PUBLIC src)
target_compile_options(airmon_core PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Werror>)

enable_testing()
add_executable(airmon_core_tests tests/core_tests.cpp)
target_link_libraries(airmon_core_tests PRIVATE airmon_core)
add_test(NAME airmon_core_tests COMMAND airmon_core_tests)

option(AIRMON_BUILD_QT_APP "Build the Qt 6 desktop application" ON)
if(AIRMON_BUILD_QT_APP)
  find_package(Qt6 6.5 COMPONENTS Widgets Gui Test QUIET)
  if(Qt6_FOUND)
    qt_standard_project_setup()
    qt_add_executable(AirmonlinkComposer
      src/app/main.cpp
      src/app/main_window.cpp
      src/app/score_view.cpp
    )
    target_link_libraries(AirmonlinkComposer PRIVATE airmon_core Qt6::Widgets Qt6::Gui)
  else()
    message(WARNING "Qt 6.5+ not found: native core/tests remain buildable; desktop target is skipped")
  endif()
endif()
