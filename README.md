# SRD-G1 software raster-core model

This is a C++ reference model for the SRD-G1 raster pipeline.

## Build

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:\MinGW\bin\g++.exe
cmake --build build-mingw
.\build-mingw\srd-render.exe
ctest --test-dir build-mingw --output-on-failure
```