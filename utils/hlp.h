#ifndef IOFPRB_HLP_H
#define IOFPRB_HLP_H
#include <string>

inline void to_ascii_upper_case(std::string& str)
{
    for(char& c : str)
    {
        int i = static_cast<int>(c);
        if(i >= 97 && i <= 122)
            c = static_cast<char>(i-32);
    }
}

#endif //IOFPRB_HLP_H
