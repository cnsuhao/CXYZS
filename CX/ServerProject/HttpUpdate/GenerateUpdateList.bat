:: 生成 updatelist.txt 工具 
:: create by 	lishihai 
:: create time 	19:20 2015/7/29

@echo off
echo 请输入要生成的版本目录
set /p directory=

del /q %directory%\updatelist.txt

setlocal EnableDelayedExpansion
::echo --------------------------
set /a find=0
set res_path=
for /f " tokens=1-10 delims=\ " %%i in ('dir /a-d /b /s %directory%') do (
     for %%a in (%%i %%j %%k %%l %%m %%n %%o %%p %%q %%r) do (
        if "!find!" == "1" ( 
            if "!res_path!" == "" (set res_path=%%a) else (
                set res_path=!res_path!/%%a
            )
        )
        if "%%a" == "%directory%" ( set /a find=1 )
     )
     echo !res_path!>>%directory%\updatelist.txt
     set res_path=
     set /a find=0
)

color 0A
echo %directory%\updatelist.txt 创建成功!

setlocal DisableDelayedExpansion

