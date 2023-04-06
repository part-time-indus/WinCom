@echo off
set "srcPath=D:/workspace/COM/src/com_component"
set "buildPath=D:/workspace/COM/build/debug/com_component"
set "libs=UUID.lib"
if not exist %buildPath% md "%buildPath%"
cd %srcPath%
cl.exe /Fo%buildPath%/guids.obj /Fd%buildPath%/vc140.pdb /c /MTd /EHsc /Zi /Od /D_DEBUG guids.cpp
cl.exe /Fo%buildPath%/cmpnt1.obj /Fd%buildPath%/vc140.pdb /c /MTd  /EHsc /Zi /Od /D_DEBUG cmpnt1.cpp
cd %buildPath%
link /DLL /DEBUG cmpnt1.obj guids.obj %libs% /DEF:%srcPath%/cmpnt1.def
cd %srcPath%  
cl.exe /Fo%buildPath%/create.obj /Fd%buildPath%/vc140.pdb /c /MTd /EHsc /Zi /Od /D_DEBUG create.cpp
cl.exe  /Fo%buildPath%/client1.obj /Fd%buildPath%/vc140.pdb /c /MTd /EHsc /Zi /Od /D_DEBUG client1.cpp
cd %buildPath%
link /DLL /DEBUG create.obj guids.obj User32.lib %libs%
link /OUT:client1.exe /DEBUG client1.obj create.obj guids.obj User32.lib %libs%
pause
