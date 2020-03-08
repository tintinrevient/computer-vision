#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct VoxelPoint
{
    float x, y;
    int index;
    int cluster;
    float minDist;

    VoxelPoint():
            x(0.0),
            y(0.0),
            index(-1),
            cluster(-1),
            minDist(numeric_limits<float>::max()) {}

    VoxelPoint(float x, float y, int i):
            x(x),
            y(y),
            index(i),
            cluster(-1),
            minDist(numeric_limits<float>::max()) {}

    float distance(VoxelPoint p) {
        return (p.x - x) * (p.x - x) + (p.y - y) * (p.y - y);
    }
};

void kmeans(vector<VoxelPoint>* points, int epochs, int k, vector<Point3f> &voxel_points);

int main(int argc, char** argv)
{
    string voxelFilename = "./data/voxel.xml";

    FileStorage fs_voxel(voxelFilename, FileStorage::READ);

    vector<Point3f> voxel_points;
    fs_voxel["voxel_points"] >> voxel_points;

    vector<VoxelPoint> points;
    for(int i = 0; i < voxel_points.size(); i++)
    {
        VoxelPoint point = VoxelPoint(voxel_points[i].x, voxel_points[i].y, i);
        points.push_back(point);
    }

    int k = 4;
    int epochs = 20;
    kmeans(&points, epochs, k, voxel_points);
}

void kmeans(vector<VoxelPoint>* points, int epochs, int k, vector<Point3f> &voxel_points)
{
    string clusterFilename = "./data/cluster.xml";
    FileStorage fs_cluster(clusterFilename, FileStorage::WRITE);

    RNG rng = theRNG();

    vector<VoxelPoint> centers;
    int n = points->size();

    // [1] initialization of centers
    for(int i = 0; i < k; i++)
        centers.push_back(points->at(rng.uniform(0, n)));

    for(int epoch = 0; epoch < epochs; epoch++)
    {
        cout << "Epoch " << (epoch+1) << " starts..." << endl;

        // [2] assignment of points to centers
        for (vector<VoxelPoint>::iterator c = begin(centers); c != end(centers); ++c)
        {
            int clusterId = c - begin(centers);

            for (vector<VoxelPoint>::iterator it = points->begin(); it != points->end(); ++it)
            {
                VoxelPoint p = *it;
                float dist = c->distance(p);
                if (dist < p.minDist) {
                    p.minDist = dist;
                    p.cluster = clusterId;
                }
                *it = p;
            }
        }

        // [3] update of centers based on the current clusters
        vector<int> nPoints;
        vector<float> sumX, sumY;

        for (int j = 0; j < k; ++j)
        {
            nPoints.push_back(0);
            sumX.push_back(0.0);
            sumY.push_back(0.0);
        }

        for (vector<VoxelPoint>::iterator it = points->begin(); it != points->end(); ++it)
        {
            int clusterId = it->cluster;
            nPoints[clusterId] += 1;
            sumX[clusterId] += it->x;
            sumY[clusterId] += it->y;

            it->minDist = numeric_limits<float>::max();  // reset distance
        }

        int flag = 0;
        for (vector<VoxelPoint>::iterator c = begin(centers); c != end(centers); ++c)
        {
            int clusterId = c - begin(centers);

            float x = sumX[clusterId] / nPoints[clusterId];
            float y = sumY[clusterId] / nPoints[clusterId];

            if (abs(x - c->x) < 10e-1 && abs(y - c->y) < 10e-1)
                flag++;

            c->x = x;
            c->y = y;

            cout << "center (x, y) of cluster " << clusterId << ": (" << x << ", " << y << ")" << endl;
        }

        // no changes of centers -> jump out of the epoch loop
        if(flag == k)
            break;
    }

    // write centers to cluster.xml
    for (vector<VoxelPoint>::iterator c = begin(centers); c != end(centers); ++c)
    {
        int clusterId = c - begin(centers);
        string centerString = "cluster_" + to_string(clusterId) + "_center";

        vector<float> pos;
        pos.push_back(c->x);
        pos.push_back(c->y);

        fs_cluster << centerString.c_str() << pos;
    }

    // write voxels to cluster.xml
    vector<vector<Point3f>> clustered_voxels;

    for (int j = 0; j < k; ++j)
        clustered_voxels.push_back({});

    for (vector<VoxelPoint>::iterator it = points->begin(); it != points->end(); ++it)
    {
        int clusterId = it->cluster;
        clustered_voxels[clusterId].push_back(voxel_points[it->index]);
    }

    for(int c = 0; c < clustered_voxels.size(); c++)
    {
        string clusterString = "cluster_" + to_string(c);
        fs_cluster << clusterString.c_str() << clustered_voxels[c];
    }

    fs_cluster.release();
}