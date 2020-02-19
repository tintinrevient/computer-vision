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

    Size boardSize = Size(6, 9);
    int winSize = 11;
    float squareSize = 0.025; // unit = meters

    vector<Point3f> objPoints;
    vector<Point2f> imgPoints;
    Mat rvecs, tvecs;

    for(int i = 0; i < boardSize.height; i++)
    {
        for(int j = 0; j < boardSize.width; j++)
        {
            objPoints.push_back(Point3f(float(j * squareSize), float(i * squareSize), 0));
        }
    }

    Mat view, viewGray;

    view = imread("./images/left06.jpg", 1);
    if(view.empty())
        return -1;

    cvtColor(view, viewGray, COLOR_BGR2GRAY);

    bool found = findChessboardCorners(view, boardSize, imgPoints,
                                       CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

    vector<Point3f> axisObjPoints;
    axisObjPoints.push_back(Point3f(5*squareSize, 0, 0));
    axisObjPoints.push_back(Point3f(0, 5*squareSize, 0));
    axisObjPoints.push_back(Point3f(0, 0, -5*squareSize));

    vector<Point3f> cubeObjPoints;
    cubeObjPoints.push_back(Point3f(0, 0, 0));
    cubeObjPoints.push_back(Point3f(0, 2*squareSize, 0));
    cubeObjPoints.push_back(Point3f(2*squareSize, 2*squareSize, 0));
    cubeObjPoints.push_back(Point3f(2*squareSize, 0, 0));
    cubeObjPoints.push_back(Point3f(0, 0, -2*squareSize));
    cubeObjPoints.push_back(Point3f(0, 2*squareSize, -2*squareSize));
    cubeObjPoints.push_back(Point3f(2*squareSize, 2*squareSize, -2*squareSize));
    cubeObjPoints.push_back(Point3f(2*squareSize, 0, -2*squareSize));

    vector<Point2f> axisImgPoints;
    vector<Point2f> cubeImgPoints;

    if(found)
    {
        cornerSubPix(viewGray, imgPoints, Size(winSize, winSize),
                     Size(-1,-1), TermCriteria(TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001));

        bool ok = solvePnP(objPoints, imgPoints, cameraMatrix, distCoeffs, rvecs, tvecs);

        if(ok)
        {
            projectPoints(axisObjPoints, rvecs, tvecs, cameraMatrix, distCoeffs, axisImgPoints);
            line(view, imgPoints[0], axisImgPoints[0], Scalar(255,0,0), 5);
            line(view, imgPoints[0], axisImgPoints[1], Scalar(0,255,0), 5);
            line(view, imgPoints[0], axisImgPoints[2], Scalar(0,0,255), 5);

            projectPoints(cubeObjPoints, rvecs, tvecs, cameraMatrix, distCoeffs, cubeImgPoints);

            line(view, cubeImgPoints[0], cubeImgPoints[1], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[1], cubeImgPoints[2], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[2], cubeImgPoints[3], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[3], cubeImgPoints[0], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[4], cubeImgPoints[5], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[5], cubeImgPoints[6], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[6], cubeImgPoints[7], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[7], cubeImgPoints[4], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[1], cubeImgPoints[5], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[2], cubeImgPoints[6], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[3], cubeImgPoints[7], Scalar(255,255,0), 3);
            line(view, cubeImgPoints[0], cubeImgPoints[4], Scalar(255,255,0), 3);

            namedWindow("Image View", 1);
            imshow("Image View", view);
            imwrite("./output/cube.png", view);
            waitKey(100);
        }
    }
}