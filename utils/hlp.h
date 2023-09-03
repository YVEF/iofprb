#ifndef IOFPRB_HLP_H
#define IOFPRB_HLP_H
#include <string>
#include <cmath>
#include "../utils/typedefs.h"

void to_ascii_upper_case(std::string& str);
std::string to_pretty_size(ull bytes);
std::string get_tmp_file_name();

namespace hlp::internal
{
template<unsigned... digits>
struct to_chars { static const char value[]; };

template<unsigned... digits>
const char to_chars<digits...>::value[] = {('0' + digits)..., 0};

template<unsigned rem, unsigned... digits>
struct explode : explode<rem / 10, rem % 10, digits...> {};

template<unsigned... digits>
struct explode<0, digits...> : to_chars<digits...> {};
}

template<unsigned num>
struct num_to_string : hlp::internal::explode<num / 10, num % 10> {};

#endif //IOFPRB_HLP_H
