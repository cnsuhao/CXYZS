:: 生成 VersionInfo.txt 工具 
:: create by 	lishihai 
:: create time 	18:56 2015/7/29
@echo off

color 0B
echo 执行本程序前,请先确保GenerateUpdateList.bat生成了最新版本的清单文件.
pause

::for for 嵌套下一般需要开启延迟变量
set createFileName=versionInfo.txt
del /q %createFileName%

setlocal EnableDelayedExpansion
set /a size=0

::遍历每个目录
for /f %%i in ('dir /Ad /b') do (
    set size=0
    ::计算每个目录文件及子文件的大小
    for /f %%j in ('dir /b /s %%i') do set /a size=!size!+%%~zj
    echo %%i !size!>>%createFileName%
)
setlocal DisableDelayedExpansion
 
color 0A
echo %createFileName%创建成功!
set /p p=
exit