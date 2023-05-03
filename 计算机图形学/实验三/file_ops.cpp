//
// Created by Bhyin on 4/30/2023.
//

#include "file_ops.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

PlyModel::PlyModel() : vertex_num(0), face_num(0), vertices(nullptr), faces(nullptr) {
}

PlyModel::~PlyModel() {
    delete[]vertices;
    delete[]faces;
    vertices = nullptr;
    faces = nullptr;
}

void PlyModel::Load(const char *filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Model file does not exist!" << std::endl;
        exit(0);
    }

    // 文件头
    std::string line;
    bool end_header = false;
    while (!end_header && std::getline(file, line)) {
        std::istringstream iss(line);
        iss >> line;
        if (line == "element") {
            iss >> line;
            if (line == "vertex")iss >> vertex_num;
            else if (line == "face")iss >> face_num;
        } else if (line == "end_header")end_header = true;
    }

    // 顶点缓存和索引缓存
    vertices = new double[vertex_num * 6];
    faces = new unsigned int[face_num * 3];

    // 顶点以及法向
    for (int i = 0; i < vertex_num; ++i)
        file >> vertices[6 * i] >> vertices[6 * i + 1] >> vertices[6 * i + 2]
             >> vertices[6 * i + 3] >> vertices[6 * i + 4] >> vertices[6 * i + 5];
    int t;
    // 面片
    for (int i = 0; i < face_num; ++i)
        file >> t >> faces[3 * i] >> faces[3 * i + 1] >> faces[3 * i + 2];
}
