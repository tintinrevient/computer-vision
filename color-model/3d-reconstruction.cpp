#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    string camNum = argv[1];

    string configFilename = "./data/" + camNum + "/intrinsics.xml";
    string inputFilename = "./data/" + camNum + "/extrinsics.avi";
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

    Size boardSize = Size(8, 6);
    int winSize = 11;
    float squareSize = 115; //  // Actual size of the checkerboard square in millimeters

    vector<Point2f> boardCorners;
    vector<Point2f> imgPoints;
    vector<Point3f> objPoints;
    Mat rvecs, tvecs;

    FileStorage fs_corner(cornerFilename, FileStorage::READ);
    fs_corner["corner_points"] >> boardCorners;

    cout << "checkerboard width: " << boardSize.width << endl;
    cout << "checkerboard height: " << boardSize.height << endl;

    // save the object points and image points
    for (int s = 0; s < boardSize.area(); ++s) {
        float x = float(s / boardSize.width * squareSize);
        float y = float(s % boardSize.width * squareSize);
        float z = 0;

        objPoints.push_back(Point3f(x, y, z));
        imgPoints.push_back(boardCorners[s]);

//        cout << "object point: " << Point3f(x, y, z) << endl;
//        cout << "image point: " << boardCorners[s] << endl;
    }

    VideoCapture cap;
    cap.open(inputFilename);

    Mat view, viewGray, canvas;

    cap >> view;
    if(view.empty())
        return -1;

    cvtColor(view, viewGray, COLOR_BGR2GRAY);

    const float x_len = float(squareSize * (boardSize.height - 1));
    const float y_len = float(squareSize * (boardSize.width - 1));
    const float z_len = float(squareSize * 3);

    vector<Point3f> axisObjPoints;
    axisObjPoints.push_back(Point3f(0, 0, 0));
    axisObjPoints.push_back(Point3f(x_len, 0, 0));
    axisObjPoints.push_back(Point3f(0, y_len, 0));
    axisObjPoints.push_back(Point3f(0, 0, z_len));

    vector<Point2f> axisImgPoints;

    bool ok = solvePnP(objPoints, imgPoints, cameraMatrix, distCoeffs, rvecs, tvecs);

    FileStorage fs_cam_config(camConfigFilename, FileStorage::APPEND);
    fs_cam_config << "camera_matrix_" + camNum << cameraMatrix;
    fs_cam_config << "distortion_coefficients_" + camNum << distCoeffs;
    fs_cam_config << "rotation_values_" + camNum << rvecs;
    fs_cam_config << "translation_values_" + camNum << tvecs;

    fs_cam_config.release();

    if(ok) {
        projectPoints(axisObjPoints, rvecs, tvecs, cameraMatrix, distCoeffs, axisImgPoints);

        line(view, imgPoints[0], axisImgPoints[1], Scalar(255,0,0), 2);
        line(view, imgPoints[0], axisImgPoints[2], Scalar(0,255,0), 2);
        line(view, imgPoints[0], axisImgPoints[3], Scalar(0,0,255), 2);
        circle(view, imgPoints[0], 4, Scalar(255, 255, 0), -1, FILLED);

        imwrite(outputFilename, view);

        namedWindow(imageWindow, 1);
        imshow(imageWindow, view);
        waitKey(0);
        destroyWindow(imageWindow);

        return 0;
    }
}