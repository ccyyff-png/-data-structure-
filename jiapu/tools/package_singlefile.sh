#!/bin/bash
# 一键生成单文件 exe（Enigma Virtual Box 打包，双击即用）
# 前置: 已安装 Enigma Virtual Box（免费，官网 enigmaprotector.com/en/downloads.html）
# 用法（Git Bash）: bash tools/package_singlefile.sh [Enigma安装目录]
#   默认安装目录: "C:\Program Files\Enigma Virtual Box"
#
# 技术要点（踩坑记录）:
# - 工程文件必须为 UTF-16LE + BOM（官方模板格式，根元素为空名 <>）
# - 控制台无法创建中文名输出文件 → 先输出 ASCII 名再重命名
# - Qt 6.11 动态库的依赖链在纯内存虚拟化下会卡死（无法定位 pthread_getspecific 等）,
#   全部文件采用「Always Write to Disk」(Action=1) 写盘模式, 退出时自动清理
#   (DeleteExtractedOnExit)
set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_DIR=$(dirname "$SCRIPT_DIR")
DIST="$PROJECT_DIR/dist/jiapu"
ENIGMA_DIR=${1:-"C:\\Program Files\\Enigma Virtual Box"}
CONSOLE="$ENIGMA_DIR\\enigmavbconsole.exe"
OUT_TMP="$PROJECT_DIR/dist/jiapu-portable.exe"
OUT="$PROJECT_DIR/dist/家谱管理系统.exe"

echo "== 1/4 构建便携版 =="
bash "$SCRIPT_DIR/package.sh" > /dev/null

echo "== 2/4 检查 Enigma 安装 =="
CONSOLE_U=$(cygpath -u "$CONSOLE" 2>/dev/null || echo "$ENIGMA_DIR/enigmavbconsole.exe")
if [ ! -f "$CONSOLE_U" ]; then
    echo "错误: 未找到 enigmavbconsole.exe（$ENIGMA_DIR）"
    echo "请先安装 Enigma Virtual Box: https://enigmaprotector.com/en/downloads.html"
    exit 1
fi

echo "== 3/4 生成 Enigma 工程（官方模板结构，UTF-16LE + BOM，写盘模式） =="
PROJ="$PROJECT_DIR/build/singlefile.evb"
DIST_WIN=$(cygpath -w "$DIST" 2>/dev/null || echo "$DIST")
EXE_WIN="$DIST_WIN\\jiapu.exe"
OUT_WIN=$(cygpath -w "$OUT_TMP" 2>/dev/null || echo "$OUT_TMP")
XML_TMP="$PROJECT_DIR/build/singlefile_utf8.xml"

emit_file() {   # $1=虚拟名 $2=真实路径
    echo "					<File>"
    echo "						<Type>2</Type>"
    echo "						<Name>$1</Name>"
    echo "						<File>$2</File>"
    echo "						<ActiveX>false</ActiveX>"
    echo "						<ActiveXInstall>false</ActiveXInstall>"
    echo "						<Action>1</Action>"
    echo "						<OverwriteDateTime>false</OverwriteDateTime>"
    echo "						<OverwriteAttributes>false</OverwriteAttributes>"
    echo "						<PassCommandLine>false</PassCommandLine>"
    echo "					</File>"
}

{
    echo '<?xml encoding="utf-16"?>'
    echo '<>'
    echo "	<InputFile>$EXE_WIN</InputFile>"
    echo "	<OutputFile>$OUT_WIN</OutputFile>"
    echo '	<Files>'
    echo '		<Enabled>true</Enabled>'
    echo '		<DeleteExtractedOnExit>true</DeleteExtractedOnExit>'
    echo '		<CompressFiles>true</CompressFiles>'
    echo '		<Files>'
    echo '			<File>'
    echo '				<Type>3</Type>'
    echo '				<Name>%DEFAULT FOLDER%</Name>'
    echo '				<Action>1</Action>'
    echo '				<OverwriteDateTime>false</OverwriteDateTime>'
    echo '				<OverwriteAttributes>false</OverwriteAttributes>'
    echo '				<Files>'
    for f in "$DIST"/*; do
        [ -f "$f" ] || continue
        name=$(basename "$f")
        [ "$name" = "jiapu.exe" ] && continue
        path_w=$(cygpath -w "$f" 2>/dev/null || echo "$f")
        emit_file "$name" "$path_w"
    done
    for d in "$DIST"/*/; do
        [ -d "$d" ] || continue
        dname=$(basename "$d")
        echo "					<File>"
        echo "						<Type>3</Type>"
        echo "						<Name>$dname</Name>"
        echo "						<Action>1</Action>"
        echo "						<OverwriteDateTime>false</OverwriteDateTime>"
        echo "						<OverwriteAttributes>false</OverwriteAttributes>"
        echo "						<Files>"
        for f in "$d"*; do
            [ -f "$f" ] || continue
            name=$(basename "$f")
            path_w=$(cygpath -w "$f" 2>/dev/null || echo "$f")
            emit_file "$name" "$path_w"
        done
        echo '						</Files>'
        echo '					</File>'
    done
    echo '				</Files>'
    echo '			</File>'
    echo '		</Files>'
    echo '	</Files>'
    echo '	<Packaging>'
    echo '		<Enabled>false</Enabled>'
    echo '	</Packaging>'
    echo '	<Options>'
    echo '		<ShareVirtualSystem>false</ShareVirtualSystem>'
    echo '		<MapExecutableWithTemporaryFile>false</MapExecutableWithTemporaryFile>'
    echo '		<AllowRunningOfVirtualExeFiles>true</AllowRunningOfVirtualExeFiles>'
    echo '	</Options>'
    echo '</>'
} > "$XML_TMP"

printf '\xff\xfe' > "$PROJ"
iconv -f UTF-8 -t UTF-16LE "$XML_TMP" >> "$PROJ"
rm -f "$XML_TMP"

echo "== 4/4 虚拟化打包 =="
rm -f "$OUT" "$OUT_TMP"
MSYS2_ARG_CONV_EXCL='*' "$CONSOLE_U" "$(cygpath -w "$PROJ" 2>/dev/null || echo "$PROJ")"
mv -f "$OUT_TMP" "$OUT"
ls -la "$OUT" && echo "== 完成: $OUT（单文件，双击即用） =="
