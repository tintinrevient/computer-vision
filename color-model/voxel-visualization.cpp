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

int main(int argc, char** argv) {

    string voxelFilename = "./data/voxel/voxel";
    string clusterFilename = "./data/cluster/colored";

    //PCL viewr initialization
    visualization::PCLVisualizer::Ptr viewer(new visualization::PCLVisualizer("3D Viewer"));
    viewer->setBackgroundColor(0, 0, 0);

    for (int frame = 1500; frame < 2500; frame++) {
        //read the clustered voxels from one frame
        string filename = clusterFilename + to_string(frame) + ".xml";
        FileStorage fs_cluster(filename, FileStorage::READ);

        if (!fs_cluster.isOpened())
            continue;

        vector<Point3f> cluster_1;
        fs_cluster["cluster_0"] >> cluster_1;
        vector<Point3f> cluster_2;
        fs_cluster["cluster_1"] >> cluster_2;
        vector<Point3f> cluster_3;
        fs_cluster["cluster_2"] >> cluster_3;
        vector<Point3f> cluster_4;
        fs_cluster["cluster_3"] >> cluster_4;

        Point2f center_1;
        fs_cluster["cluster_0_center"] >> center_1;
        Point2f center_2;
        fs_cluster["cluster_1_center"] >> center_2;
        Point2f center_3;
        fs_cluster["cluster_2_center"] >> center_3;
        Point2f center_4;
        fs_cluster["cluster_3_center"] >> center_4;

        PointCloud<pcl::PointXYZ>::Ptr cloud_ptr_1(new PointCloud<PointXYZ>);
        visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud1(cloud_ptr_1, 0, 0, 255);
        pcl::PointXYZ center_point_1;
        center_point_1.x = center_1.x;
        center_point_1.y = center_1.y;
        center_point_1.z = 0.0;
        cloud_ptr_1->points.push_back(center_point_1);
        for(int i = 0; i < cluster_1.size(); i++) {
            Point3f voxel_point = cluster_1[i];

            pcl::PointXYZ basic_point;
            basic_point.x = voxel_point.x;
            basic_point.y = voxel_point.y;
            basic_point.z = voxel_point.z;

            cloud_ptr_1->points.push_back(basic_point);
        }
        cloud_ptr_1->width = (int) cloud_ptr_1->points.size();
        cloud_ptr_1->height = 1;

        PointCloud<pcl::PointXYZ>::Ptr cloud_ptr_2(new PointCloud<PointXYZ>);
        visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud2(cloud_ptr_2, 255, 0, 0);
        pcl::PointXYZ center_point_2;
        center_point_2.x = center_2.x;
        center_point_2.y = center_2.y;
        center_point_2.z = 0.0;
        cloud_ptr_2->points.push_back(center_point_2);
        for(int i = 0; i < cluster_2.size(); i++) {
            Point3f voxel_point = cluster_2[i];

            pcl::PointXYZ basic_point;
            basic_point.x = voxel_point.x;
            basic_point.y = voxel_point.y;
            basic_point.z = voxel_point.z;

            cloud_ptr_2->points.push_back(basic_point);
        }
        cloud_ptr_2->width = (int) cloud_ptr_2->points.size();
        cloud_ptr_2->height = 1;

        PointCloud<pcl::PointXYZ>::Ptr cloud_ptr_3(new PointCloud<PointXYZ>);
        visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud3(cloud_ptr_3, 0, 255, 0);
        pcl::PointXYZ center_point_3;
        center_point_3.x = center_3.x;
        center_point_3.y = center_3.y;
        center_point_3.z = 0.0;
        cloud_ptr_3->points.push_back(center_point_3);
        for(int i = 0; i < cluster_3.size(); i++) {
            Point3f voxel_point = cluster_3[i];

            pcl::PointXYZ basic_point;
            basic_point.x = voxel_point.x;
            basic_point.y = voxel_point.y;
            basic_point.z = voxel_point.z;

            cloud_ptr_3->points.push_back(basic_point);
        }
        cloud_ptr_3->width = (int) cloud_ptr_3->points.size();
        cloud_ptr_3->height = 1;

        PointCloud<pcl::PointXYZ>::Ptr cloud_ptr_4(new PointCloud<PointXYZ>);
        visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> cloud4(cloud_ptr_4, 0, 255, 255);
        pcl::PointXYZ center_point_4;
        center_point_4.x = center_4.x;
        center_point_4.y = center_4.y;
        center_point_4.z = 0.0;
        cloud_ptr_4->points.push_back(center_point_4);
        for(int i = 0; i < cluster_4.size(); i++) {
            Point3f voxel_point = cluster_4[i];

            pcl::PointXYZ basic_point;
            basic_point.x = voxel_point.x;
            basic_point.y = voxel_point.y;
            basic_point.z = voxel_point.z;

            cloud_ptr_4->points.push_back(basic_point);
        }
        cloud_ptr_4->width = (int) cloud_ptr_4->points.size();
        cloud_ptr_4->height = 1;

        string cloudName = "voxel cloud " + to_string(frame) + " ";
        string cluster_1_cloud_name = cloudName + to_string(1);
        string cluster_2_cloud_name = cloudName + to_string(2);
        string cluster_3_cloud_name = cloudName + to_string(3);
        string cluster_4_cloud_name = cloudName + to_string(4);

        viewer->addPointCloud<pcl::PointXYZ>(cloud_ptr_1, cloud1, cluster_1_cloud_name);
        viewer->addPointCloud<pcl::PointXYZ>(cloud_ptr_2, cloud2, cluster_2_cloud_name);
        viewer->addPointCloud<pcl::PointXYZ>(cloud_ptr_3, cloud3, cluster_3_cloud_name);
        viewer->addPointCloud<pcl::PointXYZ>(cloud_ptr_4, cloud4, cluster_4_cloud_name);
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cluster_1_cloud_name);
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cluster_2_cloud_name);
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cluster_3_cloud_name);
        viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 5, cluster_4_cloud_name);

        viewer->initCameraParameters();

        //set the focal of camera to look at one center of the clusters
        vector<pcl::visualization::Camera> cameras;
        viewer->getCameras(cameras);
        for (auto&& camera : cameras)
        {
            camera.focal[0] = 500;
            camera.focal[1] = 1936;
            camera.focal[2] = 3000;
        }
        viewer->setCameraParameters(cameras[0]);

        viewer->setCameraPosition(700, 5000, 1500, -500, 0, 0, 0, 0, 1);
        viewer->setCameraFieldOfView(0.8);
        viewer->setCameraClipDistances(100, 700);

        viewer->spinOnce();
        // comment below four lines of code if it is needed to NOT redraw for the next frame
        viewer->removePointCloud(cluster_1_cloud_name);
        viewer->removePointCloud(cluster_2_cloud_name);
        viewer->removePointCloud(cluster_3_cloud_name);
        viewer->removePointCloud(cluster_4_cloud_name);
    }
}