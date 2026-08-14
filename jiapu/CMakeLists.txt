cmake_minimum_required(VERSION 3.21)

project(jiapu VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 默认携带调试符号（RelWithDebInfo）：崩溃日志中的 RVA 可用 addr2line 定位源码行
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
qt_standard_project_setup(REQUIRES 6.5)
set(CMAKE_AUTORCC ON)   # qt_standard_project_setup 未启用资源编译，显式开启

# ---------------- GUI 程序 ----------------
qt_add_executable(jiapu WIN32
    main.cpp
    familytree.h familytree.cpp
    auth.h auth.cpp
    login.h login.cpp login.ui
    mainwindow.h mainwindow.cpp
    treeview.h treeview.cpp
    chartwidget.h chartwidget.cpp
    memberdialog.h memberdialog.cpp
    accountdialog.h accountdialog.cpp
    sorce.qrc
)
target_link_libraries(jiapu PRIVATE Qt6::Widgets)

# ---------------- 数据层单元测试 ----------------
enable_testing()
qt_add_executable(tst_datalayer
    familytree.h familytree.cpp
    auth.h auth.cpp
    tests/tst_datalayer.cpp
)
target_link_libraries(tst_datalayer PRIVATE Qt6::Test Qt6::Core)
target_include_directories(tst_datalayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(tst_datalayer PRIVATE
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
add_test(NAME datalayer COMMAND tst_datalayer)

# ---------------- UI 冒烟测试（离屏渲染截图） ----------------
qt_add_executable(tst_ui
    familytree.h familytree.cpp
    auth.h auth.cpp
    login.h login.cpp login.ui
    mainwindow.h mainwindow.cpp
    treeview.h treeview.cpp
    chartwidget.h chartwidget.cpp
    memberdialog.h memberdialog.cpp
    accountdialog.h accountdialog.cpp
    tests/tst_ui.cpp
    sorce.qrc
)
target_link_libraries(tst_ui PRIVATE Qt6::Test Qt6::Widgets)
target_include_directories(tst_ui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME ui COMMAND tst_ui)
set_tests_properties(ui PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
