#ifndef LOG_H
#define LOG_H

#include <string.h>

#ifdef __linux__
    #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#elifdef _WIN32
    #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #error "This lib is not designed to be compiled on this system !"
#endif

/**
 * The different log level usable.
 * Do not use UNKNOW, as it will fail with any function
 */
enum log_level {UNKNOW, NONE, FATAL, WARN, INFO, DEBUG};

/* DO NOT USE THIS FUNCTION, use the macro LOG_MESG instead */
void log_print(enum log_level level, char *file, int line, char *msg, ...);

/**
 * @brief Print the msg with the arguments passed as variadic parameters.
 * 
 * @param level The level of the message (log_level)
 * @param msg The format message to print
 * @param ... The arguments of the format string
 * 
 * You should call log_init before any call to this macro or function.
 * This macro is vulnerable to all the known attack on the printf family such as format srting bug.
 */
#define LOG_MESG(level, msg, ...) log_print(level, __FILENAME__, __LINE__, msg __VA_OPT__(,) __VA_ARGS__)

/**
 * @brief Initialize the lib log.
 * 
 * The default level of log is INFO.
 */
void log_init(enum log_level master_level, char *dir_path);

/**
 * @brief Modify the master level of the log used.
 * 
 * @param level the log level asked to be print
 * 
 * Any thread created after this call will be initialized with this level.
 * Any log comming will only be printed if it has a equal or more critical importance than the log asked.
 * FATAL < (more critic than) WARN < INFO < DEBUG
 * By default, the master level is INFO. This log FATAL, WARN and INFO will be printed but NOT DEBUG log.
 * 
 * @warning The effect will only be applied when a log is asked to be print (wheter it is going to be print or not).
 *          At this moment (when the first log from the thread is aked to be print), it will take the CURRENT master
 *          log level for reference.
 * 
 * The effect is NOT applied to already running thread. This means there is no way to change the log level of the main
 * thread with this function. You will need to use log_thread_level.
 */
void log_master_level(enum log_level level);

/**
 * @brief Modify the thread level of the log used.
 * 
 * @param level the log level asked to be print
 * 
 * The current thread will only print log that has a equal or more critical importance than the log asked.
 * FATAL < (more critic than) WARN < INFO < DEBUG
 * By default, the thread level is initialized with the master level when the first log is asked to be print,
 * wheter it is going to be print or not.
 */
void log_thread_level(enum log_level level);

#endif
