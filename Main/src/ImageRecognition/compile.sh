#!/bin/bash

g++ ./main.cpp ./colonydetection.cpp ./conversion.cpp ./detectionsettings.cpp ./dishdetection.cpp ./imageediting.cpp ./preprocessing.cpp ./test.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -o main
