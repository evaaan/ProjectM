@echo off
REM Save the current directory and switch to the flatc directory
pushd "..\lib\vcpkg\installed\x64-windows\tools\flatbuffers"

REM Run the flatc command with correct relative path
flatc -o "../../../../../../common/fbs/" -c "../../../../../../common/fbs/entity.fbs"

REM Return to the original directory
popd