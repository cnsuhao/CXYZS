:: ͳ��һ���ļ����������ļ�����Ŀ¼�ֽڴ�С
:: create by 	lishihai 
:: create time 	15:13 2015/7/29

@echo off

::����ͳ�Ʊ���
set /a num=0
echo ������Ҫ����İ汾Ŀ¼ ��:10101
set /p directory=

if not "directory" == "" (
    ::�ۼ�ÿ���ļ���С
    for /f %%i in ('dir /b /s %directory%') do set /a num+=%%~zi
) else (
    ::���û���������
    goto quit
)

cls
color 0A
echo %directory%�ļ�����Ŀ¼�ܴ�СΪ : %num% �ֽ�
Pause
color 07

:quit