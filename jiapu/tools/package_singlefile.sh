#!/bin/bash
# 一键生成单文件 exe（Enigma Virtual Box 虚拟化打包，免解压、点击即用）
# 前置: 已安装 Enigma Virtual Box（免费，官网 enigmaprotector.com/en/downloads.html）
# 用法（Git Bash）: bash tools/package_singlefile.sh [Enigma安装目录]
#   默认安装目录: "C:\Program Files\Enigma Virtual Box"
set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_DIR=$(dirname "$SCRIPT_DIR")
DIST="$PROJECT_DIR/dist/jiapu"
ENIGMA_DIR=${1:-"C:\\Program Files\\Enigma Virtual Box"}
CONSOLE="$ENIGMA_DIR\\enigmavbconsole.exe"
OUT="$PROJECT_DIR/dist/家谱管理系统.exe"

echo "== 1/4 构建便携版 =="
bash "$SCRIPT_DIR/package.sh"

echo "== 2/4 检查 Enigma 安装 =="
if [ ! -f "$(cygpath -u "$CONSOLE" 2>/dev/null || echo "$ENIGMA_DIR/enigmavbconsole.exe")" ]; then
    echo "错误: 未找到 enigmavbconsole.exe（$ENIGMA_DIR）"
    echo "请先安装 Enigma Virtual Box: https://enigmaprotector.com/en/downloads.html"
    exit 1
fi

echo "== 3/4 生成 Enigma 工程（虚拟文件系统 = 便携版全部文件） =="
DIST_WIN=$(cygpath -w "$DIST" 2>/dev/null || echo "$DIST")
EXE_WIN="$DIST_WIN\\jiapu.exe"
OUT_WIN=$(cygpath -w "$OUT" 2>/dev/null || echo "$OUT")
PROJ="$PROJECT_DIR/build/singlefile.evb"

{
    echo '<?xml version="1.0" encoding="UTF-8"?>'
    echo '<EnigmaVirtualBoxProject>'
    echo '    <Options>'
    echo '        <Files>'
    echo '            <CompressFiles>true</CompressFiles>'
    echo '            <CheckFilesExistOnStartup>true</CheckFilesExistOnStartup>'
    echo '        </Files>'
    echo '    </Options>'
    echo '    <InputFiles>'
    echo "        <InputFile name=\"$EXE_WIN\">"
    echo '            <FileSystem>'
    echo '                <FileEntryFolder name="%DEFAULT FOLDER%" />'
    while IFS= read -r f; do
        rel="${f#$DIST/}"
        if [ "$rel" = "jiapu.exe" ]; then continue; fi
        case "$rel" in
            */*)
                dir="${rel%/*}"
                echo "                <FileEntryFolder name=\"%DEFAULT FOLDER%\\$dir\">"
                echo "                    <FileEntryFile name=\"$(cygpath -w "$f")\" />"
                echo "                </FileEntryFolder>"
                ;;
            *)
                echo "                <FileEntryFile name=\"$(cygpath -w "$f")\" />"
                ;;
        esac
    done < <(find "$DIST" -type f | sort)
    echo '            </FileSystem>'
    echo '        </InputFile>'
    echo '    </InputFiles>'
    echo "    <OutputFile name=\"$OUT_WIN\" />"
    echo '</EnigmaVirtualBoxProject>'
} > "$PROJ"

echo "== 4/4 虚拟化打包 =="
"$(cygpath -u "$CONSOLE")" "$(cygpath -w "$PROJ")"
ls -la "$OUT"
echo "== 完成: $OUT =="
