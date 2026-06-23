//#include"MMACsolver6.h"
//#include"tabu_weighted.h"
#include"MMACsolver9.h"
#include <iostream>
#include <filesystem> 
int main()
{
     /*string file = "D:\\C++\\MMAC\\instances\\uniform\\test.txt";*/
     /*string file = "D:\\CodeProgram\\C++\\Heuristic_Algorithm\\DMCLS\\MMAC\\instances\\uniform\\.txt";*/
     /*string file = "D:\\C++\\MMAC\\instances\\connected\\c1000_2000_25_4_3.txt";*/
    /* string file = "D:\\C++\\MMAC\\instances\\Rome\\rome.30.35.74.txt";*/
     /*string file = "D:\\C++\\MMAC\\instances\\North\\north.40.49.2.txt";*/
   /*  Short_k = 6;
     Long_k = 5;
     s_restore = 7;
     MMACsolver MMAC(file,3);
     MMAC.solve();*/

     Short_k = 6;
     Long_k = { 1,1 };
     s_restore = 3;
    const std::string target_folder = R"(D:\CodeProgram\C++\Heuristic_Algorithm\DMCLS\MMAC\\instances\connected)";  
    for (const auto& entry : std::filesystem::directory_iterator(target_folder)) {
        std::string file_path = entry.path().string();
        for (int i = 0; i < 10; i++)
        {
            MMACsolver MMAC(file_path, i);
            cout << file_path << ",";
            MMAC.solve();
        }
    }
}

