#include <linux/module.h> // Add this include for MODULE_LICENSE and EXPORT_SYMBOL
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include "shared_buffer.h"

MODULE_LICENSE("GPL");

static char shared_memory[SHARED_MEMORY_SIZE] = {0};
static int buffer_position = 0;
DEFINE_MUTEX(shared_memory_mutex);

ssize_t write_to_shared_buffer(const char *buffer, size_t length)
{
    size_t bytes_to_write;

    if (length > SHARED_MEMORY_SIZE - buffer_position) {
        bytes_to_write = SHARED_MEMORY_SIZE - buffer_position;
        pr_info("shared_buffer: not enough space left in the buffer, dropping the rest\n");
        if (bytes_to_write == 0) {
            pr_info("shared_buffer: buffer is full\n");
        }
    } else {
        bytes_to_write = length;
    }

    memcpy(shared_memory + buffer_position, buffer, bytes_to_write);
    buffer_position += bytes_to_write;
    pr_info("shared_buffer: writing %zu bytes\n", bytes_to_write);

    return bytes_to_write;
}

ssize_t read_from_shared_buffer(char *buffer, size_t length)
{
    size_t bytes_to_read;

    if (length > buffer_position) {
        bytes_to_read = buffer_position;
    } else {
        bytes_to_read = length;
    }

    if (bytes_to_read == 0) {
        pr_info("shared_buffer: buffer is empty\n");
    }

    memcpy(buffer, shared_memory, bytes_to_read);
    memmove(shared_memory, shared_memory + bytes_to_read, buffer_position - bytes_to_read);
    buffer_position -= bytes_to_read;
    pr_info("shared_buffer: reading %zu bytes\n", bytes_to_read);

    return bytes_to_read;
}

EXPORT_SYMBOL(shared_memory_mutex);
EXPORT_SYMBOL(write_to_shared_buffer);
EXPORT_SYMBOL(read_from_shared_buffer);
