#ifndef IOFPRB_HIST_H
#define IOFPRB_HIST_H
#include "config_state.h"
#include <vector>
#include <string>

#define HIST_FILENAME ".iofprbhist"
namespace ambient {

typedef std::vector<double> read_thr_t;
typedef std::vector<double> write_thr_t;
typedef struct
{
    read_thr_t read;
    write_thr_t write;
} hist_t;;

class hist
{
public:
    explicit hist(std::string base_path) noexcept
    : file_name(HIST_FILENAME), base_path_(std::move(base_path)) {}

    void clear();
    hist_t load();
    void save(const ambient::read_thr_t& rd_data, const ambient::write_thr_t& wr_data);
    const std::string file_name;

private:
    std::string base_path_;

};

} // ambient

#endif //IOFPRB_HIST_H
