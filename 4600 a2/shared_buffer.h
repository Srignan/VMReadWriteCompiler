#ifndef SHARED_BUFFER_H
#define SHARED_BUFFER_H

#include <linux/mutex.h>

#define SHARED_MEMORY_SIZE 1024

extern struct mutex shared_memory_mutex;

ssize_t write_to_shared_buffer(const char *buffer, size_t length);
ssize_t read_from_shared_buffer(char *buffer, size_t length);

#endif
