#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    string voxelFilename = "./data/voxel/voxel";
    string clusterFilename = "./data/cluster/cluster";

    int clusterCount = 4;

    for (int frame = 1500; frame < 1945; frame++) {

        string voxelframeFilename = voxelFilename + to_string(frame) + ".xml";
        FileStorage fs_voxel(voxelframeFilename, FileStorage::READ);
        vector<Point3f> voxel_points;
        fs_voxel["voxel_points"] >> voxel_points;

        int i, sampleCount = voxel_points.size();

        if(sampleCount < clusterCount)
            continue;

        Mat points(sampleCount, 1, CV_32FC2), labels;
        vector<Point2f> centers;

        for(i = 0; i < sampleCount; i++) {
            float x = voxel_points[i].x;
            float y = voxel_points[i].y;

            points.at<Point2f>(i, 0) = Point2f(x, y);
        }

        double compactness = kmeans(points, clusterCount, labels,
                                    TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 10, 1.0),
                                    3, KMEANS_PP_CENTERS, centers);

        cout << "Compactness of frame " << frame << ": " << compactness << endl;

        string clusterframeFilename = clusterFilename + to_string(frame) + ".xml";
        FileStorage fs_cluster(clusterframeFilename, FileStorage::WRITE);

        for (i = 0; i < centers.size(); i++) {
            Point2f center = centers[i];
            cout << "center " << i << " of frame " << frame << ": " << center << endl;

            string centerString = "cluster_" + to_string(i) + "_center";
            fs_cluster << centerString.c_str() << center;
        }

        vector<vector<Point3f>> clustered_voxels;
        for (i = 0; i < centers.size(); i++)
            clustered_voxels.push_back({});

        for(i = 0; i < sampleCount; i++) {
            int clusterId = labels.at<int>(i);
            Point2f point = points.at<Point2f>(i);

            clustered_voxels[clusterId].push_back(voxel_points[i]);
        }

        for (i = 0; i < centers.size(); i++) {
            string clusterString = "cluster_" + to_string(i);
            fs_cluster << clusterString.c_str() << clustered_voxels[i];
        }

        fs_cluster.release();
    }

    return 0;
}