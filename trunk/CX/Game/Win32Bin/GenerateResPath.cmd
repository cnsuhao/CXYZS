:: 创建 mklink 资源目录 
:: create by 	lishihai 
:: create time 	13:20 2015/12/28

echo off
mklink /d Data ..\Resources\Data
mklink /d fonts ..\Resources\fonts
mklink /d Icon ..\Resources\Icon
mklink /d Image ..\Resources\Image
mklink /d ItemIcon ..\Resources\ItemIcon
mklink /d Map ..\Resources\Map
mklink /d MapEffect ..\Resources\MapEffect
mklink /d Object ..\Resources\Object
mklink /d Particle ..\Resources\Particle
mklink /d Public ..\Resources\Public
mklink /d res ..\Resources\res
mklink /d shader ..\Resources\shader
mklink /d Sound ..\Resources\Sound
mklink /d UI ..\Resources\UI
mklink /d UI2 ..\Resources\UI2

mklink NetConfig.xml ..\Resources\NetConfig.xml