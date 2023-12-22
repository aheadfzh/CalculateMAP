## 计算目标检测指标map

+ 使用方法： 编译后在build文件夹下生成 calculate_map.exe 可执行文件(linux：calculate_map)； 终端执行并给两个参数，
	- 第一个参数：原数据集txt文件夹路径(绝对路径)
	- 第二个参数：检测后输出的txt文件夹路径(绝对路径)
	- 示例：  .\calculate_map.exe "\path\of\origin\txt_files" "\path\of\detect_result\txt_files"

+ 终端结果输出： 
    - 各个类的ap值，
    - 原数据集标注各类的总数
    - 各类检测结果总数。
    - map结果
  
+ 结果文件保存，执行文件路径下生成MAP_result.txt文件：
    - 保存各个类的ap值
    - 原数据集标注各类的总数
    - 各类检测结果总数。
    - map结果
    - 读取原txt文件和检测结果文件数量，以及没有任何检测目标的图片数量
    - 文件保存时间戳。