#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace monitor {
class ReadFile {
 public:

 //接受一个文件名， 构建ifs——
  explicit ReadFile(const std::string& name) : ifs_(name) {}
  ~ReadFile() { ifs_.close(); }

  //将一行数据按照 空格分开， 得到 string 数组
  bool ReadLine(std::vector<std::string>* args);
  static std::vector<std::string> GetStatsLines(const std::string& stat_file,
                                                const int line_count) {
    //将一个大文件，按行分割
    std::vector<std::string> stats_lines;
    std::ifstream buffer(stat_file);
    for (int line_num = 0; line_num < line_count; ++line_num) {
      std::string line;
      std::getline(buffer, line);
      if (line.empty()) {
        break;
      }
      stats_lines.push_back(line);
    }
    return stats_lines;
  }

 private:
  std::ifstream ifs_;
};
}  // namespace monitor
