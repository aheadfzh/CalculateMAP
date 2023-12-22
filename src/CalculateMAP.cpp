#include "CalculateMAP.h"

// 寻找匹配 是target类的友函数
double calculateIoU(const TargetBox &origin, const TargetBox &result)
{
    double x_less = max(origin.center_x - origin.width / 2, result.center_x - result.width / 2);
    double y_less = max(origin.center_y - origin.hight / 2, result.center_y - result.hight / 2);
    double x_greater = min(origin.center_x + origin.width / 2, result.center_x + result.width / 2);
    double y_greater = min(origin.center_y + origin.hight / 2, result.center_y + result.hight / 2);

    double intersectionArea = max(0.0, x_greater - x_less) * max(0.0, y_greater - y_less); // 右边一定比左边大，上边一定比下边大
    double unionArea = origin.getArea() + result.getArea() - intersectionArea;
    if (unionArea == 0)
        return 0.0;
    return intersectionArea / unionArea;
}

void CalculateMAPClass::readAllTxtFiles(const string &folder_ori, const string &folder_res)
{
    DIR *dir_ori = opendir(folder_ori.c_str()), *dir_res = opendir(folder_res.c_str()); // 指向目录流指针
    dirent *entry_ori, *entry_res;                                                      // 指向目录中的条目的指针
    if (dir_ori == nullptr && dir_res == nullptr)
    {
        cerr << "Folder Path is Error ....." << endl;
        error_log.emplace_back("Folder Path is Error ");
        return;
    }
    vector<string> ori_fileNameVec;
    vector<string> res_fileNameVec;

    while ((entry_ori = readdir(dir_ori)) != nullptr, (entry_res = readdir(dir_res)) != nullptr)
    {
        string ori_filename = entry_ori->d_name;
        string res_filename = entry_res->d_name;
        string ori_filepath;
        string res_filepath;

        if (ori_filename.length() > 4 && res_filename.length() > 4 && ori_filename.substr(ori_filename.length() - 4) == ".txt" && res_filename.substr(res_filename.length() - 4) == ".txt")
        {
            total_ori_files++;
            total_res_files++;
            while (ori_filename != res_filename) // 跳过 对于检测数据txt遍历到了空文件名
            {
                ori_filepath = folder_ori + '/' + ori_filename;
                readTxtFiles(ori_filepath); // 只读取原始数据集

                entry_ori = readdir(dir_ori); //  然后下一个 文件
                ori_filename = entry_ori->d_name;
                total_ori_files++;
                total_emptyfiles++;
            }
            ori_filepath = folder_ori + '/' + ori_filename;
            res_filepath = folder_res + '/' + res_filename;
            readTxtFiles(ori_filepath, res_filepath);
        }
        else if (ori_filename == "." || ori_filename == ".." || res_filename == "." || res_filename == "..")
        {
            cout << "Read .  or  .." << endl;
            continue;
        }
        else
        {
            cerr << "Read Error file " << ori_filename << " or " << res_filename << endl;
            error_log.emplace_back("Read Error file " + ori_filename + " or " + res_filename);
            closedir(dir_ori);
            closedir(dir_res);
            return;
        }
    }

    closedir(dir_ori);
    closedir(dir_res);
}

/**
 * @brief 读入原始txt
 */
void CalculateMAPClass::readTxtFiles(string data_files_path, string result_files_path)
{
    ifstream files1(data_files_path);
    if (!files1.is_open())
    {
        cerr << "Error opening data txt file:" << data_files_path << endl;
        error_log.emplace_back("Error opening data txt file:" + data_files_path);
    }
    ifstream files2(result_files_path);
    if (!files2.is_open())
    {
        cerr << "Error opening result txt file:" << result_files_path << endl;
        error_log.emplace_back("Error opening result txt file:" + result_files_path);
    }
    if (error_log.size() > 0)
        return;

    vector<TargetBox> ori_pic_box_vec; // 临时容器 为了确定 iou 出作用域会销毁
    string line1;
    // cout << setprecision(8);
    while (getline(files1, line1)) // 读入原始图像检测信息的txt文件
    {
        istringstream iss(line1);
        int classIndex;
        double x, y, w, h;
        iss >> classIndex >> x >> y >> w >> h;
        ori_pic_box_vec.emplace_back(classIndex, x, y, w, h, 0.0);
        gt_map[classIndex].emplace_back(classIndex, x, y, w, h, 0.0); // 放入已分类的容器
    }
    files1.close();
    // cout << "\n";
    string line2;
    while (getline(files2, line2)) // 读入检测后转化的txt
    {
        istringstream iss(line2);
        int classIndex;
        double x, y, w, h, c;
        iss >> classIndex >> x >> y >> w >> h >> c;
        TargetBox box(classIndex, x, y, w, h, c);
        double bestIoU = 0; // 0.5
        // 寻转匹配一定是在同一张图片中
        for (size_t j = 0; j < ori_pic_box_vec.size(); j++)
        {
            if (box.class_index == ori_pic_box_vec[j].class_index)
            {
                double temp_iou = calculateIoU(ori_pic_box_vec[j], box);
                if (temp_iou >= bestIoU)
                    bestIoU = temp_iou;
            }
        }
        box.iou = bestIoU; // 更新iou
        pred_map[classIndex].push_back(box);
    }
    files2.close();
    // cout << "read txt data over......" << endl;
}
// 重载
void CalculateMAPClass::readTxtFiles(string data_files_path)
{
    ifstream files1(data_files_path);
    if (!files1.is_open())
    {
        cerr << "Error opening data txt file:" << data_files_path << endl;
        error_log.emplace_back("Error opening data txt file:" + data_files_path);
    }
    if (error_log.size() > 0)
        return;

    vector<TargetBox> ori_pic_box_vec; // 临时容器 为了确定 iou 出作用域会销毁
    string line1;
    // cout << setprecision(8);
    while (getline(files1, line1)) // 读入原始图像检测信息的txt文件
    {
        istringstream iss(line1);
        int classIndex;
        double x, y, w, h;
        iss >> classIndex >> x >> y >> w >> h;
        ori_pic_box_vec.emplace_back(classIndex, x, y, w, h, 0.0);
        gt_map[classIndex].emplace_back(classIndex, x, y, w, h, 0.0); // 放入已分类的容器
    }
    files1.close();
    // cout << "\n";
}

/**
 * @brief 计算单个类别的AP
 * @param  gt_vec           My Param doc
 * @param  pred_vec         My Param doc
 * @param  iouThreshold     My Param doc
 * @return double
 */
double CalculateMAPClass::calSingleClassAP(std::vector<TargetBox> &gt_vec, std::vector<TargetBox> &pred_vec, int class_index)
{

    std::sort(pred_vec.begin(), pred_vec.end(), [](const TargetBox &a, const TargetBox &b)
              { return a.confidence > b.confidence; }); // 按置信度排序
    // for (auto &temp : pred_vec)
    // {
    //     cout << temp.confidence << " | ios:" << temp.iou << endl;
    // }

    // cout << "--next--one--" << endl;

    int gt_number = gt_vec.size();
    int fp_cur = 0, tp_cur = 0; // current
    // cout << left << setprecision(7) << setw(8);
    // cout << "cl\t"
    //      << "TP\t"
    //      << "FP\t"
    //      << "FN\t"
    //      << "pre\t"
    //      << "rec" << endl;

    for (size_t i = 0; i < pred_vec.size();)
    {
        // 以当前的预测框作为划分阈值 t or f
        do
        {
            if (pred_vec[i].iou >= (iouThreshold - 10e-8)) // 成功 匹配
                tp_cur++;
            else
                fp_cur++;
        } while (pred_vec[i].confidence == pred_vec[++i].confidence);

        double cur_precision = double(tp_cur) / double(tp_cur + fp_cur);
        double cur_recall = double(tp_cur) / double(gt_number);

        precision[class_index].emplace_back(cur_precision);
        recall[class_index].emplace_back(cur_recall);
        // cout << class_index << "\t" << tp_cur << "\t" << fp_cur << "\t" << gt_number - tp_cur << "\t" << cur_precision << "\t" << cur_recall << endl;
    }
    // 计算AP（Average Precision）：使用梯形法则计算P-R曲线下的面积
    double temp_ap = 0.0;
    for (size_t i = 0; i < precision[class_index].size(); ++i)
    {
        double area = 0;
        if (i == 0)
            area = 0.5 * (recall[class_index][0] - 0) * (precision[class_index][0] + precision[class_index][0]);
        else
            area = 0.5 * (recall[class_index][i] - recall[class_index][i - 1]) * (precision[class_index][i] + precision[class_index][i - 1]);
        temp_ap += area;
    }
    ap.emplace_back(class_index, temp_ap);
    return temp_ap;
}

/**
 * @brief 计算map
 */
void CalculateMAPClass::calculateTotalAP()
{
    if (error_log.size() > 0)
        return;
    double totalAP = 0.0;
    for (auto &entry : gt_map)
    {
        auto class_index = entry.first;
        auto &gt_box_vec = entry.second;
        auto predIter = pred_map.find(class_index); // 迭代器
        if (predIter != pred_map.end())             // 找到了对应的
        {
            auto &pred_box_vec = predIter->second;
            totalAP += calSingleClassAP(gt_box_vec, pred_box_vec, class_index);
        }
        else // 没有找到
        {
            ap.emplace_back(class_index, 0);
            totalAP += 0;
        }
    }
    MAP = totalAP / gt_map.size();
}

/**
 * @brief 输出结果文件
 */
void CalculateMAPClass::printResult()
{
    std::ofstream outfile("MAP_Result.txt"); //
    if (outfile.is_open() == false)
    {
        cout << "create result file failed......" << endl;
        return;
    }
    if (error_log.size() > 0)
    {
        for (const auto &iter : error_log)
            outfile << iter << endl; // 将字符串写入文件
        return;
    }
    if (ap.size() > 0)
    {
        // 设置输出格式
        outfile << std::left << ::setw(10) << "class"
                << std::left << ::setw(15) << "ap"
                << std::left << ::setw(15) << "ori_number"
                << std::left << ::setw(15) << "res_number" << std::endl;
        // 设置小数点精度
        outfile << std::fixed << std::setprecision(4);
        for (const auto &iter : ap)
        {
            outfile << std::left << ::setw(10) << iter.first
                    << std::left << ::setw(15) << iter.second
                    << std::left << ::setw(15) << gt_map[iter.first].size()
                    << std::left << ::setw(15) << pred_map[iter.first].size() << std::endl;
        }
        outfile << "-----------------------------------------------------" << endl
                << "MAP:" << std::setw(15) << MAP << std::endl;
    }
    outfile << "Total: " << total_ori_files << " origin files, " << total_res_files << " detect files, " << total_emptyfiles << " origin files without detect result" << endl;
    // 获取当前时间戳
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // 转换为字符串
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // 在输出文件中添加时间戳
    outfile << "Timestamp: " << timestamp << std::endl;

    outfile.close(); // 关闭文件
    return;
}
