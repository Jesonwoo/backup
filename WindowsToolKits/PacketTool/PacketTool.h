#pragma once
#include <string>
#include <vector>
#define DEBUG_LOG(...) printf("\n" __VA_ARGS__)

#define REGISTER_INFO  "Register_Info"
#define ROOTDIR        "$(SolutionDir)"

// 配置文件中的关键字
#define DESTINATION    "dest"
#define SOURCE         "src"
#define FILE           "file"

void searchCatalog(std::string& root,std::vector<std::string>& fileNames);