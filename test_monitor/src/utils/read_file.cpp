#include "utils/read_file.h"

namespace monitor {
bool ReadFile::ReadLine(std::vector<std::string>* args) {
  std::string line;
  std::getline(ifs_, line);
  if (ifs_.eof() || line.empty()) {
    return false;
  }

//读单词
  std::istringstream line_ss(line);
//这里还可以用 getline(line_ss, word, ','), 指定分隔符
  while (!line_ss.eof()) {
    std::string word;
    line_ss >> word;
    args->push_back(word);
  }
  return true;
}


}  // namespace monitor