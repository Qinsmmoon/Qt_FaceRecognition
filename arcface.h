#ifndef ARCFACE_H
#define ARCFACE_H

#include <cmath>
#include <vector>
#include <string>
#include "net.h"
#include <opencv2/highgui.hpp>


//ncnn::Mat preprocess(ncnn::Mat img, FaceInfo info);

float calcSimilar(std::vector<float> feature1, std::vector<float> feature2);


class Arcface {
    const std::string project_path = "E:/qtproject/FaceRec";

public:
    Arcface(std::string model_folder = ".");
    ~Arcface();
    cv::Mat getFeature(cv::Mat img);

private:
    ncnn::Net net;

    const int feature_dim = 128;

    void normalize(std::vector<float>& feature);
};

#endif

