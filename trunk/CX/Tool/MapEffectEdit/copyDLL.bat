set OutDir=%1
set SourcetDir=%2

::  添加文件到不拷贝列表给xcopy使用.
echo vc90.pdb>uncopy.txt
echo vc100.pdb>>uncopy.txt
echo vc110.pdb>>uncopy.txt
echo vc120.pdb>>uncopy.txt

if not exist "%OutDir%" mkdir "%OutDir%"

xcopy /Y /Q "%SourcetDir%*.lib" "%OutDir%"

xcopy /Y /Q "%SourcetDir%*.dll" "%OutDir%"
xcopy /Y /Q "%SourcetDir%*.pdb" "%OutDir%" /exclude:%OutDir..\%uncopy.txt

:: 删除uncopy.txt文件
del uncopy.txt