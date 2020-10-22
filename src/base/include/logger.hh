#ifndef __H_LOGGER__
#define __H_LOGGER__

#include <sstream>

#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

class log_stream
{
public:
    typedef std::shared_ptr<spdlog::logger> t_ptr_log;
    typedef spdlog::level::level_enum       t_lvl;

    log_stream(t_lvl lvl);
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
};


#define DEBUG_MODE spdlog::set_level(spdlog::level::trace)

#define INFO  log_stream(spdlog::level::info)
#define TRACE log_stream(spdlog::level::trace)
#define DEBUG log_stream(spdlog::level::debug)

#endif // !__H_LOGGER__
