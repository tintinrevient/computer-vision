#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    string camNum = argv[1];

    string configFilename = "./data/" + camNum + "/intrinsics.xml";
    string inputFilename = "./data/" + camNum + "/checkerboard.avi";
    string cornerFilename = "./data/" + camNum + "/boardcorners.xml";
    string outputFilename = "./data/" + camNum + "/axis.png";
    string camConfigFilename = "./data/config.xml";
    string imageWindow = "Image View";

    FileStorage fs_config(configFilename, FileStorage::READ);

    Mat cameraMatrix, distCoeffs;
    fs_config["camera_matrix"] >> cameraMatrix;
    fs_config["distortion_coefficients"] >> distCoeffs;

    if(!fs_config.isOpened() || cameraMatrix.empty() || distCoeffs.empty())
        return -1;
    fs_config.release();

    Size boardSize = Size(6, 8);
    int winSize = 11;
    float squareSize = 0.115; // unit = meters -> 115 millimeters

    vector<Point2f> imgPoints;
    vector<Point3f> objPoints;
    Mat rvecs, tvecs;

    FileStorage fs_corner(cornerFilename, FileStorage::READ);
    fs_corner["corner_points"] >> imgPoints;

//    cout << "image points: " << endl;
//    cout << imgPoints << endl;

    for(int i = 0; i < boardSize.width; i++)
    {
        for(int j = 0; j < boardSize.height; j++)
        {
            objPoints.push_back(Point3f(float(j * squareSize), float(i * squareSize), 0));
        }
    }

//    cout << "object points: " << endl;
//    cout << objPoints << endl;

    VideoCapture cap;
    cap.open(inputFilename);

    Mat view, viewGray, canvas;

    cap >> view;
    if(view.empty())
        return -1;

    cvtColor(view, viewGray, COLOR_BGR2GRAY);

    vector<Point3f> axisObjPoints;
    axisObjPoints.push_back(Point3f(5*squareSize, 0, 0));
    axisObjPoints.push_back(Point3f(0, 5*squareSize, 0));
    axisObjPoints.push_back(Point3f(0, 0, -5*squareSize));

    vector<Point2f> axisImgPoints;

    bool ok = solvePnP(objPoints, imgPoints, cameraMatrix, distCoeffs, rvecs, tvecs);

    FileStorage fs_cam_config(camConfigFilename, FileStorage::APPEND);
    fs_cam_config << "camera_matrix_" + camNum << cameraMatrix;
    fs_cam_config << "distortion_coefficients_" + camNum << distCoeffs;
    fs_cam_config << "rotation_values_" + camNum << rvecs;
    fs_cam_config << "translation_values_" + camNum << tvecs;

    fs_cam_config.release();

    if(ok)
    {
        projectPoints(axisObjPoints, rvecs, tvecs, cameraMatrix, distCoeffs, axisImgPoints);

        line(view, imgPoints[0], axisImgPoints[0], Scalar(255,0,0), 2);
        line(view, imgPoints[0], axisImgPoints[1], Scalar(0,255,0), 2);
        line(view, imgPoints[0], axisImgPoints[2], Scalar(0,0,255), 2);
        circle(view, imgPoints[0], 4, Scalar(255, 255, 0), -1, FILLED);

        imwrite(outputFilename, view);

        namedWindow(imageWindow, 1);
        imshow(imageWindow, view);
        waitKey(0);
        destroyWindow(imageWindow);

        return 0;
    }
}