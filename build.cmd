@echo off

set CFLAGS=/nologo /W3 /Z7 /GS- /Gs999999
set LDFLAGS=/incremental:no /opt:icf /opt:ref
set BASE_FILES=../wol.c

if exist bin rmdir /s /q bin
mkdir bin
pushd bin

if [%1] == [] call cl -D_DEBUG -Od -Fewol.exe %CFLAGS% %BASE_FILES% /link %LDFLAGS% & echo Debug build complete...
if [%1] == [-r] call cl -O2 -Fewol.exe %CFLAGS% %BASE_FILES% /link %LDFLAGS% & echo Release build complete...

popd