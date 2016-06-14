#pragma once

#include <string>

#include <glog/logging.h>

void init_logging(
    const std::string & program_name,
    const std::string & log_path
);

#define CUSTOM_LOG(severity) google::LogMessage( \
    __FILE__, __LINE__, google::GLOG_ ## severity).stream()


#define CUSTOM_SOME_KIND_OF_LOG_EVERY_N(severity, n, what_to_do) \
  static int LOG_OCCURRENCES = 0, LOG_OCCURRENCES_MOD_N = 0; \
  ++LOG_OCCURRENCES; \
  if (++LOG_OCCURRENCES_MOD_N > n) LOG_OCCURRENCES_MOD_N -= n; \
  if (LOG_OCCURRENCES_MOD_N == 1) \
    google::LogMessage( \
        __FILE__, __LINE__, google::GLOG_ ## severity, LOG_OCCURRENCES, \
        &what_to_do).stream()


#define CUSTOM_LOG_EVERY_N(severity, n)                                        \
  GOOGLE_GLOG_COMPILE_ASSERT(google::GLOG_ ## severity < \
                             google::NUM_SEVERITIES,     \
                             INVALID_REQUESTED_LOG_SEVERITY);           \
  CUSTOM_SOME_KIND_OF_LOG_EVERY_N(severity, (n), google::LogMessage::SendToLog)


#define LOG_INFO \
    CUSTOM_LOG(INFO)


#define LOG_INFO_EVERY_N(counter, message) \
    CUSTOM_LOG_EVERY_N(INFO, counter) << "Processed " << \
    google::COUNTER << " " << message << std::endl

