#ifndef LOGGER_CLIENT_H
#define LOGGER_CLIENT_H

int  log_init(void);
void log_msg(const char *level, const char *fmt, ...);
void log_close(void);

#endif
