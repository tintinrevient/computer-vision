#include <opencv2/opencv.hpp>
#include "opencv2/ml.hpp"
#include <iostream>

using namespace cv;
using namespace cv::ml;
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

void loadConfig(const string &camConfigFilename);
void projectVoxel(Mat &frame, vector<Point2f> &image_points, const string &outputFilename);

vector<Ptr<EM>> train(const vector<string> &inputCamFilenames, const string &clusterFilename);
Ptr<EM> _train(vector<Mat> &frames, vector<vector<Point2f>> &image_points);
int predict(vector<Point3f> cluster, Mat &frame_cam1, Mat &frame_cam2, Mat &frame_cam3, Mat &frame_cam4, vector<Ptr<EM> > &em_models);

int main(int argc, char** argv) {
    string camConfigFilename = "./data/config.xml";
    loadConfig(camConfigFilename);

    int frameId = 1895;
    vector<string> inputCamFilenames(4);
    for(int i = 0; i < 4; i++) {
        string filename = "./data/cam" + to_string(i+1) + "/frame" + to_string(frameId) + ".png";
        inputCamFilenames[i] = filename;
    }

    string clusterFilename = "./data/cluster/cluster" + to_string(frameId) + ".xml";

    vector<Ptr<EM>> em_models = train(inputCamFilenames, clusterFilename);

    // color each cluster
    string uncoloredClusterFilename = "./data/cluster/cluster";
    string coloredClusterFilename = "./data/cluster/colored";

    for (int frame = 1500; frame < 2500; frame++) {

        cout << "frame " << frame << " starts..." << endl;

        string sourceFilename = uncoloredClusterFilename + to_string(frame) + ".xml";
        string targetFilename = coloredClusterFilename + to_string(frame) + ".xml";

        string frameFilename_cam1 = "./data/cam1/frame" + to_string(frame) + ".png";
        string frameFilename_cam2 = "./data/cam2/frame" + to_string(frame) + ".png";
        string frameFilename_cam3 = "./data/cam3/frame" + to_string(frame) + ".png";
        string frameFilename_cam4 = "./data/cam4/frame" + to_string(frame) + ".png";

        FileStorage fs_source(sourceFilename, FileStorage::READ);

        Mat frame_target_cam1 = imread(frameFilename_cam1, 1);
        Mat frame_target_cam2 = imread(frameFilename_cam2, 1);
        Mat frame_target_cam3 = imread(frameFilename_cam3, 1);
        Mat frame_target_cam4 = imread(frameFilename_cam4, 1);

        vector<vector<Point3f>> clusters(4);
        fs_source["cluster_0"] >> clusters[0];
        fs_source["cluster_1"] >> clusters[1];
        fs_source["cluster_2"] >> clusters[2];
        fs_source["cluster_3"] >> clusters[3];

        vector<Point2f> centers(4);
        fs_source["cluster_0_center"] >> centers[0];
        fs_source["cluster_1_center"] >> centers[1];
        fs_source["cluster_2_center"] >> centers[2];
        fs_source["cluster_3_center"] >> centers[3];


        // if there are duplicate clusterId predictions, just ignore this frame
        vector<int> flag;
        for(int clusterIndex = 0; clusterIndex < clusters.size(); clusterIndex++) {

            cout << "source cluster ID is: " << clusterIndex << endl;

            int clusterId = predict(clusters[clusterIndex], frame_target_cam1, frame_target_cam2, frame_target_cam3, frame_target_cam4, em_models);

//            vector<int>::iterator it = find(flag.begin(), flag.end(), clusterId);
//            if (it != flag.end())
//                break;
//            else
//                flag.push_back(clusterId);
        }

//        if(flag.size() == 4) {
//
//            FileStorage fs_target(targetFilename, FileStorage::WRITE);
//
//            for(int clusterIndex = 0; clusterIndex < clusters.size(); clusterIndex++) {
//                int clusterId = predict(clusters[clusterIndex], frame_target_cam1, frame_target_cam2, frame_target_cam3, frame_target_cam4, em_models);
//                cout << "cluster " << clusterIndex << " belongs to " << "cluster " << clusterId << endl;
//
//                // write the colored center
//                string centerString = "cluster_" + to_string(clusterId) + "_center";
//                fs_target << centerString.c_str() << centers[clusterIndex];
//
//                // write the colored cluster
//                string clusterString = "cluster_" + to_string(clusterId);
//                fs_target << clusterString.c_str() << clusters[clusterIndex];
//            }
//
//            fs_source.release();
//            fs_target.release();
//        }
    }
}

void loadConfig(const string &camConfigFilename) {

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
}

void projectVoxel(Mat &frame, vector<Point2f> &image_points, const string &outputFilename) {
    int width = frame.size().width;
    int height = frame.size().height;

    for(int i = 0; i < image_points.size(); i++) {
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

vector<Ptr<EM>> train(const vector<string> &inputCamFilenames, const string &clusterFilename){

    FileStorage fs_cluster(clusterFilename, FileStorage::READ);
    vector<vector<Point3f>> clusters(4);
    fs_cluster["cluster_0"] >> clusters[0];
    fs_cluster["cluster_1"] >> clusters[1];
    fs_cluster["cluster_2"] >> clusters[2];
    fs_cluster["cluster_3"] >> clusters[3];
    fs_cluster.release();

    vector<Ptr<EM>> em_models;

    int nclusters = 4;
    for(int clusterId = 0; clusterId < nclusters; clusterId++) {

        vector<Mat> frame_from_cams(4);
        frame_from_cams[0] = imread(inputCamFilenames[0], 1);
        frame_from_cams[1] = imread(inputCamFilenames[1], 1);
        frame_from_cams[2] = imread(inputCamFilenames[2], 1);
        frame_from_cams[3] = imread(inputCamFilenames[3], 1);

//        vector<Mat> frame_hsv_from_cams(4);
//        cvtColor(frame_from_cams[0], frame_hsv_from_cams[0], COLOR_BGR2HSV);
//        cvtColor(frame_from_cams[1], frame_hsv_from_cams[1], COLOR_BGR2HSV);
//        cvtColor(frame_from_cams[2], frame_hsv_from_cams[2], COLOR_BGR2HSV);
//        cvtColor(frame_from_cams[3], frame_hsv_from_cams[3], COLOR_BGR2HSV);

        vector<vector<Point2f>> image_points_from_cams(4);
        projectPoints(clusters[clusterId], rvecs_cam1, tvecs_cam1, cameraMatrix_cam1, distCoeffs_cam1, image_points_from_cams[0]);
        projectPoints(clusters[clusterId], rvecs_cam2, tvecs_cam2, cameraMatrix_cam2, distCoeffs_cam2, image_points_from_cams[1]);
        projectPoints(clusters[clusterId], rvecs_cam3, tvecs_cam3, cameraMatrix_cam3, distCoeffs_cam3, image_points_from_cams[2]);
        projectPoints(clusters[clusterId], rvecs_cam4, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, image_points_from_cams[3]);

        // for cluster 1: use only the frame from cam1 and cam3
        if(clusterId == 0) {
            cout << "use cam1 and cam3 for cluster 1 training..." << endl;

            vector<Mat> frames_cam1;
            frames_cam1.push_back(frame_from_cams[0]);
            frames_cam1.push_back(frame_from_cams[2]);

            vector<vector<Point2f>> image_points_cam1;
            image_points_cam1.push_back(image_points_from_cams[0]);
            image_points_cam1.push_back(image_points_from_cams[2]);

            Ptr<EM> em_model = _train(frames_cam1, image_points_cam1);
            em_models.push_back(em_model);
        }

        // for cluster 2: use only the frames from cam1, cam2 and cam4
        if(clusterId == 1) {
            cout << "use cam1, cam2 and cam4 for cluster 2 training..." << endl;

            vector<Mat> frames_cam2;
            frames_cam2.push_back(frame_from_cams[0]);
            frames_cam2.push_back(frame_from_cams[1]);
            frames_cam2.push_back(frame_from_cams[3]);

            vector<vector<Point2f>> image_points_cam2;
            image_points_cam2.push_back(image_points_from_cams[0]);
            image_points_cam2.push_back(image_points_from_cams[1]);
            image_points_cam2.push_back(image_points_from_cams[3]);

            Ptr<EM> em_model = _train(frames_cam2, image_points_cam2);
            em_models.push_back(em_model);
        }

        // for cluster 3: use only the frames from cam3 and cam4
        if(clusterId == 2) {
            cout << "use cam3 and cam4 for cluster 3 training..." << endl;

            vector<Mat> frames_cam3;
            frames_cam3.push_back(frame_from_cams[2]);
            frames_cam3.push_back(frame_from_cams[3]);

            vector<vector<Point2f>> image_points_cam3;
            image_points_cam3.push_back(image_points_from_cams[2]);
            image_points_cam3.push_back(image_points_from_cams[3]);

            Ptr<EM> em_model = _train(frames_cam3, image_points_cam3);
            em_models.push_back(em_model);
        }

        // for cluster 4: use only the frames from cam1, cam2 and cam4
        if(clusterId == 3) {
            cout << "use cam1, cam2 and cam4 for cluster 4 training..." << endl;

            vector<Mat> frames_cam4;
            frames_cam4.push_back(frame_from_cams[0]);
            frames_cam4.push_back(frame_from_cams[1]);
            frames_cam4.push_back(frame_from_cams[3]);

            vector<vector<Point2f>> image_points_cam4;
            image_points_cam4.push_back(image_points_from_cams[0]);
            image_points_cam4.push_back(image_points_from_cams[1]);
            image_points_cam4.push_back(image_points_from_cams[3]);

            Ptr<EM> em_model = _train(frames_cam4, image_points_cam4);
            em_models.push_back(em_model);
        }
    }

    return em_models;
}

// EM model for 1 cluster
// 1 cluster of voxels is projected onto 4 cameras
// 4 frames = frames from 4 cameras for this 1 cluster
// 4 image points = image points on 4 frames for this 1 cluster
Ptr<EM> _train(vector<Mat> &frames, vector<vector<Point2f>> &image_points) {
    int num_cluster = 1;
    printf("num of num_cluster is %d\n", num_cluster);

    int size = image_points[0].size() * frames.size();
    Mat points(size, 3, CV_64FC1);
//    Mat points(size, 2, CV_64FC1);
    Mat labels;

    for(int frameIndex = 0; frameIndex < frames.size(); frameIndex++) {
        Mat frame = frames[frameIndex];
        vector<Point2f> image_point = image_points[frameIndex];

        // prepare the samples
        int nsamples = image_point.size();

        cout << "number of samples for " << frameIndex << ": " << nsamples << endl;

        int width = frame.cols;
        int height = frame.rows;
        int dims = frame.channels();

        int index = 0;
        for(int imgPointindex = 0; imgPointindex < nsamples; imgPointindex++) {
            index = frameIndex * nsamples + imgPointindex;

            int x = image_point[imgPointindex].x;
            int y = image_point[imgPointindex].y;

//            cout << "position: (" << x << ", " << y << ")" << endl;

            Vec3b bgr = frame.at<Vec3b>(x, y);
            points.at<double>(index, 0) = static_cast<int>(bgr[0]);
            points.at<double>(index, 1) = static_cast<int>(bgr[1]);
            points.at<double>(index, 2) = static_cast<int>(bgr[2]);

//            cout << "rgb: (" << rgb[0] << ", " << rgb[1] << ", " << rgb[2] << ")" << endl;
        }
    }

    cout << "size of points: " << points.rows << endl;

    // EM Cluster Train
    Ptr<EM> em_model = EM::create();
    //Partition number
    em_model->setClustersNumber(num_cluster);
    //Set covariance matrix type
    em_model->setCovarianceMatrixType(EM::COV_MAT_SPHERICAL);
    //Set convergence conditions
    em_model->setTermCriteria(TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 100, 0.1));
    //Store the probability partition to labs EM according to the sample training
    em_model->trainEM(points, noArray(), labels, noArray());

    return em_model;
}

int predict(vector<Point3f> cluster, Mat &frame_cam1, Mat &frame_cam2, Mat &frame_cam3, Mat &frame_cam4, vector<Ptr<EM> > &em_models) {

    vector<Point2f> image_points_cam1;
    vector<Point2f> image_points_cam2;
    vector<Point2f> image_points_cam3;
    vector<Point2f> image_points_cam4;

    projectPoints(cluster, rvecs_cam1, tvecs_cam1, cameraMatrix_cam1, distCoeffs_cam1, image_points_cam1);
    projectPoints(cluster, rvecs_cam2, tvecs_cam2, cameraMatrix_cam2, distCoeffs_cam2, image_points_cam2);
    projectPoints(cluster, rvecs_cam3, tvecs_cam3, cameraMatrix_cam3, distCoeffs_cam3, image_points_cam3);
    projectPoints(cluster, rvecs_cam4, tvecs_cam4, cameraMatrix_cam4, distCoeffs_cam4, image_points_cam4);

    // the average likelihood prediction from cam1
    vector<int> voting_cam1(4, 0);
    for (int i = 0; i < image_points_cam1.size(); i++) {

        Mat sample_cam1(1, 3, CV_64FC1);
        int b_cam1 = 0, g_cam1 = 0, r_cam1 = 0;
        int row_cam1 = image_points_cam1[i].x;
        int col_cam1 = image_points_cam1[i].y;

        //Get the color of each channel
//        Mat frame_hsv_cam1;
//        cvtColor(frame_cam1, frame_hsv_cam1, COLOR_BGR2HSV);
        b_cam1 = frame_cam1.at<Vec3b>(row_cam1, col_cam1)[0];
        g_cam1 = frame_cam1.at<Vec3b>(row_cam1, col_cam1)[1];
        r_cam1 = frame_cam1.at<Vec3b>(row_cam1, col_cam1)[2];

        //Put pixels in sample data
        sample_cam1.at<double>(0, 0) = static_cast<double>(b_cam1);
        sample_cam1.at<double>(0, 1) = static_cast<double>(g_cam1);
        sample_cam1.at<double>(0, 2) = static_cast<double>(r_cam1);

        // rounding
        vector<double> likelihoods(4);
        likelihoods[0] = (em_models[0]->predict2(sample_cam1, noArray()))[0];
        likelihoods[1] = (em_models[1]->predict2(sample_cam1, noArray()))[0];
        likelihoods[2] = (em_models[2]->predict2(sample_cam1, noArray()))[0];
        likelihoods[3] = (em_models[3]->predict2(sample_cam1, noArray()))[0];

        double max_likelihood = -10000;
        int clusterId = -1;
        for (int i = 0; i < 4; i++) {
            if(likelihoods[i] > max_likelihood) {
                max_likelihood = likelihoods[i];
                clusterId = i;
            }
        }

        if(clusterId == 0)
            voting_cam1[0]++;
        if(clusterId == 1)
            voting_cam1[1]++;
        if(clusterId == 2)
            voting_cam1[2]++;
        if(clusterId == 3)
            voting_cam1[3]++;
    }

    // the average likelihood prediction from cam2
    vector<int> voting_cam2(4, 0);
    for (int i = 0; i < image_points_cam2.size(); i++) {

        Mat sample_cam2(1, 3, CV_64FC1);
        int b_cam2 = 0, g_cam2 = 0, r_cam2 = 0;
        int row_cam2 = image_points_cam2[i].x;
        int col_cam2 = image_points_cam2[i].y;

        //Get the color of each channel
//        Mat frame_hsv_cam2;
//        cvtColor(frame_cam2, frame_hsv_cam2, COLOR_BGR2HSV);
        b_cam2 = frame_cam2.at<Vec3b>(row_cam2, col_cam2)[0];
        g_cam2 = frame_cam2.at<Vec3b>(row_cam2, col_cam2)[1];
        r_cam2 = frame_cam2.at<Vec3b>(row_cam2, col_cam2)[2];

        //Put pixels in sample data
        sample_cam2.at<double>(0, 0) = static_cast<double>(b_cam2);
        sample_cam2.at<double>(0, 1) = static_cast<double>(g_cam2);
        sample_cam2.at<double>(0, 2) = static_cast<double>(r_cam2);

        // rounding
        vector<double> likelihoods(4);
        likelihoods[0] = (em_models[0]->predict2(sample_cam2, noArray()))[0];
        likelihoods[1] = (em_models[1]->predict2(sample_cam2, noArray()))[0];
        likelihoods[2] = (em_models[2]->predict2(sample_cam2, noArray()))[0];
        likelihoods[3] = (em_models[3]->predict2(sample_cam2, noArray()))[0];

        double max_likelihood = -10000;
        int clusterId = -1;
        for (int i = 0; i < 4; i++) {
            if(likelihoods[i] > max_likelihood) {
                max_likelihood = likelihoods[i];
                clusterId = i;
            }
        }

        if(clusterId == 0)
            voting_cam2[0]++;
        if(clusterId == 1)
            voting_cam2[1]++;
        if(clusterId == 2)
            voting_cam2[2]++;
        if(clusterId == 3)
            voting_cam2[3]++;
    }

    // the average likelihood prediction from cam3
    vector<int> voting_cam3(4, 0);
    for (int i = 0; i < image_points_cam3.size(); i++) {

        Mat sample_cam3(1, 3, CV_64FC1);
        int b_cam3 = 0, g_cam3 = 0, r_cam3 = 0;
        int row_cam3 = image_points_cam3[i].x;
        int col_cam3 = image_points_cam3[i].y;

        //Get the color of each channel
//        Mat frame_hsv_cam3;
//        cvtColor(frame_cam3, frame_hsv_cam3, COLOR_BGR2HSV);
        b_cam3 = frame_cam3.at<Vec3b>(row_cam3, col_cam3)[0];
        g_cam3 = frame_cam3.at<Vec3b>(row_cam3, col_cam3)[1];
        r_cam3 = frame_cam3.at<Vec3b>(row_cam3, col_cam3)[2];

        //Put pixels in sample data
        sample_cam3.at<double>(0, 0) = static_cast<double>(b_cam3);
        sample_cam3.at<double>(0, 1) = static_cast<double>(g_cam3);
        sample_cam3.at<double>(0, 2) = static_cast<double>(r_cam3);

        // rounding
        vector<double> likelihoods(4);
        likelihoods[0] = (em_models[0]->predict2(sample_cam3, noArray()))[0];
        likelihoods[1] = (em_models[1]->predict2(sample_cam3, noArray()))[0];
        likelihoods[2] = (em_models[2]->predict2(sample_cam3, noArray()))[0];
        likelihoods[3] = (em_models[3]->predict2(sample_cam3, noArray()))[0];

        double max_likelihood = -10000;
        int clusterId = -1;
        for (int i = 0; i < 4; i++) {
            if(likelihoods[i] > max_likelihood) {
                max_likelihood = likelihoods[i];
                clusterId = i;
            }
        }

        if(clusterId == 0)
            voting_cam3[0]++;
        if(clusterId == 1)
            voting_cam3[1]++;
        if(clusterId == 2)
            voting_cam3[2]++;
        if(clusterId == 3)
            voting_cam3[3]++;
    }

    // the average likelihood prediction from cam4
    vector<int> voting_cam4(4, 0);
    for (int i = 0; i < image_points_cam4.size(); i++) {

        Mat sample_cam4(1, 3, CV_64FC1);
        int b_cam4 = 0, g_cam4 = 0, r_cam4 = 0;
        int row_cam4 = image_points_cam4[i].x;
        int col_cam4 = image_points_cam4[i].y;

        //Get the color of each channel
//        Mat frame_hsv_cam4;
//        cvtColor(frame_cam4, frame_hsv_cam4, COLOR_BGR2HSV);
        b_cam4 = frame_cam4.at<Vec3b>(row_cam4, col_cam4)[0];
        g_cam4 = frame_cam4.at<Vec3b>(row_cam4, col_cam4)[1];
        r_cam4 = frame_cam4.at<Vec3b>(row_cam4, col_cam4)[2];

        //Put pixels in sample data
        sample_cam4.at<double>(0, 0) = static_cast<double>(b_cam4);
        sample_cam4.at<double>(0, 1) = static_cast<double>(g_cam4);
        sample_cam4.at<double>(0, 2) = static_cast<double>(r_cam4);

        // rounding
        vector<double> likelihoods(4);
        likelihoods[0] = (em_models[0]->predict2(sample_cam4, noArray()))[0];
        likelihoods[1] = (em_models[1]->predict2(sample_cam4, noArray()))[0];
        likelihoods[2] = (em_models[2]->predict2(sample_cam4, noArray()))[0];
        likelihoods[3] = (em_models[3]->predict2(sample_cam4, noArray()))[0];

        double max_likelihood = -10000;
        int clusterId = -1;
        for (int i = 0; i < 4; i++) {
            if(likelihoods[i] > max_likelihood) {
                max_likelihood = likelihoods[i];
                clusterId = i;
            }
        }

        if(clusterId == 0)
            voting_cam4[0]++;
        if(clusterId == 1)
            voting_cam4[1]++;
        if(clusterId == 2)
            voting_cam4[2]++;
        if(clusterId == 3)
            voting_cam4[3]++;
    }

    vector<double> total_voting(4, 0.0);
    for (int i = 0; i < 4; i++)
        total_voting[i] = voting_cam1[i] + voting_cam2[i] + voting_cam3[i] + voting_cam4[i];

//    cout << "voting of cluster 1: " << total_voting[0] << endl;
//    cout << "voting of cluster 2: " << total_voting[1] << endl;
//    cout << "voting of cluster 3: " << total_voting[2] << endl;
//    cout << "voting of cluster 4: " << total_voting[3] << endl;

    double max_voting = -10000;
    int clusterId = -1;
    for (int i = 0; i < 4; i++) {
        if(total_voting[i] > max_voting) {
            max_voting = total_voting[i];
            clusterId = i;
        }
    }

    cout << "predicted cluster is: " << clusterId << endl;

    return clusterId;
}