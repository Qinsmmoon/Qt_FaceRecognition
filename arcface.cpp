#include "arcface.h"

Arcface::Arcface(std::string model_folder)
{
    std::string param_file = project_path + "/models/mobilefacenet/mobilefacenet.param";
    std::string bin_file = project_path + "/models/mobilefacenet/mobilefacenet.bin";

    this->net.load_param(param_file.c_str());
    this->net.load_model(bin_file.c_str());
}

Arcface::~Arcface()
{
    this->net.clear();
}

// 提取人脸图片128*128的特征矩阵
cv::Mat Arcface::getFeature(cv::Mat img)
{
    std::vector<float> feature;

    //cv to NCNN
    ncnn::Mat in = ncnn::Mat::from_pixels(img.data, ncnn::Mat::PIXEL_BGR, img.cols, img.rows);

    //in = bgr2rgb(in);
    ncnn::Extractor ex = net.create_extractor();
    ex.set_light_mode(true);
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("fc1", out);
    feature.resize(this->feature_dim); // 128*128
    for (int i = 0; i < this->feature_dim; i++)
        feature[i] = out[i];

    //normalize(feature);
    cv::Mat feature__ = cv::Mat(feature, true);

    return feature__;
}

//归一化
void Arcface::normalize(std::vector<float>& feature)
{
    float sum = 0;
    for (auto it = feature.begin(); it != feature.end(); it++)
        sum += (float)*it * (float)*it;
    sum = sqrt(sum);
    for (auto it = feature.begin(); it != feature.end(); it++)
        *it /= sum;
}

//计算相似值
float calcSimilar(std::vector<float> feature1, std::vector<float> feature2)
{
    //assert(feature1.size() == feature2.size());
    float sim = 0.0;
    for (int i = 0; i < feature1.size(); i++)
        sim += feature1[i] * feature2[i];
    return sim;
}
