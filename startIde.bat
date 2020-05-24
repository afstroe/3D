set CRT_DIR=%cd%
set LIBS_INC="%CRT_DIR%\libs\SDL\include";"%CRT_DIR%\libs\glm";"%CRT_DIR%";"C:\Program Files\Autodesk\FBX\FBX SDK\2020.0.1\include"
set LIBS_DIR=%CRT_DIR%\libs\SDL;C:\Program Files\Autodesk\FBX\FBX SDK\2020.0.1\lib\vs2017\x64\release
set PATH=%PATH%;%LIBS_DIR%
start "Visual Studio" "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\devenv.exe" "%CRT_DIR%\3D.sln"
exit