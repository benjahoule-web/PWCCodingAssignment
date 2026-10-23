cmake_minimum_required(VERSION 3.16)

project(ObservabilityDashboard
    VERSION 0.1.0
    LANGUAGES CXX
)

include(FetchContent)
FetchContent_Declare(
    fltk
    GIT_REPOSITORY https://github.com/fltk/fltk.git
    GIT_TAG release-1.4.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fltk)

add_executable(ObservabilityDashboard
    src/main.cpp
    src/Frontend/Frontend.cpp
    src/Frontend/Frontend.h
    src/Frontend/MessageTable.cpp
    src/Frontend/MessageTable.h
    src/Frontend/DashboardWindow.cpp
    src/Frontend/DashboardWindow.h
    src/Backend/NetMessage.h
    src/Backend/Backend.cpp
    src/Backend/JSON.cpp
    external/httplib.h

)

target_compile_features(ObservabilityDashboard PRIVATE cxx_std_20)
target_include_directories(ObservabilityDashboard PRIVATE external)
target_link_libraries(ObservabilityDashboard PRIVATE fltk)
