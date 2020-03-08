#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//Define 5 colors with a maximum classification of no more than 5
Scalar color_tab[] = {
        Scalar(0,0,255),
        Scalar(0,255,0),
        Scalar(255,0,0),
        Scalar(0,255,255),
        Scalar(255,0,255)
};

Mat cameraMatrix_cam1, distCoeffs_cam1, rvecs_cam1, tvecs_cam1;
Mat cameraMatrix_cam2, distCoeffs_cam2, rvecs_cam2, tvecs_cam2;
Mat cameraMatrix_cam3, distCoeffs_cam3, rvecs_cam3, tvecs_cam3;
Mat cameraMatrix_cam4, distCoeffs_cam4, rvecs_cam4, tvecs_cam4;

void projectVoxel(int clusterId, Mat &frame, vector<Point2f> &image_points, vector<Point2f> &center, const string &outputFilename);

int main(int argc, char** argv) {
    string frameId = argv[1];
    string camConfigFilename = "./data/config.xml";
    string inputCam1Filename = "./data/cam1/frame" + frameId + ".png";
    string inputCam2Filename = "./data/cam2/frame" + frameId + ".png";
    string inputCam3Filename = "./data/cam3/frame" + frameId + ".png";
    string inputCam4Filename = "./data/cam4/frame" + frameId + ".png";
    string clusterFilename = "./data/cluster/cluster" + frameId + ".xml";

    FileStorage fs_config(camConfigFilename, FileStorage::READ);

    fs_config["camera_matrix_cam1"] >> cameraMatrix_cam1;
    fs_config["distortion_coefficients_cam1"] >> distCoeffs_cam1;
    fs_config["rotation_values_cam1"] >> rvecs_cam1;
    fs_config["translation_values_cam1"] >> tvecs_cam1;

    fs_config["camera_matrix_cam2"] >> cameraMatrix_cam2;
    fs_config["distortion_coefficients_cam2"] >> distCoeffs_cam2;
    fs_config["rotation_values_cam2"] >> rvecs_cam2;
    fs_config["translation_values_cam2"] >> tvecs_cam2;

    fs_config["camera_matrix_cam3"] >> cameraMatrix_cam3;
    fs_config["distortion_coefficients_cam3"] >> distCoeffs_cam3;
    fs_config["rotation_values_cam3"] >> rvecs_cam3;
    fs_config["translation_values_cam3"] >> tvecs_cam3;

    fs_config["camera_matrix_cam4"] >> cameraMatrix_cam4;
    fs_config["distortion_coefficients_cam4"] >> distCoeffs_cam4;
    fs_config["rotation_values_cam4"] >> rvecs_cam4;
    fs_config["translation_values_cam4"] >> tvecs_cam4;

    fs_config.release();

    FileStorage fs_cluster(clusterFilename, FileStorage::READ);

    vector<Point2f> _centers(4);
    fs_cluster["cluster_0_center"] >> _centers[0];
    fs_cluster["cluster_1_center"] >> _centers[1];
    fs_cluster["cluster_2_center"] >> _centers[2];
    fs_cluster["cluster_3_center"] >> _centers[3];
    vector<vector<Point3f>> centers(4);
    for(int i = 0; i < 4; i++) {
        centers[i].push_back(Point3f(_centers[i].x, _centers[i].y, 0));
    }

    vector<vector<Point3f>> clustered_voxels(4);
    fs_cluster["cluster_0"] >> clustered_voxels[0];
    fs_cluster["cluster_1"] >> clustered_voxels[1];
    fs_cluster["cluster_2"] >> clustered_voxels[2];
    fs_cluster["cluster_3"] >> clustered_voxels[3];

    int nclusters = 4;
    for(int clusterId = 0; clusterId < nclusters; clusterId++) {

        Mat frame_cam1 = imread(inputCam1Filename, 1);
        Mat frame_cam2 = imread(inputCam2Filename, 1);
        Mat frame_cam3 = imread(inputCam3Filename, 1);
        Mat frame_cam4 = imread(inputCam4Filename, 1);

        string outputFilename_cam1 = "./data/cam1/cluster_" + to_string(clusterId) + ".png";
        string outputFilename_cam2 = "./data/cam2/cluster_" + to_string(clusterId) + ".png";
        string outputFilename_cam3 = "./data/cam3/cluster_" + to_string(clusterId) + ".png";
        string outputFilename_cam4 = "./data/cam4/cluster_" + to_string(clusterId) + ".png";

        vector<Point2f> _image_points_cam1;
        vector<Point2f> _image_points_cam2;
        vector<Point2f> _image_points_cam3;
        vector<Point2f> _image_points_cam4;

        vector<Point2f> _image_center_cam1;
        vector<Point2f> _image_center_cam2;
        vector<Point2f> _image_center_cam3;
        vector<Point2f> _image_center_cam4;

        projectPoints(clustered_voxels[clusterId], rvecs_cam1, tvecs_cam1, cameraMatrix_cam1, distCoeffs_cam1, _image_points_cam1);
        projectPoints(clustered_voxels[clusterId], rvecs_cam2, tvecs_cam2, cameraMatrix_cam2, distCoeffs_cam2, _image_points_cam2);
        projectPoints(clustered_voxels[clusterId], rvecs_cam3, tvecs_cam3, cameraMatrix_cam3, distCoeffs_cam3, _image_points_cam3);
        projectPoints(clustered_voxels[clusterId], rvecs_cam4, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, _image_points_cam4);

        projectPoints(centers[clusterId], rvecs_cam1, tvecs_cam1, cameraMatrix_cam1, distCoeffs_cam1, _image_center_cam1);
        projectPoints(centers[clusterId], rvecs_cam2, tvecs_cam2, cameraMatrix_cam2, distCoeffs_cam2, _image_center_cam2);
        projectPoints(centers[clusterId], rvecs_cam3, tvecs_cam3, cameraMatrix_cam3, distCoeffs_cam3, _image_center_cam3);
        projectPoints(centers[clusterId], rvecs_cam4, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, _image_center_cam4);

        projectVoxel(clusterId, frame_cam1, _image_points_cam1, _image_center_cam1, outputFilename_cam1);
        projectVoxel(clusterId, frame_cam2, _image_points_cam2, _image_center_cam2, outputFilename_cam2);
        projectVoxel(clusterId, frame_cam3, _image_points_cam3, _image_center_cam3, outputFilename_cam3);
        projectVoxel(clusterId, frame_cam4, _image_points_cam4, _image_center_cam4, outputFilename_cam4);
    }
}

void projectVoxel(int clusterId, Mat &frame, vector<Point2f> &image_points, vector<Point2f> &center, const string &outputFilename) {
    int width = frame.size().width;
    int height = frame.size().height;

    for(int i = 0; i < image_points.size(); i++) {
        int x = image_points[i].x;
        int y = image_points[i].y;

        circle(frame, image_points[i], 2, color_tab[clusterId], -1, FILLED);
    }

    circle(frame, center[0], 10, color_tab[4], -1, FILLED);

    string imageWindow = "Image View";

    imwrite(outputFilename, frame);

    namedWindow(imageWindow, 1);
    imshow(imageWindow, frame);
    waitKey(0);
    destroyWindow(imageWindow);
}