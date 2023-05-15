REM cleanup root directory

rmdir /S /Q .vs
rmdir /S /Q bin
rmdir /S /Q bin-int

Rem This deletes imgui's example projects, but whatever
del /F /S *.vcxproj
del /F /S *.filters
del /F /S *.user
del /F *.sln