#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int temp[5000];
bool voxelMapping(Mat &frame, Point2f &image_point);
void voxelProjection(Mat &frame, vector<Point2f> &image_points, const string &outputFilename);

int main(int argc, char** argv)
{
    string inputCam1Filename = "./data/cam1/foreground.png";
    string inputCam2Filename = "./data/cam2/foreground.png";
    string inputCam3Filename = "./data/cam3/foreground.png";
    string inputCam4Filename = "./data/cam4/foreground.png";
    string camConfigFilename = "./data/config.xml";

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
//    vector<Point2f> _image_points_cam4;
//    string outputFilename = "./data/cam4/voxel.png";
//    projectPoints(voxels, rvecs_cam3, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, _image_points_cam4);
//    voxelProjection(frame_cam4, _image_points_cam4, outputFilename);

    for(int i = 0; i < voxels.size(); i++)
    {
        Point3f voxel_point = voxels[i];
        vector<Point3f> voxel_points;
        voxel_points.push_back(voxel_point);

        cout << "voxel: " << voxel_point << endl;

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
        bool is_cam1_mapped = voxelMapping(frame_cam1, image_points_cam1[0]);

        projectPoints(voxel_points, rvecs_cam2, tvecs_cam2, cameraMatrix_cam2, distCoeffs_cam2, image_points_cam2);
        bool is_cam2_mapped = voxelMapping(frame_cam2, image_points_cam2[0]);

        projectPoints(voxel_points, rvecs_cam3, tvecs_cam3, cameraMatrix_cam3, distCoeffs_cam3, image_points_cam3);
        bool is_cam3_mapped = voxelMapping(frame_cam3, image_points_cam3[0]);

        projectPoints(voxel_points, rvecs_cam4, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, image_points_cam4);
        bool is_cam4_mapped = voxelMapping(frame_cam4, image_points_cam4[0]);

            if(is_cam1_mapped && is_cam2_mapped && is_cam3_mapped && is_cam4_mapped)
            valid_voxels_index.push_back(i);
    }

    cout << "valid voxels: " << valid_voxels_index.size() << endl;

}

void voxelProjection(Mat &frame, vector<Point2f> &image_points, const string &outputFilename)
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

bool voxelMapping(Mat &frame, Point2f &image_point)
{
    cout << "image point: " << image_point << endl;

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