#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>
/*
* date: 2024-4-17
* author: JLX
* func: Blazek 结构化欧拉的前后处理程序
*/
using namespace std;

// 去除字符串前后的所有空白字符
std::string trim(const std::string& str) {
    auto left = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });

    auto right = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();

    if (left >= right) return "";
    return std::string(left, right);
}

/*
* 输出 tecplotWrite2D 数据
*/
void tecplotWrite2D(string filename, const vector<vector<vector<double>>>&& data, const vector<string>&& labels={});

/*
* 读取Blazek ".grd" 类型网格
*/
void Read_Blazek_grd(string filename, vector<vector<double>> &X, vector<vector<double>> &Y); 

/*
* 读取Blazek ".v2d" 流场类型数据
*/
void Read_Blazek_FlowV2D(string filename, vector<vector<vector<double>>>& data, vector<string>& labels); 

/*
* 写入 PLOT3D 类型网格
*/
void PLOT3Dwrite2D(string filename, const vector<vector<double>> &X, const vector<vector<double>> &Y);

/*
* 错误处理
*/
void ERROR_Handle(bool flag = false, string msg="");

int main(int argc, char * argv[])
{
    std::cout << "*****************************************************************************************"<<std::endl;
    std::cout<< "     Preprocessing and Postprocessing Programs for Blazek's Structured Euler Code" <<std::endl;
    std::cout << "*****************************************************************************************"<<std::endl;
    std::cout <<"Preprocessing: 0, Postprocessing: 1, Other number: Quit"<<std::endl;
    string filename = "";
    int flag = -1;
    std::cin >> flag;
    ERROR_Handle(!cin.fail(), "input error!");
    if (flag == 0) {
        // 执行前处理相关代码
        cout << "******************************Preprocessing***********************************************"<<endl;
        cout <<"To convert a Blazek \".grd\" type mesh to plot3D \".x\" type, press 0. "<<endl;
        cout <<"To convert a \".x\" type mesh to Blazek type, press 1. "<<endl;
        cout <<"Other number: Quit"<<endl;
        cin >> flag;
        ERROR_Handle(!cin.fail(), "input error!");
        // 将Blazek ".grd" 类型网格转换成 PLOT3D 格式
        if(flag== 0){
            cout <<"Please enter the path of the file to be converted:";      
            cin >> filename;  ERROR_Handle(!cin.fail(), "input error!");
            cout << "\r"; 
            vector<vector<double>> X, Y;
            Read_Blazek_grd(filename, X, Y);
            // 输出 tecplot 格式文件
            string tecplot_filename = filename.substr(0, filename.length() - 4);
            cout << "output " <<"tecplot format " <<tecplot_filename <<"..."<<endl; 
            tecplotWrite2D(tecplot_filename, {X, Y}, {"x", "y"});
            string plot3d_filename = filename.substr(0, filename.length() - 4);
            cout << "output " << "plot3d format " <<plot3d_filename <<"..."<<endl; 
            PLOT3Dwrite2D(plot3d_filename, X, Y);
        }
        // 将 PLOT3D 格式转换成Blazek ".grd" 类型网格
        else if(flag == 1){
            
        }else{
            std::cout << "Quit" << std::endl;
            return 0;
        }
    } else if (flag == 1) {
        // 执行后处理相关代码
         cout << "******************************Postprocessing***********************************************"<<endl;
        cout <<"Plot flow field data, press 0."<<endl;
        cout <<"plot residual data, press 1. "<<endl;
        cout <<"plot surface data, press 2. "<<endl;
        cout <<"Other number: Quit"<<endl;
        cin >> flag;
        ERROR_Handle(!cin.fail(), "input error!");
        // 0: 处理流场数据
        if(flag == 0){
            cout <<"Please enter the path of the file to be converted:";      
            cin >> filename;  ERROR_Handle(!cin.fail(), "input error!");
            cout << "\r"; 
            vector<vector<vector<double>>> data; vector<string> labels;
            Read_Blazek_FlowV2D(filename, data, labels);
            string tecplot_filename = filename.substr(0, filename.length() - 4);
            cout << "output " <<"tecplot format " <<tecplot_filename <<"..."<<endl; 
            tecplotWrite2D(tecplot_filename, move(data), move(labels));
        }
        // 1: 处理残差数据
        else if(flag == 1){

        }
        // 2: 处理表面数据
        else if(flag == 2){

        }else {

        }
    } else {
        std::cout << "Quit" << std::endl;
        return 0;
    }
    std::cout << "End!" << std::endl;
    return 0;
}

void tecplotWrite2D(string filename, const vector<vector<vector<double>>>&& data, const vector<string>&& labels){
    filename += ".plt";
    ofstream file(filename);
    auto variables(labels);                                         // 标签数组    
    const int N = data.size();                                      // 数据的数量
    for(int i = labels.size()+1; i <= N; i++){                      // 填充 variables
        variables.push_back("V"+to_string(i));
    }
    const int m = data[0].size(); const int n = data[0][0].size();  // 网格大小
    /*--- tecplot 文件头 ---*/
    file << "TITLE = \"contour\"" << endl;						    ///< 标题
    file << "VARIABLES = ";                                         ///< 输出标题
    for(int i = 0; i < variables.size(); i++){
        if(i != (variables.size() - 1)){
            file << "\"" << variables[i] << "\", ";
        }else{
            file <<"\"" << variables[i] << "\"" << endl;
        }
    }
    //
    file << "zone I = " << m << ", " << "J = " << n << endl;	///< 数据大小
    file << "DATAPACKING = point" << endl;						///< 每一行是一个点
    /*--- tecplot 数据 ---*/
    for (int j = 0; j < n; j++) {								///< 从左到右，从下到上遍历网格
        for (int i = m - 1; i >= 0; i--) {
            for(int k = 0; k < N; k++){
                file << data[k][i][j] << " ";
            }
            file << endl;
        }
    }
    file.close();
}

void PLOT3Dwrite2D(string filename, const vector<vector<double>> &X, const vector<vector<double>> &Y){
    filename += ".X";
    ofstream file(filename);
    const int m = X.size(); const int n = X[0].size();
    // 输出块数
    file << 1 << endl;
    // 输出块的尺寸
    file << n <<" "<< m << " "<< 1 << endl;
    // 输出 X
    for(int i = m-1; i>= 0; i--){
        for(int j = 0; j < n; j++){
            file << X[i][j] << endl;
        }
    }
    // 输出 Y
    for(int i = m-1; i>= 0; i--){
        for(int j = 0; j < n; j++){
            file << Y[i][j] << endl;
        }
    }
    // 输出 Z
    for(int i = m-1; i>= 0; i--){
        for(int j = 0; j < n; j++){
            file << 0.0 << endl;
        }
    }
    file.close();
}


void ERROR_Handle(bool flag, string msg){
    if(!flag){
        std::cerr << msg << std::endl;
        exit(1);
    }
}

void Read_Blazek_grd(string filename, vector<vector<double>> &X, vector<vector<double>> &Y){
    // 读取文件
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        exit(1);
    }
    int I, J;   // 网格尺寸
    std::string line;
    // 跳过前三行注释
    getline(file, line);
    getline(file, line);
    getline(file, line);
    // 读取网格尺寸
    getline(file, line);
    istringstream iss(line);
    iss >> I >> J;
    ++I; ++J;
    cout << "I: "<<I<<", J: "<<J<<endl;
    // 跳过 coordinates (x, y):
    getline(file, line);
    // 
    X = vector<vector<double>>(J, vector<double>(I));
    Y = vector<vector<double>>(J, vector<double>(I));
    // 
    int K = 0,cnt = I*J;
    int i, j;
    double x, y;
    while(cnt--){
        j = K % I;
        i = J - (K/I) - 1;
        // 读取坐标
        getline(file, line);
        istringstream iss(line);
        iss >> x >> y;
        X[i][j] = x;
        Y[i][j] = y;
        K++;
    }
    file.close();
}

void Read_Blazek_FlowV2D(string filename, vector<vector<vector<double>>>& data, vector<string>& labels){
    // 读取文件
    std::ifstream file(filename, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        exit(1);
    }
    int N;      // 数据数量
    int I, J;   // 网格尺寸
    std::string line;
    // 跳过
    getline(file, line);
    getline(file, line);
    getline(file, line);
    // 读取数据数量
    getline(file, line);
    istringstream iss(line);
    cout << line<< endl;
    iss >> N >> N;
    cout << "Num of data: "<<N <<endl;
    // 读取labels
    labels = {};
    for(int i = 0; i < N; i++){
        getline(file, line);
        line = trim(line);
        labels.push_back(line);
    }
     // 读取网格尺寸
    getline(file, line);
    iss = istringstream(line);
    iss >> I >> J;
    cout << "I: "<<I<<", J: "<<J<<endl;
    //
    getline(file, line);
    getline(file, line);
    // 初始化 data
    data = vector<vector<vector<double>>>(N);
    for(int i =0; i < N; i++){
        data[i] = vector<vector<double>>(J, vector<double>(I));
    }
    //
    int K = 0,cnt = I*J;
    int i, j;
    while(cnt--){
        j = K % I;
        i = J - (K/I) - 1;
        // 读取坐标
        getline(file, line);
        iss = istringstream(line);
        for(int k = 0; k < N; k++){
            iss >> data[k][i][j];
        }
        K++;
    }
}
