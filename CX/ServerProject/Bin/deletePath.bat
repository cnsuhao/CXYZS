::添加环境变量GAME_TABLE_PATH
@echo off

for /f "tokens=1 delims=, " %%i in ('reg query "HKLM\SYSTEM\ControlSet001\Control\Session Manager\Environment"') do ( 
    if "%%i" == "GAMESERVER_TABLE_PATH" (
        reg delete "HKLM\SYSTEM\ControlSet001\Control\Session Manager\Environment" /v "%%i" /f
        pause>nul
    )
)
