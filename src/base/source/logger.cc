#include "logger.hh"

log_stream::t_ptr_log log_stream::logger_ = spdlog::default_logger();

log_stream::log_stream(log_stream::t_lvl lvl)
    : lvl_(lvl)
    , buffer_()
{
}

log_stream::~log_stream()
{
    log_stream::logger_->log(lvl_, buffer_.str());
}