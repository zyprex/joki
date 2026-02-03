#ifdef DEBUG

#ifndef LOG
#define LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

#else

#ifndef LOG
#define LOG(fmt, ...)
#endif

#endif
