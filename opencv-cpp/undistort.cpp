#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    string cameraConfigFilename = "camera.yml";
    FileStorage fs(cameraConfigFilename, FileStorage::READ);

    Mat cameraMatrix, distCoeffs;

    fs["camera_matrix"] >> cameraMatrix;
    fs["distortion_coefficients"] >> distCoeffs;

    Mat src, dst;

    src = imread("./images/left06.jpg", 1);
    if(src.empty())
        return -1;

    undistort(src, dst, cameraMatrix, distCoeffs, noArray());

    namedWindow("Image View", 1);
    imshow("Image View", dst);
    imwrite("./output/undistort.png", dst);
    waitKey(500);
}