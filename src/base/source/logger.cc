#include "logger.hh"

log_stream::t_ptr_log log_stream::logger_ = spdlog::default_logger();

log_stream::log_stream(t_lvl lvl, std::string file, int line)
    : lvl_(lvl)
    , buffer_()
    , file_(file)
    , line_(line)
{
    // if (lvl == t_lvl::debug or lvl == t_lvl::trace)
    buffer_ << fmt::format(
        "[{:-<25} #{:>3}] ", file_ + " ", line_);
}

log_stream::~log_stream()
{
    log_stream::logger_->log(lvl_, buffer_.str());
}