cmake_minimum_required(VERSION 3.21)

project(jiapu VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
qt_standard_project_setup(REQUIRES 6.5)

# ---------------- GUI 程序 ----------------
qt_add_executable(jiapu WIN32
    main.cpp
    familytree.h familytree.cpp
    login.h login.cpp login.ui
    mainwindow.h mainwindow.cpp
    sorce.qrc
)
target_link_libraries(jiapu PRIVATE Qt6::Widgets)

# ---------------- 数据层单元测试 ----------------
enable_testing()
qt_add_executable(tst_datalayer
    familytree.h familytree.cpp
    tests/tst_datalayer.cpp
)
target_link_libraries(tst_datalayer PRIVATE Qt6::Test Qt6::Core)
target_include_directories(tst_datalayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(tst_datalayer PRIVATE
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
add_test(NAME datalayer COMMAND tst_datalayer)
