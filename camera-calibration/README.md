# Camera Calibration

## Install OpenCV 3.4 on Mac OS (C++)

Install OpenCV:
```
brew search opencv
brew install opencv@3
```

Update .bash_profile:
```
export PATH="/usr/local/opt/opencv@3/bin:$PATH"
export LDFLAGS="-L/usr/local/opt/opencv@3/lib"
export CPPFLAGS="-I/usr/local/opt/opencv@3/include"
export PKG_CONFIG_PATH="/usr/local/opt/opencv@3/lib/pkgconfig"
```

Verify:
```
pkg-config --cflags --libs opencv
```

## Compile and Run

Option 1 - Use CMake:
```
cmake .
make
```

Option 2 - Use g++:
```
g++ $(pkg-config --cflags --libs opencv) -std=c++11 CameraCalibration.cpp -o CameraCalibration
```

## References
* https://medium.com/@jaskaranvirdi/setting-up-opencv-and-c-development-environment-in-xcode-b6027728003
* https://docs.opencv.org/3.1.0/db/df5/tutorial_linux_gcc_cmake.html#gsc.tab=0






