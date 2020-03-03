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

int main(int argc, char** argv)
{
    string inputCam1Filename = "./data/cam1/foreground.png";
    string inputCam2Filename = "./data/cam2/foreground.png";
    string inputCam3Filename = "./data/cam3/foreground.png";
    string inputCam4Filename = "./data/cam4/foreground.png";
    string camConfigFilename = "./data/config.xml";
    string voxelFilename = "./data/voxel.xml";

    FileStorage fs_config(camConfigFilename, FileStorage::READ);
    FileStorage fs_voxel(voxelFilename, FileStorage::WRITE);

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

    for(int i = -m_height; i < m_height; i = i + m_step)
    {
        for(int j = -m_height; j < m_height; j = j + m_step)
        {
            for(int k = 0; k < m_height; k = k + m_step)
            {
                voxels.push_back(Point3f(float(i), float(j), float(k)));
            }
        }
    }

    Mat frame_cam1 = imread(inputCam1Filename, 0);
    Mat frame_cam2 = imread(inputCam2Filename, 0);
    Mat frame_cam3 = imread(inputCam3Filename, 0);
    Mat frame_cam4 = imread(inputCam4Filename, 0);

    string imageWindow = "Image View";

    vector<int> valid_voxels_index;

    // debug
    vector<int> valid_voxels_cam1_index;
    vector<int> valid_voxels_cam2_index;
    vector<int> valid_voxels_cam3_index;
    vector<int> valid_voxels_cam4_index;

    // debug
//    vector<Point2f> _image_points_cam4;
//    string outputFilename = "./data/cam4/voxel.png";
//    projectPoints(voxels, rvecs_cam3, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, _image_points_cam4);
//    projectVoxel(frame_cam4, _image_points_cam4, outputFilename);

    for(int i = 0; i < voxels.size(); i++)
    {
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

        // debug
        if(is_cam1_mapped)
            valid_voxels_cam1_index.push_back(i);
        if(is_cam2_mapped)
            valid_voxels_cam2_index.push_back(i);
        if(is_cam3_mapped)
            valid_voxels_cam3_index.push_back(i);
        if(is_cam4_mapped)
            valid_voxels_cam4_index.push_back(i);
    }

    // debug
    cout << "valid voxels from cam1: " << valid_voxels_cam1_index.size() << endl;
    cout << "valid voxels from cam2: " << valid_voxels_cam2_index.size() << endl;
    cout << "valid voxels from cam3: " << valid_voxels_cam3_index.size() << endl;
    cout << "valid voxels from cam4: " << valid_voxels_cam4_index.size() << endl;

    // debug
//    vector<int> intersection_cam1_cam2;
//    set_intersection(valid_voxels_cam1_index.begin(), valid_voxels_cam1_index.end(),
//                     valid_voxels_cam2_index.begin(), valid_voxels_cam2_index.end(),
//                     back_inserter(intersection_cam1_cam2));
//    cout << "intersection of cam1 and cam2: " << intersection_cam1_cam2.size() << endl;

    cout << "valid voxels from all cams: " << valid_voxels_index.size() << endl;

    PointCloud<pcl::PointXYZ>::Ptr basic_cloud_ptr(new PointCloud<PointXYZ>);

    vector<Point3f> voxel_points;

    for(int i = 0; i < valid_voxels_index.size(); i++)
    {
        Point3f voxel_point = voxels[valid_voxels_index[i]];
        voxel_points.push_back(voxel_point);
        pcl::PointXYZ basic_point;

        basic_point.x = voxel_point.x;
        basic_point.y = voxel_point.y;
        basic_point.z = voxel_point.z;

        basic_cloud_ptr->points.push_back(basic_point);
    }

    fs_voxel << "voxel_points" << voxel_points;
    fs_voxel.release();

    basic_cloud_ptr->width = (int) basic_cloud_ptr->points.size();
    basic_cloud_ptr->height = 1;

    visualization::PCLVisualizer::Ptr viewer = drawVoxel(basic_cloud_ptr);

    while (!viewer->wasStopped())
    {
        viewer->spinOnce(100);
        std::this_thread::sleep_for(100ms);
    }
}

visualization::PCLVisualizer::Ptr drawVoxel(PointCloud<pcl::PointXYZ>::ConstPtr cloud)
{
    visualization::PCLVisualizer::Ptr viewer(new visualization::PCLVisualizer("3D Viewer"));
    viewer->setBackgroundColor(0, 0, 0);
    viewer->addPointCloud<pcl::PointXYZ>(cloud, "voxel cloud");
    viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "voxel cloud");
    viewer->addCoordinateSystem(1.0);
    viewer->initCameraParameters();
    return (viewer);
}

void projectVoxel(Mat &frame, vector<Point2f> &image_points, const string &outputFilename)
{
    int width = frame.size().width;
    int height = frame.size().height;

    for(int i = 0; i < image_points.size(); i++)
    {
        int x = image_points[i].x;
        int y = image_points[i].y;

        if(x >= 0 && x < width && y >= 0 && y < height)
        {
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

bool mapVoxel(Mat &frame, Point2f &image_point)
{
    int width = frame.size().width;
    int height = frame.size().height;

    int x = image_point.x;
    int y = image_point.y;

    if(x >= 0 && x < width && y >= 0 && y < height)
    {
        int binary_value = int(frame.at<uchar>(image_point));

        if(binary_value == 255)
            return true;
    }

    return false;
}