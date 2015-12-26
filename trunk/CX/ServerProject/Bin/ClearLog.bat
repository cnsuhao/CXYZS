::删除日志文件
:: create by 	lishihai 
:: create time 	11:28 2015/8/27
@echo off

for /f %%i in ('dir /b') do ( 
    if "%%~xi" == ".html" ( del %%i )
)
