:: 统计一个文件夹下所有文件和子目录字节大小
:: create by 	lishihai 
:: create time 	15:13 2015/7/29

@echo off

::设置统计变量
set /a num=0

if not "%1" == "" (
    ::累加每个文件大小
    for /f %%i in ('dir /b /s %1') do set /a num+=%%~zi
) else (
    ::如果没有输入参数
    echo 需要版本目录参数来参与计算 如:
    echo VesrionSize.bat C:\HttpUpdate\10101
    Pause
    goto quit
)

cls
color 0A
echo %1文件及子目录总大小为 : %num% 字节
Pause
color 07

:quit