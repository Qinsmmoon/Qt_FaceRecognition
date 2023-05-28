#include <stdio.h>
#include <iostream>
#include <array>
#include <vector>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include "arcface.h"
#include "live.h"

// 可调参数
const bool largest_face_only = true; // 是否只检测最大人脸
const int distance_threshold = 90; // 防止过远人脸误检测
const float face_thre = 0.40; // 人脸识别阈值
const float true_thre = 0.89; // 活体检测阈值
const int jump = 10; // 跳帧，跳过某几帧的人脸识别和活体检测来加速
const int input_width = 320; // 输入宽度
const int input_height = 240; // 输入高度
const int output_width = 320; // 输出宽度
const int output_height = 240; // 输出高度（输出太大会降低速度）
const std::string project_path = "E:/qtproject/FaceRec"; // 目录
//end

const cv::Size frame_size = cv::Size(output_width, output_height);
const float ratio_x = (float)output_width / input_width;
const float ratio_y = (float)output_height / input_height;

void MTCNNDetection(cv::Mat);
std::string getName();
std::string getFaceConfidence();
std::string getLive();
float getLiveConfidence();
void FaceRegisterAligned(cv::Mat, std::string);



