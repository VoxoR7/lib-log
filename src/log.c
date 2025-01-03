#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "log.h"

#include "color.h"

_Atomic enum log_level log_mesg_master = LOG_INFO;
thread_local enum log_level log_mesg_thread = LOG_UNKNOW;

FILE *output = NULL;

FILE *file_debug = NULL;
FILE *file_info = NULL;
FILE *file_warn = NULL;
FILE *file_fatal = NULL;

static void log_exit(void) {
    LOG_MESG(LOG_INFO, "Exiting lib log");

    if (file_debug != NULL)
        fclose(file_debug);

    if (file_info != NULL)
        fclose(file_info);

    if (file_warn != NULL)
        fclose(file_warn);

    if (file_fatal != NULL)
        fclose(file_fatal);

    if (output != NULL && output != stdout)
        fclose(output);
}

static void create_log_directory(char *dir_path) {
    errno = 0;

    #ifdef __linux__
        if (mkdir(dir_path, S_IRWXU | S_IRGRP | S_IROTH) == -1) {
    #elifdef _WIN32
        if (mkdir(dir_path) == -1) {
    #else
        #error "This lib is not designed to be compiled on this system !"
    #endif
        switch (errno) {
            case EACCES:
                LOG_MESG(LOG_WARN, "Search permission is denied on a component of the path prefix, or write permission is denied on the parent directory of the directory to be created");
                return;
            case EEXIST:
                LOG_MESG(LOG_INFO, "The named path exists: %s", dir_path);
                break;
            case ELOOP:
                LOG_MESG(LOG_WARN, "A loop exists in symbolic links encountered during resolution of the path argument");
                return;
            case ENOENT:
                LOG_MESG(LOG_WARN, "A component of the path prefix specified by path does not name an existing directory or path is an empty string");
                return;
            case ENOSPC:
                LOG_MESG(LOG_WARN, "The file system does not contain enough space to hold the contents of the new directory or to extend the parent directory of the new directory");
                return;
            case ENOTDIR:
                LOG_MESG(LOG_WARN, "A component of the path prefix is not a directory");
                return;
            default:
                LOG_MESG(LOG_WARN, "Unknow error ocurred when creating directory");
                return;
        }
    }

    char *file_name;

    asprintf(&file_name, "%s/log.debug", dir_path);
    file_debug = fopen(file_name, "w");
    if (file_debug == NULL) {
        LOG_MESG(LOG_WARN, "Error when creating %s: %s", file_name, strerror(errno));
    }
    free(file_name);

    asprintf(&file_name, "%s/log.info", dir_path);
    file_info = fopen(file_name, "w");
    if (file_info == NULL) {
        LOG_MESG(LOG_WARN, "Error when creating %s: %s", file_name, strerror(errno));
    }
    free(file_name);

    asprintf(&file_name, "%s/log.warn", dir_path);
    file_warn = fopen(file_name, "w");
    if (file_warn == NULL) {
        LOG_MESG(LOG_WARN, "Error when creating %s: %s", file_name, strerror(errno));
    }
    free(file_name);

    asprintf(&file_name, "%s/log.fatal", dir_path);
    file_fatal = fopen(file_name, "w");
    if (file_fatal == NULL) {
        LOG_MESG(LOG_WARN, "Error when creating %s: %s", file_name, strerror(errno));
    }
    free(file_name);
}

void log_init(enum log_level master_level, char *dir_path) {
    output = stdout;
    log_master_level(master_level);
    LOG_MESG(LOG_INFO, "Initializing lib log ...");

    atexit(log_exit);

    if (dir_path != NULL) {
        create_log_directory(dir_path);
    }

    LOG_MESG(LOG_INFO, "Initialization done");
}

void log_change_output(char *file_path) {
    if (file_path == NULL) {
        if (output != NULL && output != stdout)
            fclose(output);

        output = stdout;
        return;
    }

    FILE *old = output;

    output = fopen(file_path, "w");
    if (file_debug == NULL) {
        LOG_MESG(LOG_WARN, "Error when creating %s: %s", file_path, strerror(errno));
        LOG_MESG(LOG_WARN, "No change has been made");
        output = old;
        return;
    }

    if (old != NULL && old != stdout)
        fclose(old);
}

static void check_thread_level_defined(void) {
    if (log_mesg_thread == LOG_UNKNOW)
        log_mesg_thread = log_mesg_master;
}

static size_t get_time(char *buf) {
    time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    char s[64];
    return strftime(buf, sizeof(s), "%Y %b %d %H:%M:%S", &tm);
}

static void print_log(enum log_level level, char *msg_base, char *msg_sent) {
    char *msg;
    asprintf(&msg, "%s%s\n", msg_base, msg_sent);

    switch (level) {
        case LOG_DEBUG:
            if (file_debug != NULL)
                fwrite(msg, 1, strlen(msg), file_debug);
            if (log_mesg_thread >= LOG_DEBUG)
                fprintf(output, msg);
            break;
        case LOG_INFO:
            if (file_info != NULL)
                fwrite(msg, 1, strlen(msg), file_info);
            if (log_mesg_thread >= LOG_INFO)
                fprintf(output, msg);
            break;
        case LOG_WARN:
            if (file_warn != NULL)
                fwrite(msg, 1, strlen(msg), file_warn);
            if (log_mesg_thread >= LOG_WARN)
                fprintf(output, msg);
            break;
        case LOG_FATAL:
            if (file_fatal != NULL)
                fwrite(msg, 1, strlen(msg), file_fatal);
            if (log_mesg_thread >= LOG_FATAL)
                fprintf(output, msg);
            break;
        default:
            abort();
    }

    free(msg);
}

static void log_debug(char *file, int line, char *msg, va_list args) {
    char buf[256];
    if (get_time(buf) == 0)
        buf[0] = '\0';

    color_blue();
    color_bold();

    char *msg_base, *msg_sent;
    asprintf(&msg_base, "[DEBUG] %s %s/%d: ", buf, file, line);
    vasprintf(&msg_sent, msg, args);

    print_log(LOG_DEBUG, msg_base, msg_sent);

    free(msg_base);
    free(msg_sent);

    color_reset();
}

static void log_info(char *file, int line, char *msg, va_list args) {
    char buf[256];
    if (get_time(buf) == 0)
        buf[0] = '\0';

    color_bold();

    char *msg_base, *msg_sent;
    asprintf(&msg_base, "[INFO ] %s %s/%d: ", buf, file, line);
    vasprintf(&msg_sent, msg, args);

    print_log(LOG_INFO, msg_base, msg_sent);

    free(msg_base);
    free(msg_sent);

    color_reset();
}

static void log_warn(char *file, int line, char *msg, va_list args) {
    char buf[256];
    if (get_time(buf) == 0)
        buf[0] = '\0';

    color_yellow();
    color_bold();

    char *msg_base, *msg_sent;
    asprintf(&msg_base, "[WARN ] %s %s/%d: ", buf, file, line);
    vasprintf(&msg_sent, msg, args);

    print_log(LOG_WARN, msg_base, msg_sent);

    free(msg_base);
    free(msg_sent);

    color_reset();
}

static void log_fatal(char *file, int line, char *msg, va_list args) {
    char buf[256];
    if (get_time(buf) == 0)
        buf[0] = '\0';

    color_red();
    color_bold();

    char *msg_base, *msg_sent;
    asprintf(&msg_base, "[FATAL] %s %s/%d: ", buf, file, line);
    vasprintf(&msg_sent, msg, args);

    print_log(LOG_FATAL, msg_base, msg_sent);

    free(msg_base);
    free(msg_sent);

    color_reset();
}

void log_print(enum log_level level, char *file, int line, char *msg, ...) {
    check_thread_level_defined();

    va_list args;
    va_start(args, msg);

    switch (level) {
        case LOG_UNKNOW:
                log_print(LOG_WARN, FILENAME, __LINE__, "Can't print log with log level to UNKNOW");
            break;
        case LOG_DEBUG:
            log_debug(file, line, msg, args);
            break;
        case LOG_INFO:
            log_info(file, line, msg, args);
            break;
        case LOG_WARN:
            log_warn(file, line, msg, args);
            break;
        case LOG_FATAL:
            log_fatal(file, line, msg, args);
            break;
        default:
            LOG_MESG(LOG_WARN, "The log level used is not defined");
            break;
    }

    va_end(args);
}

void log_master_level(enum log_level level) {
    if (level == LOG_UNKNOW) {
        LOG_MESG(LOG_WARN, "Can't set thread log level to UNKNOW");
        return;
    }

    if (level > LOG_DEBUG) {
        LOG_MESG(LOG_WARN, "Can't set this log level : %d", level);
        return;
    }

    log_mesg_master = level;
}

void log_thread_level(enum log_level level) {
    if (level == LOG_UNKNOW) {
        LOG_MESG(LOG_WARN, "Can't set thread log level to UNKNOW");
        return;
    }

    if (level > LOG_DEBUG) {
        LOG_MESG(LOG_WARN, "Can't set this log level : %d", level);
        return;
    }

    log_mesg_thread = level;
}
