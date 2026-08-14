#!/bin/bash
# 一键打包分发版到 dist/jiapu
# 用法（Git Bash，MSYS2 环境）: bash tools/package.sh
# 说明: windeployqt 只复制 Qt 动态库，不复制 MinGW 编译器运行库
# （libstdc++-6.dll / libgcc_s_seh-1.dll / libwinpthread-1.dll 等），
# 本脚本自动扫描 exe 与全部 DLL 的依赖并补齐（仅复制 ucrt64/bin 中存在的运行库）。
set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_DIR=$(dirname "$SCRIPT_DIR")
BIN=/d/msys64/ucrt64/bin
DIST="$PROJECT_DIR/dist/jiapu"
BUILD="$PROJECT_DIR/build"

export PATH="$BIN:$PATH"

echo "== 1/3 构建 =="
cmake --build "$BUILD" --target jiapu

echo "== 2/3 windeployqt =="
rm -rf "$DIST"
mkdir -p "$DIST"
cp "$BUILD/jiapu.exe" "$DIST/"
windeployqt --no-translations --no-system-d3d-compiler --no-opengl-sw \
    "$BUILD/jiapu.exe" --dir "$DIST" > /dev/null

echo "== 3/3 补齐 MinGW 运行库依赖 =="
for pass in 1 2 3; do
    copied=0
    for f in $(find "$DIST" \( -name "*.exe" -o -name "*.dll" \)); do
        for dll in $(objdump -p "$f" 2>/dev/null | grep -oE "DLL Name: [A-Za-z0-9_.+-]+\.dll" | sed 's/DLL Name: //'); do
            # 仅复制 MSYS2 运行库（Windows 系统 DLL 不在 ucrt64/bin 中，自然跳过）
            if [ -f "$BIN/$dll" ] && [ ! -f "$DIST/$dll" ]; then
                cp "$BIN/$dll" "$DIST/"
                echo "  + $dll"
                copied=$((copied + 1))
            fi
        done
    done
    [ $copied -eq 0 ] && break
done

echo "== 完成: $DIST =="
du -sh "$DIST"
