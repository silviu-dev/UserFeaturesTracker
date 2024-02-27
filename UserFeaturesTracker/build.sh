#!/bin/sh
#rm -rf build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" && \
cmake --build build --config Release -t UserFeaturesTracker && \
./build/Release/UserFeaturesTracker.exe