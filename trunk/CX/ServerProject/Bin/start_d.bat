@echo off

::�������ط�����
start GateServer_d.exe
ping 127.0.0.1 -n 3 > nul

::������½������
start LoginServer_d.exe
ping 127.0.0.1 -n 1 > nul

::�������ݿ������
start DBServer_d.exe 1
ping 127.0.0.1 -n 1 > nul

::������Ϸ������
start GameServer_d.exe 1