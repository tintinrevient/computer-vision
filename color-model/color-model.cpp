#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void projectVoxel(Mat &frame, vector<Point2f> &image_points, const string &outputFilename);

int main(int argc, char** argv)
{
    string inputCam1Filename = "./data/cam1/frame.png";
    string inputCam2Filename = "./data/cam2/frame.png";
    string inputCam3Filename = "./data/cam3/frame.png";
    string inputCam4Filename = "./data/cam4/frame.png";
    string camConfigFilename = "./data/config.xml";
    string clusterFilename = "./data/cluster.xml";

    FileStorage fs_config(camConfigFilename, FileStorage::READ);

    Mat cameraMatrix_cam1, distCoeffs_cam1, rvecs_cam1, tvecs_cam1;
    Mat cameraMatrix_cam2, distCoeffs_cam2, rvecs_cam2, tvecs_cam2;
    Mat cameraMatrix_cam3, distCoeffs_cam3, rvecs_cam3, tvecs_cam3;
    Mat cameraMatrix_cam4, distCoeffs_cam4, rvecs_cam4, tvecs_cam4;

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
    vector<Point3f> voxels_cluster1;
    vector<Point3f> voxels_cluster2;
    vector<Point3f> voxels_cluster3;
    vector<Point3f> voxels_cluster4;
    fs_cluster["cluster_0"] >> voxels_cluster1;
    fs_cluster["cluster_1"] >> voxels_cluster2;
    fs_cluster["cluster_2"] >> voxels_cluster3;
    fs_cluster["cluster_3"] >> voxels_cluster4;

    vector<vector<Point3f>> voxels;
    voxels.push_back(voxels_cluster1);
    voxels.push_back(voxels_cluster2);
    voxels.push_back(voxels_cluster3);
    voxels.push_back(voxels_cluster4);

    fs_cluster.release();

    string outputFilename_cam1 = "./data/cam1/voxel.png";
    Mat frame_cam1 = imread(inputCam1Filename, 1);
    vector<Point2f> _image_points_cam1;

    string outputFilename_cam2 = "./data/cam2/voxel.png";
    Mat frame_cam2 = imread(inputCam2Filename, 1);
    vector<Point2f> _image_points_cam2;

    string outputFilename_cam3 = "./data/cam3/voxel.png";
    Mat frame_cam3 = imread(inputCam3Filename, 1);
    vector<Point2f> _image_points_cam3;

    string outputFilename_cam4 = "./data/cam4/voxel.png";
    Mat frame_cam4 = imread(inputCam4Filename, 1);
    vector<Point2f> _image_points_cam4;

    string clusterString = argv[1];
    int clusterId = stoi(clusterString);

    projectPoints(voxels[clusterId], rvecs_cam1, tvecs_cam1, cameraMatrix_cam1, distCoeffs_cam1, _image_points_cam1);
    projectPoints(voxels[clusterId], rvecs_cam2, tvecs_cam2, cameraMatrix_cam2, distCoeffs_cam2, _image_points_cam2);
    projectPoints(voxels[clusterId], rvecs_cam3, tvecs_cam3, cameraMatrix_cam3, distCoeffs_cam3, _image_points_cam3);
    projectPoints(voxels[clusterId], rvecs_cam4, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, _image_points_cam4);

    projectVoxel(frame_cam1, _image_points_cam1, outputFilename_cam4);
    projectVoxel(frame_cam2, _image_points_cam2, outputFilename_cam4);
    projectVoxel(frame_cam3, _image_points_cam3, outputFilename_cam4);
    projectVoxel(frame_cam4, _image_points_cam4, outputFilename_cam4);

}

void projectVoxel(Mat &frame, vector<Point2f> &image_points, const string &outputFilename)
{
    int width = frame.size().width;
    int height = frame.size().height;

    for(int i = 0; i < image_points.size(); i++)
    {
        int x = image_points[i].x;
        int y = image_points[i].y;

        circle(frame, image_points[i], 4, Scalar(255, 0, 0), -1, FILLED);
    }

    string imageWindow = "Image View";

    imwrite(outputFilename, frame);

    namedWindow(imageWindow, 1);
    imshow(imageWindow, frame);
    waitKey(0);
    destroyWindow(imageWindow);
}