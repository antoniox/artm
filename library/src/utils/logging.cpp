#include <fcntl.h>

#include "utils/logging.h"


void init_logging(
    const std::string & program_name,
    const std::string & log_path
) {
    int log_fd = open(log_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

    dup2(log_fd, STDERR_FILENO);
    FLAGS_logtostderr = 1;

    google::InitGoogleLogging(program_name.c_str());
}
