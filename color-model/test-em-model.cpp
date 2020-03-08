#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace  std;
using namespace cv;
using namespace cv::ml;

int main(void) {
    Mat src;

    //Get the complete picture path and name
    string pic = string("./data/cam1/frame.png");

    //Print picture path
    cout << "pic path is :"<<pic<<endl;

    //Read pictures
    src = imread(pic, 1);

    //Judge whether the picture exists
    if(src.empty()) {
        cout<<"pic is not exist!!!!"<<endl;
        return -1;
    }

    //display picture
    namedWindow("src pic",WINDOW_AUTOSIZE);
    imshow("src pic",src);
    waitKey(0);
    destroyWindow("src pic");

    //Define 5 colors with a maximum classification of no more than 5
    Scalar color_tab[] = {
            Scalar(0,0,255),
            Scalar(0,255,0),
            Scalar(255,0,0),
            Scalar(0,255,255),
            Scalar(255,0,255)
    };

    int width = src.cols;
    int height = src.rows;
    int dims = src.channels();

    int nsamples = width*height;
    Mat points(nsamples, dims, CV_64FC1);
    Mat labels;
    Mat result = Mat::zeros(src.size(), CV_8UC3);

    cout << "size: " << src.size() << endl;
    cout << "nsamples: " << nsamples << endl;

    //Define classification, that is, how many classification points of function K value
    int num_cluster = 3;
    printf("num of num_cluster is %d\n",num_cluster);
    // Image RGB pixel data to sample data
    int index = 0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            index = row*width + col;
            Vec3b rgb = src.at<Vec3b>(row, col);
            points.at<double>(index, 0) = static_cast<int>(rgb[0]);
            points.at<double>(index, 1) = static_cast<int>(rgb[1]);
            points.at<double>(index, 2) = static_cast<int>(rgb[2]);
        }
    }

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

    // Mark color and display for each pixel
    Mat sample(1, dims, CV_64FC1);//
    int r = 0, g = 0, b = 0;
    //Put each pixel in the sample
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            index = row*width + col;

            //Get the color of each channel
            b = src.at<Vec3b>(row, col)[0];
            g = src.at<Vec3b>(row, col)[1];
            r = src.at<Vec3b>(row, col)[2];

            //Put pixels in sample data
            sample.at<double>(0, 0) = static_cast<double>(b);
            sample.at<double>(0, 1) = static_cast<double>(g);
            sample.at<double>(0, 2) = static_cast<double>(r);

            //Rounding
            int response = cvRound(em_model->predict2(sample, noArray())[1]);
            Scalar c = color_tab[response];
            result.at<Vec3b>(row, col)[0] = c[0];
            result.at<Vec3b>(row, col)[1] = c[1];
            result.at<Vec3b>(row, col)[2] = c[2];
        }
    }
    imshow("EM-Segmentation", result);
    cout << "here" << endl;
    waitKey(0);
    destroyAllWindows();
    return 0;
}