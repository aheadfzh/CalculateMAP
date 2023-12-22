#include "main.h"
using namespace std;
// "E:/VScodeProject/CPlusPlus/CPludPlusStudy/txtfiles/origin"
// "E:/VScodeProject/CPlusPlus/CPludPlusStudy/txtfiles/detect"

int main(int argc, const char *argv[])
{
    CalculateMAPClass solve;
    if (argc < 2)
    {
        cerr << "please input two paths: origin data files path and detect result files path" << endl;
        return 1;
    }
    solve.readAllTxtFiles(argv[1], argv[2]);
    // solve.readAllTxtFiles("E:/VScodeProject/CPlusPlus/CPludPlusStudy/txtfiles/origin_labels", "E:/VScodeProject/CPlusPlus/CPludPlusStudy/txtfiles/detect_labels");
    solve.calculateTotalAP();
    solve.show();
    solve.printResult();
    return 0;
}