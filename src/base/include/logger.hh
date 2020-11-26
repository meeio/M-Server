#ifndef __H_LOGGER__
#define __H_LOGGER__

#include <sstream>

// #define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
// #include "spdlog/sinks/stdout_sinks.h"
#include <experimental/source_location>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>
#include <string>

using std::experimental::source_location;

class log_stream
{
public:
    typedef std::shared_ptr<spdlog::logger> t_ptr_log;
    typedef spdlog::level::level_enum       t_lvl;

    log_stream(t_lvl lvl, source_location loc = source_location::current());
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
    t_lvl             lvl_;
    std::string       file_name_;
};

#define DEBUG_MODE spdlog::set_level(spdlog::level::debug)
#define TRACE_MODE spdlog::set_level(spdlog::level::trace)

#define ERR   log_stream(spdlog::level::err)
#define INFO  log_stream(spdlog::level::info)
#define TRACE log_stream(spdlog::level::trace)
#define DEBUG log_stream(spdlog::level::debug)
#define WARN  log_stream(spdlog::level::warn)

#endif // !__H_LOGGER__
