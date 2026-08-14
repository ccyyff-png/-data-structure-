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
)
target_link_libraries(jiapu PRIVATE Qt6::Widgets)

# 资源文件：常规构建走 AUTORCC；静态构建下 AUTORCC 的 rcc list 会崩溃（0xc0000409），
# 改在配置期手动调用 rcc 生成 qrc_sorce.cpp
if(DEFINED QT6_STATIC_PREFIX AND QT6_STATIC_PREFIX)
    execute_process(
        COMMAND "${QT6_STATIC_PREFIX}/share/qt6/bin/rcc.exe"
                --name sorce
                --output "${CMAKE_CURRENT_BINARY_DIR}/qrc_sorce.cpp"
                "${CMAKE_CURRENT_SOURCE_DIR}/sorce.qrc"
        RESULT_VARIABLE _rcc_result
    )
    if(NOT _rcc_result EQUAL 0)
        message(FATAL_ERROR "rcc 生成资源文件失败（exit ${_rcc_result}）")
    endif()
    target_sources(jiapu PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/qrc_sorce.cpp")
else()
    target_sources(jiapu PRIVATE sorce.qrc)
endif()

# 静态 Qt 发布构建（单文件 exe，无任何 DLL 依赖）：
#   cmake -S . -B build-static -G Ninja -DCMAKE_BUILD_TYPE=Release \
#         -DCMAKE_PREFIX_PATH=<msys2>/mingw64/qt6-static \
#         -DQT6_STATIC_PREFIX=<msys2>/mingw64/qt6-static -DBUILD_TESTING=OFF
if(DEFINED QT6_STATIC_PREFIX AND QT6_STATIC_PREFIX)
    target_compile_definitions(jiapu PRIVATE QT_STATIC)
    target_link_options(jiapu PRIVATE -static)
    # 插件静态库 + 其资源对象 + 系统依赖（依插件 .prl 清单）
    target_link_libraries(jiapu PRIVATE
        "${QT6_STATIC_PREFIX}/lib/objects-Release/QWindowsIntegrationPlugin_resources_1/.qt/rcc/qrc_openglblacklists_init.cpp.obj"
        "${QT6_STATIC_PREFIX}/lib/objects-Release/QWindowsIntegrationPlugin_resources_2/.qt/rcc/qrc_cursors_init.cpp.obj"
        "${QT6_STATIC_PREFIX}/share/qt6/plugins/platforms/libqwindows.a"
        -ldwmapi -limm32 -loleaut32 -lsetupapi -lshlwapi -lwinspool -lwtsapi32
        -lshcore -lcomdlg32 -ld3d9 -lwinmm -lruntimeobject -lole32
        "${QT6_STATIC_PREFIX}/share/qt6/plugins/imageformats/libqjpeg.a"
        -ljpeg)
endif()

# ---------------- 测试 ----------------
include(CTest)
if(BUILD_TESTING)
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
endif()
