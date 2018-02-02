#!/bin/bash

rm -rf .build2
mkdir .build2
cd .build2
#cmake .. -DCMAKE_BUILD_TYPE=Debug
gcc ../main.cpp -lEGL -lOpenGL -o EGLTest
cd ..

