#include <opencv2/opencv.hpp>
#include <iostream>

#include <thread>

#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>

using namespace cv;
using namespace std;
using namespace pcl;
using namespace std::chrono_literals;

bool mapVoxel(Mat &frame, Point2f &image_point);
void projectVoxel(Mat &frame, vector<Point2f> &image_points, const string &outputFilename);
visualization::PCLVisualizer::Ptr drawVoxel(PointCloud<pcl::PointXYZ>::ConstPtr cloud);

int main(int argc, char** argv) {
    string camConfigFilename = "./data/config.xml";
    string inputCam1Filename = "./data/cam1/foreground";
    string inputCam2Filename = "./data/cam2/foreground";
    string inputCam3Filename = "./data/cam3/foreground";
    string inputCam4Filename = "./data/cam4/foreground";
    string voxelFilename = "./data/voxel";

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

    int m_height = 2048;
    int edge = 2 * m_height;
    int m_step = 32;
    int m_voxels_amount = (edge / m_step) * (edge / m_step) * (m_height / m_step);

    vector<Point3f> voxels;

    for(int i = -m_height; i < m_height; i = i + m_step) {
        for(int j = -m_height; j < m_height; j = j + m_step) {
            for(int k = 0; k < m_height; k = k + m_step) {
                voxels.push_back(Point3f(float(i), float(j), float(k)));
            }
        }
    }

    // hook
    int hook = 450;
    for(int i = hook; i < 2725; i++) {
        cout << "frame " << i << " starts...";

        string filename1 = inputCam1Filename + to_string(i) + ".png";
        string filename2 = inputCam2Filename + to_string(i) + ".png";
        string filename3 = inputCam3Filename + to_string(i) + ".png";
        string filename4 = inputCam4Filename + to_string(i) + ".png";

        Mat frame_cam1 = imread(filename1, 0);
        Mat frame_cam2 = imread(filename2, 0);
        Mat frame_cam3 = imread(filename3, 0);
        Mat frame_cam4 = imread(filename4, 0);

        vector<int> valid_voxels_index;

        for(int i = 0; i < voxels.size(); i++) {
            Point3f voxel_point = voxels[i];
            vector<Point3f> voxel_points;
            voxel_points.push_back(voxel_point);

            Point2f image_point_cam1;
            vector<Point2f> image_points_cam1;
            image_points_cam1.push_back(image_point_cam1);

            Point2f image_point_cam2;
            vector<Point2f> image_points_cam2;
            image_points_cam2.push_back(image_point_cam2);

            Point2f image_point_cam3;
            vector<Point2f> image_points_cam3;
            image_points_cam3.push_back(image_point_cam3);

            Point2f image_point_cam4;
            vector<Point2f> image_points_cam4;
            image_points_cam4.push_back(image_point_cam4);

            projectPoints(voxel_points, rvecs_cam1, tvecs_cam1, cameraMatrix_cam1, distCoeffs_cam1, image_points_cam1);
            bool is_cam1_mapped = mapVoxel(frame_cam1, image_points_cam1.front());

            projectPoints(voxel_points, rvecs_cam2, tvecs_cam2, cameraMatrix_cam2, distCoeffs_cam2, image_points_cam2);
            bool is_cam2_mapped = mapVoxel(frame_cam2, image_points_cam2.front());

            projectPoints(voxel_points, rvecs_cam3, tvecs_cam3, cameraMatrix_cam3, distCoeffs_cam3, image_points_cam3);
            bool is_cam3_mapped = mapVoxel(frame_cam3, image_points_cam3.front());

            projectPoints(voxel_points, rvecs_cam4, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, image_points_cam4);
            bool is_cam4_mapped = mapVoxel(frame_cam4, image_points_cam4.front());

            if(is_cam1_mapped && is_cam2_mapped && is_cam3_mapped && is_cam4_mapped)
                valid_voxels_index.push_back(i);

        }

        cout << "valid voxels from all cams: " << valid_voxels_index.size() << "...";

        vector<Point3f> voxel_points;
        for(int i = 0; i < valid_voxels_index.size(); i++) {
            Point3f voxel_point = voxels[valid_voxels_index[i]];
            voxel_points.push_back(voxel_point);
        }

        string voxelframefilename = voxelFilename + to_string(i) + ".xml";
        FileStorage fs_voxel(voxelframefilename, FileStorage::WRITE);

        fs_voxel << "voxel_points" << voxel_points;
        fs_voxel.release();

        cout << "frame " << i << " ends..." << endl;
    }
}

visualization::PCLVisualizer::Ptr drawVoxel(PointCloud<pcl::PointXYZ>::ConstPtr cloud) {
    visualization::PCLVisualizer::Ptr viewer(new visualization::PCLVisualizer("3D Viewer"));
    viewer->setBackgroundColor(0, 0, 0);
    viewer->addPointCloud<pcl::PointXYZ>(cloud, "voxel cloud");
    viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "voxel cloud");
    viewer->addCoordinateSystem(1.0);
    viewer->initCameraParameters();
    return (viewer);
}

void projectVoxel(Mat &frame, vector<Point2f> &image_points, const string &outputFilename) {
    int width = frame.size().width;
    int height = frame.size().height;

    for(int i = 0; i < image_points.size(); i++) {
        int x = image_points[i].x;
        int y = image_points[i].y;

        if(x >= 0 && x < width && y >= 0 && y < height) {
            int binary_value = int(frame.at<uchar>(image_points[i]));

            if(binary_value == 255)
                circle(frame, image_points[i], 4, Scalar(255, 255, 0), -1, FILLED);
        }
    }

    string imageWindow = "Image View";

    imwrite(outputFilename, frame);

    namedWindow(imageWindow, 1);
    imshow(imageWindow, frame);
    waitKey(0);
    destroyWindow(imageWindow);
}

bool mapVoxel(Mat &frame, Point2f &image_point) {
    int width = frame.size().width;
    int height = frame.size().height;

    int x = image_point.x;
    int y = image_point.y;

    if(x >= 0 && x < width && y >= 0 && y < height) {
        int binary_value = int(frame.at<uchar>(image_point));

        if(binary_value == 255)
            return true;
    }

    return false;
}