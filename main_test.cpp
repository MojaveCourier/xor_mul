#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // 添加头文件以使用 std::memcpy
#include "encoder.h"
#include <chrono>
#include <fstream>
#include <thread>

using namespace std;
using namespace std::chrono;

int main() {
    microseconds encoding_duration(0);

    std::ifstream file("temp\\random_data.bin", std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "无法打开文件" << std::endl;
        return 1;
    }

    

    int k, r;
    std::cin >> k >> r;
    //int group_size = k / r;
    


    std::streamsize size = file.tellg();

    int numBlocks = k; // 指定块数
    unsigned char **blocks = new unsigned char*[numBlocks];
    std::streamsize blockSize = size / numBlocks;

    unsigned char **parity_block = new unsigned char*[r];
    for(int i = 0; i < r; i++){
        parity_block[i] = new unsigned char[blockSize];
        std::memset(parity_block[i], 0, blockSize);
    }

    file.seekg(0, std::ios::beg);





    for(int i = 0; i < numBlocks; i++){
        std::cout << "读取文件块 " << i << std::endl;
        blocks[i] = new unsigned char[blockSize];
        if (!file.read(reinterpret_cast<char*>(blocks[i]), blockSize)) {
            std::cerr << "读取文件块失败" << std::endl;
            return 1;
        }
    }


    std::cout << "开始编码" << std::endl;
    for(int i = 0; i < 20; i++){
        cout << "开始第" << i + 1 << "次编码" << endl;
        high_resolution_clock::time_point start = high_resolution_clock::now();


        encode_rs1(k, r, blocks, parity_block, blockSize);

        high_resolution_clock::time_point end = high_resolution_clock::now();
        encoding_duration = duration_cast<microseconds>(end - start);
        cout << "编码时间: " << encoding_duration.count() << " 微秒" << endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "开始编码" << std::endl;
    for(int i = 0; i < 20; i++){
        cout << "开始第" << i + 1 << "次编码" << endl;
        high_resolution_clock::time_point start = high_resolution_clock::now();


        encode_rs2(k, r, blocks, parity_block, blockSize);

        high_resolution_clock::time_point end = high_resolution_clock::now();
        encoding_duration = duration_cast<microseconds>(end - start);
        cout << "编码时间: " << encoding_duration.count() << " 微秒" << endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    



    for (int i = 0; i < r; i++) {
        std::ofstream outFile("temp\\Parity" + std::to_string(i) + ".bin", std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "无法打开输出文件 parity_block_" << i << ".bin" << std::endl;
            return 1;
        }
        outFile.write(reinterpret_cast<char*>(parity_block[i]), blockSize);
        outFile.close();
    }

    // 释放内存

    for (int i = 0; i < numBlocks; ++i) {
        delete[] blocks[i];
    }
    delete[] blocks;

    /*unsigned char *res = new unsigned char[blockSize];
    for(int i = 0; i < 20; i++){
        cout << "开始第" << i + 1 << "次解码" << endl;
        high_resolution_clock::time_point start = high_resolution_clock::now();
        ECProject::gf_xor_128(res, parity_block[0]);

    }*/   

    return 0;
}


