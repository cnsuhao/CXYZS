:: 统计一个文件夹下所有文件和子目录字节大小
:: create by 	lishihai 
:: create time 	15:13 2015/7/29

@echo off

::设置统计变量
set /a num=0
echo 请输入要计算的版本目录 如:10101
set /p directory=

if not "directory" == "" (
    ::累加每个文件大小
    for /f %%i in ('dir /b /s %directory%') do set /a num+=%%~zi
) else (
    ::如果没有输入参数
    goto quit
)

cls
color 0A
echo %directory%文件及子目录总大小为 : %num% 字节
Pause
color 07

:quit