#ifndef IOFPRB_MNTPOINT_H
#define IOFPRB_MNTPOINT_H
#include <string>

namespace fs {

struct mntpoint
{
    std::string dev;
    std::string mntroot;
    std::string fstype;
};

}


#endif //IOFPRB_MNTPOINT_H
