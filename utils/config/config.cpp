#include "config.hpp"

namespace monitor{
std::ostream& operator<<(std::ostream &out, const monitor::ConfigVarBase &cvb){
    out << cvb.getName() << ": " << cvb.toString();
    return out;
}
}
