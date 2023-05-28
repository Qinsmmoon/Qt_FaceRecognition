#include <math.h>
#include <time.h>
#include "livefacereco.hpp"
#include "mtcnn.h"
#include "FacePreprocess.h"
#include <algorithm>
#include "math.hpp"

#define PI 3.14159265

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
//#include "stdtostring.h"
#endif

double sum_score, sum_fps, sum_confidence;

#define  PROJECT_PATH "E:/qtproject/FaceRec";

//sigmod函数对人脸置信度归一化 数据为提前算得
static std::string identifyConfidence;
void getIdentifyConfidence(double con)
{
    con = (1 / (1 + exp(-21.3795 * con + 8.72687)));
    identifyConfidence = std::to_string(con);
}

//处理图片路径 得到人脸姓名使用
std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

    return output;
}

// face_descriptors_map中 string为人脸名称 Mat为归一化的人脸特征向量
void calculateFaceDescriptorsFromDisk(Arcface& facereco, std::map<std::string, cv::Mat>& face_descriptors_map)
{
    const std::string project_path = PROJECT_PATH;
    std::string pattern_jpg = project_path + "/img/*.jpg";
    std::vector<cv::String> image_names;

    cv::glob(pattern_jpg, image_names, true); // 遍历pattern_jpg下的图片 存入image_names中

    int image_number = image_names.size(); // 图片数量

    if (image_number == 0) {
        //std::cout << "No image files[jpg]" << std::endl;
        //std::cout << "At least one image of 112*112 should be put into the img folder. Otherwise, the program will broke down." << std::endl;
        exit(0);
    }

    cv::Mat  face_img;


    // 获取图片下人脸姓名
    for (auto const& img_name : image_names)
    {
        auto splitted_string = split(img_name, '/');

        auto splitted_string_2 = splitted_string[splitted_string.size() - 1];

        std::size_t name_length = splitted_string_2.find_last_of('_');
        auto person_name = splitted_string_2.substr(0, name_length);
        face_img = cv::imread(img_name);

        cv::Mat face_descriptor = facereco.getFeature(face_img); // 128*128特征矩阵

        face_descriptors_map[person_name] = Statistics::zScore(face_descriptor);

    }


}

void calculateFaceDescriptorsFromImgDataset(Arcface& facereco, std::map<std::string, std::list<cv::Mat>>& img_dataset, std::map<std::string, std::list<cv::Mat>>& face_descriptors_map)
{
    int img_idx = 0;
    const int image_number = img_dataset.size() * 5;
    for (const auto& dataset_pair : img_dataset)
    {
        const std::string person_name = dataset_pair.first;

        std::list<cv::Mat> descriptors;
        if (image_number == 0) {
            std::cout << "No image files[jpg]" << std::endl;
            return;
        }
        else {
            std::cout << "loading pictures..." << std::endl;
            for (const auto& face_img : dataset_pair.second)
            {
                cv::Mat face_descriptor = facereco.getFeature(face_img);
                descriptors.push_back(Statistics::zScore(face_descriptor));
                //std::cout << "now loading image " << ++img_idx << " out of " << image_number << std::endl;
                //printf("\rloading[%.2lf%%]\n",  (++img_idx)*100.0 / (image_number));
            }
            face_descriptors_map[person_name] = std::move(descriptors);
        }

    }
}
void loadLiveModel(Live& live)
{
    //Live detection configs
    struct ModelConfig config1 = { 2.7f,0.0f,0.0f,80,80,"model_1",false };
    struct ModelConfig config2 = { 4.0f,0.0f,0.0f,80,80,"model_2",false };
    std::vector<struct ModelConfig> configs;
    configs.emplace_back(config1);
    configs.emplace_back(config2);
    live.LoadModel(configs);
}

cv::Mat createFaceLandmarkGTMatrix()
{
    // 标准的关键点。
    float v1[5][2] = {
            {30.2946f, 51.6963f},
            {65.5318f, 51.5014f},
            {48.0252f, 71.7366f},
            {33.5493f, 92.3655f},
            {62.7299f, 92.2041f} };

    cv::Mat src(5, 2, CV_32FC1, v1);
    memcpy(src.data, v1, 2 * 5 * sizeof(float));
    return src.clone();
}

cv::Mat createFaceLandmarkMatrixfromBBox(const Bbox& box)
{
    float v2[5][2] =
    { {box.ppoint[0], box.ppoint[5]},
    {box.ppoint[1], box.ppoint[6]},
    {box.ppoint[2], box.ppoint[7]},
    {box.ppoint[3], box.ppoint[8]},
    {box.ppoint[4], box.ppoint[9]},
    };
    cv::Mat dst(5, 2, CV_32FC1, v2);
    memcpy(dst.data, v2, 2 * 5 * sizeof(float));

    return dst.clone();
}

Bbox getLargestBboxFromBboxVec(const std::vector<Bbox>& faces_info)
{
    if (faces_info.size() > 0)
    {
        int lagerest_face = 0, largest_number = 0;
        for (int i = 0; i < faces_info.size(); i++) {
            int y_ = (int)faces_info[i].y2 * ratio_y;
            int h_ = (int)faces_info[i].y1 * ratio_y;
            if (h_ - y_ > lagerest_face) {
                lagerest_face = h_ - y_;
                largest_number = i;
            }
        }

        return faces_info[largest_number];
    }
    return Bbox();
}

LiveFaceBox Bbox2LiveFaceBox(const Bbox& box)
{
    float x_ = box.x1;
    float y_ = box.y1;
    float x2_ = box.x2;
    float y2_ = box.y2;
    int x = (int)x_;
    int y = (int)y_;
    int x2 = (int)x2_;
    int y2 = (int)y2_;
    struct LiveFaceBox  live_box = { x_,y_,x2_,y2_ };
    return live_box;
}

// 人脸对齐
cv::Mat alignFaceImage(const cv::Mat& frame, const Bbox& bbox, const cv::Mat& gt_landmark_matrix)
{
    cv::Mat face_landmark = createFaceLandmarkMatrixfromBBox(bbox);

    cv::Mat transf = FacePreprocess::similarTransform(face_landmark, gt_landmark_matrix);

    cv::Mat aligned = frame.clone();
    cv::warpPerspective(frame, aligned, transf, cv::Size(96, 112), cv::INTER_LINEAR);
    resize(aligned, aligned, cv::Size(112, 112), 0, 0, cv::INTER_LINEAR);

    return aligned.clone();
}


//得到与特征值距离最近的人脸姓名
std::string getClosestFaceDescriptorPersonName(std::map<std::string, cv::Mat>& disk_face_descriptors, cv::Mat face_descriptor)
{
    std::vector<double> score_(disk_face_descriptors.size());

    std::vector<std::string> labels;

    int i = 0;

    for (const auto& disk_descp : disk_face_descriptors)
    {
        // cout << "comparing with " << disk_descp.first << endl;

        score_[i] = (Statistics::cosineDistance(disk_descp.second, face_descriptor)); // 计算输入特征与真实特征之间的余弦距离
        //cout << "score  " << score_[i] << endl;
        labels.push_back(disk_descp.first);
        i++;
    }
    int maxPosition = max_element(score_.begin(), score_.end()) - score_.begin();
    getIdentifyConfidence(score_[maxPosition]);
    int pos = score_[maxPosition] > face_thre ? maxPosition : -1;
    //std::cout << "score_[maxPosition] " << score_[maxPosition] << std::endl;
    std::string person_name = "";
    if (pos >= 0)
    {
        person_name = labels[pos];
    }
    score_.clear();

    return person_name;
}


Arcface facereco;
std::map<std::string, cv::Mat> face_descriptors_dict;
cv::Mat face_landmark_gt_matrix = createFaceLandmarkGTMatrix();
std::string person_name;
std::string liveface; // Fake face or True face
float confidence;
cv::Mat aligned_img;
Live live;


//人脸检测
void MTCNNDetection(cv::Mat frame)
{
    //face_descriptors_dict中 存储的是人脸名称和对应的人脸特征向量
    calculateFaceDescriptorsFromDisk(facereco, face_descriptors_dict);
    loadLiveModel(live);


    float ratio_x = 1;
    float ratio_y = 1;

     // 检测人脸
    std::vector<Bbox> faces_info = detect_mtcnn(frame);
    if (faces_info.size() >= 1)
    {
        auto large_box = getLargestBboxFromBboxVec(faces_info);

        LiveFaceBox live_face_box = Bbox2LiveFaceBox(large_box);

        // 获得对齐后的人脸图片数据矩阵
        aligned_img = alignFaceImage(frame, large_box, face_landmark_gt_matrix);

        // 获得对齐后人脸图片128*128的特征矩阵
        cv::Mat face_descriptor = facereco.getFeature(aligned_img);

        cv::rectangle(frame, cv::Point(large_box.x1 * ratio_x, large_box.y1 * ratio_y), cv::Point(large_box.x2 * ratio_x, large_box.y2 * ratio_y), cv::Scalar(255, 0, 0), 2);

        // 人脸特征矩阵归一化
        face_descriptor = Statistics::zScore(face_descriptor);

        // 获得人脸姓名
        person_name = getClosestFaceDescriptorPersonName(face_descriptors_dict, face_descriptor);

        confidence = live.Detect(frame, live_face_box);

        if (confidence <= true_thre)
        {
            liveface = "Fake face";
        }
        else
        {
            liveface = "True face";
        }
    }

}


//人脸对齐处理并将用户注册信息存入文件夹
void FaceRegisterAligned(cv::Mat frame, std::string new_name){
    std::vector<Bbox> faces_info = detect_mtcnn(frame);
    if (faces_info.size() >= 1)
    {
        // cv::Mat face_landmark_gt_matrix = createFaceLandmarkGTMatrix();
        auto large_box = getLargestBboxFromBboxVec(faces_info);
        aligned_img = alignFaceImage(frame, large_box, face_landmark_gt_matrix);
        cv::Mat face_descriptor = facereco.getFeature(aligned_img);
        face_descriptor = Statistics::zScore(face_descriptor);
        cv::imwrite(project_path + "/img/" + new_name + "_0.jpg", aligned_img);
        face_descriptors_dict[new_name] = face_descriptor;
    }
}


//返回人脸姓名
std::string getName(){
    return person_name;
}

//返回人脸检测姓名置信度
std::string getFaceConfidence(){
    return identifyConfidence;
}

//返回活体检测结果
std::string getLive(){
    return liveface;
}

//返回活体检测值
float getLiveConfidence(){
    return confidence;
}
