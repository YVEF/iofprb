#ifndef IOFPRB_HLP_H
#define IOFPRB_HLP_H
#include <string>
#include <cmath>
#include "../utils/typedefs.h"

void to_ascii_upper_case(std::string& str);
std::string to_pretty_size(ull bytes);
std::string get_tmp_file_name();

#endif //IOFPRB_HLP_H
