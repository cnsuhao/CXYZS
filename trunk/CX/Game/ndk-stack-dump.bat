set dir=%~dp0

"%NDK_ROOT%/ndk-stack" -sym "%dir%proj.android/obj/local/armeabi" -dump "%dir%logcat.txt"

