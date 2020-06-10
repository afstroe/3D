set CRT_DIR=%cd%
set OPENCV_LIB=C:\.alin\devel\OpenCV\_opencv\install
set OPENCV_DLL=C:\.alin\devel\OpenCV\_opencv\bin\Debug;C:\.alin\devel\OpenCV\_opencv\bin\Release
set LIBS_INC="%CRT_DIR%\libs\SDL\include";"%CRT_DIR%\libs\glm";"%CRT_DIR%";"%CRT_DIR%\libs\FBX\include";"%OPENCV_LIB%\include"
set LIBS_DIR=%CRT_DIR%\libs\SDL;%OPENCV_LIB%\x64\vc16\lib
set FBXSDK_DIR=C:\Program Files\Autodesk\FBX\FBX SDK\2020.0.1\lib\vs2017\x64\
set PATH=%PATH%;%LIBS_DIR%;%OPENCV_DLL%
start "Visual Studio" "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe" "%CRT_DIR%\3D.sln"
exit