#include "hlp.h"


static std::string round_size(double size)
{
    int a = static_cast<int>(size);
    int b = static_cast<int>((size-static_cast<double>(a))*100.0);
    return std::string(std::to_string(a) + "." + std::to_string(b));
}

std::string to_pretty_size(ull bytes)
{
    auto kb = static_cast<double>(bytes)/1024.0;
    auto mb = kb/1024.0;
    auto gb = mb/1024.0;
    if(gb >= 1.0)
        return round_size(gb) + "Gb";
    if(mb >= 1.0)
        return round_size(mb) + "Mb";
    if(kb >= 1.0)
        return round_size(kb) + "Kb";

    return std::to_string(bytes) + "B";
}


void to_ascii_upper_case(std::string& str)
{
    for(char& c : str)
    {
        int i = static_cast<int>(c);
        if(i >= 97 && i <= 122)
            c = static_cast<char>(i-32);
    }
}