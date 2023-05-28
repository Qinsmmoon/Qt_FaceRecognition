#ifndef __MATH__HEADER__
#define __MATH__HEADER__

#include <opencv2/opencv.hpp>

namespace Statistics
{
#define PI 3.14159265
    using namespace cv;
    /*
    计算余弦距离之前的一个归一化函数。可以破坏原分布，使两个特征更加明显。
    mean均值设置为0,std标准差设置为1。
    */
    cv::Mat zScore(const cv::Mat& fc)
    {
        cv::Mat mean, std;
        cv::meanStdDev(fc, mean, std);
        //cout <<"mean is :"<< mean <<"std is :"<< std << endl;
        cv::Mat fc_norm = (fc - mean) / std;
        return fc_norm;
    }

    /*
    计算输入特征与真实特征之间的余弦距离
    */
    inline float cosineDistance(const cv::Mat& v1, const cv::Mat& v2) {
        double dot = v1.dot(v2);
        double denom_v1 = norm(v1);
        double denom_v2 = norm(v2);
        return dot / (denom_v1 * denom_v2);
    }

    /*
     计算脸的旋转角度
     */
    inline double countAngle(float landmark[5][2]) {
        double a = landmark[2][1] - (landmark[0][1] + landmark[1][1]) / 2;
        double b = landmark[2][0] - (landmark[0][0] + landmark[1][0]) / 2;
        double angle = atan(abs(b) / a) * 180 / PI;
        return angle;
    }
}
#endif

