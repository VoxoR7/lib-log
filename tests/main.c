#include <stdlib.h>

#include "log.h"

int main() {
    log_init(FATAL, "/tmp/test_log");

    log_thread_level(DEBUG);
    log_master_level(UNKNOW);
    log_master_level(10);
    log_master_level(INFO);

    LOG_MESG(FATAL, "This is a test log, no error occured %d '%s' %lu", 50, "this is a test string", 10UL);
    LOG_MESG(DEBUG, "This is a test log, no debug mesg to print : %s", "this is a test string");

    log_change_output("/tmp/test_log.output");
    log_change_output(NULL);
    log_change_output("/tmp/test_log.output");

    LOG_MESG(FATAL, "(file) This is a test log, no error occured %d '%s' %lu", 50, "this is a test string", 10UL);
    LOG_MESG(DEBUG, "(file) This is a test log, no debug mesg to print : %s", "this is a test string");

    log_change_output(NULL);

    exit(EXIT_SUCCESS);
}
