:: ���� VersionInfo.txt ���� 
:: create by 	lishihai 
:: create time 	18:56 2015/7/29
@echo off

color 0B
echo ִ�б�����ǰ,����ȷ��GenerateUpdateList.bat���������°汾���嵥�ļ�.
pause

::for for Ƕ����һ����Ҫ�����ӳٱ���
set createFileName=versionInfo.txt
del /q %createFileName%

setlocal EnableDelayedExpansion
set /a size=0

::����ÿ��Ŀ¼
for /f %%i in ('dir /Ad /b') do (
    set size=0
    ::����ÿ��Ŀ¼�ļ������ļ��Ĵ�С
    for /f %%j in ('dir /b /s %%i') do set /a size=!size!+%%~zj
    echo %%i !size!>>%createFileName%
)
setlocal DisableDelayedExpansion
 
color 0A
echo %createFileName%�����ɹ�!
set /p p=
exit