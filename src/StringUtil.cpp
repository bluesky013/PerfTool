#include <StringUtil.h>

namespace perf {

    std::vector<std::string> SplitString(const std::string& str, char *seperator)
    {
        std::vector<std::string> output;
        std::string::size_type pPos = 0, pos = 0;
        while((pos = str.find_first_of(std::string(seperator), pos)) != std::string::npos) {
            if (pos > pPos) {
                std::string substring(str.substr(pPos, pos - pPos));
                output.push_back(substring);
            }
            pPos = ++pos;
        }

        if (pPos <= str.length()) {
            output.push_back(str.substr(pPos, std::string::npos));
        }
        return output;
    }

}
