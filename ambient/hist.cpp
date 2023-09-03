#include "hist.h"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <iostream>

#define HIST_BUF_SIZE 256
#define HIST_DATA_FORMAT "%lf:%lf;"

namespace ambient {


void hist::clear()
{
    std::string full_name = base_path_ + "/" + file_name;
    int fd = open(full_name.c_str(), O_TRUNC);
    if(fd != -1) close(fd);
}

hist_t hist::load()
{
    std::string full_name = base_path_ + "/" + file_name;
    int fd = open(full_name.c_str(), O_RDONLY);
    if(fd == -1)
        return hist_t{read_thr_t(1, 0.0), write_thr_t(1, 0.0)};

    char buf[HIST_BUF_SIZE];
    int total = read(fd, buf, HIST_BUF_SIZE);
    if(total <= 0)
        return hist_t{};

    read_thr_t rd;
    write_thr_t wr;
    char* bufp = buf;
    while(total > 0)
    {
        double rdv, wrv;
        int r = sscanf(bufp, HIST_DATA_FORMAT, &rdv, &wrv);
        rd.push_back(rdv), wr.push_back(wrv);
        bufp += r;
        total -= r;
    }
    close(fd);

    return hist_t{rd, wr};
}

void hist::save(const ambient::read_thr_t& rd_data, const ambient::write_thr_t& wr_data)
{
    assert(rd_data.size() == wr_data.size()
           || rd_data.size() == wr_data.size() + 1);

    std::string full_name = base_path_ + "/" + file_name;
    int fd = open(full_name.c_str(),
                  O_RDWR | O_CREAT | O_APPEND,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(fd == -1) return;

    char buf[HIST_BUF_SIZE];
    char* bufp = buf;
    std::size_t total = 0;
    for(std::size_t i=0; i<rd_data.size(); i++)
    {
        int r = sprintf(bufp, HIST_DATA_FORMAT, rd_data[i], (i >= wr_data.size()) ? 0.0 : wr_data[i]);
        bufp += r;
        total += r;
    }
    write(fd, buf, total);
    close(fd);
}


} // ambient