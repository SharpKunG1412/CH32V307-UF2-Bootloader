@echo off

color 07

cmake -S . -B build -G "MinGW Makefiles"
cmake --build build -j32

color 07
