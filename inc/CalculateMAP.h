#pragma

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <unordered_map>
#include <set>
#include <dirent.h>
#include <chrono>
#include <ctime> // 包含头文件以使用 std::localtime 和 std::put_time

using namespace std;
// namespace fs = std::filesystem;
// 目标检测框
class TargetBox
{
public:
    int class_index;
    double center_x;
    double center_y;
    double width;
    double hight;
    double confidence; // 置信度
    double iou;

    TargetBox(double index, double cx, double cy, double w, double h) : class_index(index), center_x(cx), center_y(cy), width(w), hight(h), iou(0) {}

    TargetBox(double index, double cx, double cy, double w, double h, double c) : class_index(index), center_x(cx), center_y(cy), width(w), hight(h), confidence(c), iou(0) {}

    double getArea() const { return width * hight; }
    friend double calculateIoU(const TargetBox &origin, const TargetBox &result);
};

double calculateIoU(const TargetBox &origin, const TargetBox &result);

// 由 confidence判断P or N 正负样本
// 由 IOU判断  T or F 判断正误
class CalculateMAPClass
{
private:
    //  TP;    // IoU>threshold 的检测框数量   // 假正确率 检测到了的正确目标会根据
    //  FP;    // IoU<=threshold 的检测框数量  // 假阳性   检测到了的非正确目标
    //  FN;    // 没有检测到的目标数量 漏检测         没有检测到的正确目标
    std::unordered_map<int, vector<double>> precision; // 查准率 TP/(TP+FP)
    std::unordered_map<int, vector<double>> recall;    // 查全率 TP/(TP+FN)
    std::vector<std::pair<int, double>> ap;            // p-r曲线下的面积
    double MAP;
    // int ap;
    // TP+FN 正确目标数量
    double iouThreshold;

    std::vector<string> error_log;
    int total_ori_files, total_res_files, total_emptyfiles;

public:
    CalculateMAPClass() : iouThreshold(0.5), MAP(0)
    {
        total_ori_files = 0;
        total_res_files = 0;
        total_emptyfiles = 0;
    }
    std::unordered_map<int, vector<TargetBox>> gt_map;
    std::unordered_map<int, vector<TargetBox>> pred_map;

    void readAllTxtFiles(const string &folder_ori, const string &folder_res);
    void readTxtFiles(string data_files_path, string result_files_path);
    void readTxtFiles(string data_files_path);
    double calSingleClassAP(std::vector<TargetBox> &gt, std::vector<TargetBox> &pred, int class_index);
    void calculateTotalAP();
    double getMAP() { return MAP; }
    void show()
    {
        if (error_log.size() > 0)
            return;
        cout << std::left << ::setw(10) << "class"
             << std::left << ::setw(15) << "ap"
             << std::left << ::setw(15) << "ori_number"
             << std::left << ::setw(15) << "res_number" << std::endl;
        std::sort(ap.begin(), ap.end(), [](const auto &a, const auto &b)
                  { return a.first < b.first; });
        for (auto &iter : ap)
        {
            cout << std::left << ::setw(10) << iter.first
                 << std::left << ::setw(15) << iter.second
                 << std::left << ::setw(15) << gt_map[iter.first].size()
                 << std::left << ::setw(15) << pred_map[iter.first].size() << endl;
        }
        cout << "map:" << MAP << endl;
    }
    void printResult();
};
