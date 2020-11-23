#include "logger.hh"

log_stream::t_ptr_log log_stream::logger_ = spdlog::default_logger();

log_stream::log_stream(t_lvl           lvl,
                       source_location loc)
    : lvl_(lvl)
    , buffer_()
{
    std::string file_line = fmt::format(
        "{}:{}", strrchr(loc.file_name(), '/'), loc.line());

    buffer_ << fmt::format(
        "[{:}] [{:}] ", file_line, loc.function_name());
}

log_stream::~log_stream()
{
    log_stream::logger_->log(lvl_, buffer_.str());
}