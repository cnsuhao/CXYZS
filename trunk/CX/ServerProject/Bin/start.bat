@echo off

::启动网关服务器
start GateServer.exe
ping 127.0.0.1 -n 3 > nul

::启动登陆服务器
start LoginServer.exe
ping 127.0.0.1 -n 1 > nul

::启动数据库服务器
start DBServer.exe 1
ping 127.0.0.1 -n 1 > nul

::启动游戏服务器
start GameServer.exe 1
