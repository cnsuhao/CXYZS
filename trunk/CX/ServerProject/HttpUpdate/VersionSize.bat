:: ͳ��һ���ļ����������ļ�����Ŀ¼�ֽڴ�С
:: create by 	lishihai 
:: create time 	15:13 2015/7/29

@echo off

::����ͳ�Ʊ���
set /a num=0

if not "%1" == "" (
    ::�ۼ�ÿ���ļ���С
    for /f %%i in ('dir /b /s %1') do set /a num+=%%~zi
) else (
    ::���û���������
    echo ��Ҫ�汾Ŀ¼������������� ��:
    echo VesrionSize.bat C:\HttpUpdate\10101
    Pause
    goto quit
)

cls
color 0A
echo %1�ļ�����Ŀ¼�ܴ�СΪ : %num% �ֽ�
Pause
color 07

:quit