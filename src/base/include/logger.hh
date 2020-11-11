#ifndef __H_LOGGER__
#define __H_LOGGER__

#include <sstream>

// #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
// #include "spdlog/sinks/stdout_sinks.h"
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

#include <string>

class log_stream
{
public:
    typedef std::shared_ptr<spdlog::logger> t_ptr_log;
    typedef spdlog::level::level_enum       t_lvl;

    log_stream(t_lvl lvl, std::string file, int line);
    ~log_stream();

    template <typename T>
    log_stream& operator<<(T const& value)
    {
        buffer_ << value;
        return *this;
    }

protected:
    static t_ptr_log logger_;

private:
    std::stringstream buffer_;
    int               line_;
    std::string       file_;
    t_lvl             lvl_;
};

#define DEBUG_MODE  spdlog::set_level(spdlog::level::debug)     
#define TRACE_MODE  spdlog::set_level(spdlog::level::trace)     


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define ERR log_stream(spdlog::level::err, __FILENAME__, __LINE__)
#define INFO log_stream(spdlog::level::info, __FILENAME__, __LINE__)
#define TRACE log_stream(spdlog::level::trace, __FILENAME__, __LINE__)
#define DEBUG log_stream(spdlog::level::debug, __FILENAME__, __LINE__)

#endif // !__H_LOGGER__
