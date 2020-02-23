# Voxel Reconstruction

## Command to compile the source code on Mac OS with OpenCV 4.2
```
$ g++ $(pkg-config --cflags --libs opencv4) -std=c++11 -o target-file-name source-file-name.cpp
```