#include "hist.h"
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>

#define HIST_BUF_SIZE 256
#define HIST_DATA_FORMAT "%lf:%lf;"

namespace ambient {


void hist::clear()
{
    std::string full_name = base_path_ + file_name;
    int fd = open(full_name.c_str(), O_TRUNC);
    close(fd);
}

hist_t hist::load()
{
    std::string full_name = base_path_ + file_name;
    int fd = open(full_name.c_str(), O_RDONLY);
    if(fd == -1)
        return hist_t{};

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

    return std::make_pair(rd, wr);
}

void hist::save(const ambient::hist_t& data)
{
    std::string full_name = base_path_ + file_name;
    int fd = open(full_name.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if(fd == -1) return;

    assert(data.first.size() == data.second.size()
           || data.first.size() == data.second.size() + 1);

    char buf[HIST_BUF_SIZE];
    char* bufp = buf;
    size_t total = 0;
    for(size_t i=0; i<data.first.size(); i++)
    {
        int r = sprintf(bufp, HIST_DATA_FORMAT, data.first[i], data.second[i]);
        bufp += r;
        total += r;
    }
    write(fd, buf, total);
    close(fd);
}


} // ambient