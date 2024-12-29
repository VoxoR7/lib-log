#include <stdlib.h>

#include "log.h"

int main() {
    log_init(LOG_FATAL, "/tmp/test_log");

    log_thread_level(LOG_DEBUG);
    log_master_level(LOG_UNKNOW);
    log_master_level(10);
    log_master_level(LOG_INFO);

    LOG_MESG(LOG_FATAL, "This is a test log, no error occured %d '%s' %lu", 50, "this is a test string", 10UL);
    LOG_MESG(LOG_DEBUG, "This is a test log, no debug mesg to print : %s", "this is a test string");

    log_change_output("/tmp/test_log.output");
    log_change_output(NULL);
    log_change_output("/tmp/test_log.output");

    LOG_MESG(LOG_FATAL, "(file) This is a test log, no error occured %d '%s' %lu", 50, "this is a test string", 10UL);
    LOG_MESG(LOG_DEBUG, "(file) This is a test log, no debug mesg to print : %s", "this is a test string");

    log_change_output(NULL);

    exit(EXIT_SUCCESS);
}
