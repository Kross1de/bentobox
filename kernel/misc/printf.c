#include <stdarg.h>
#include <stdint.h>
#include <kernel/fd.h>
#include <kernel/vfs.h>
#include <kernel/printf.h>
#include <kernel/string.h>

int hex_length(uint64_t val) {
    int len = 0;
    do {
        len++; val >>= 4;
    } while (val != 0);
    return len;
}

void parse_num(char *s, int *ptr, int64_t val, uint32_t base, bool is_signed) {
    if (is_signed && val < 0) {
        s[(*ptr)++] = '-';
        val = -val;
    }
    uint64_t n = (uint64_t)val / base;
    int r = (uint64_t)val % base;
    if (val >= (int64_t)base) {
        parse_num(s, ptr, n, base, is_signed);
    }
    s[(*ptr)++] = (r + '0');
}

void parse_hex(char *s, int *ptr, uint64_t val, int i) {
    if (!i) {
        i = hex_length(val);
    }
    while (i-- > 0) {
        s[(*ptr)++] = "0123456789abcdef"[val >> (i * 4) & 0x0F];
    }
}

void parse_string(char *s, int *ptr, char *str) {
    if (!str) {
        memcpy(s + *ptr, "(null)", 6);
        *ptr += 6;
        return;
    }
    strcpy(s + *ptr, str);
    *ptr += strlen(str);
}

int vsprintf(char *s, const char *fmt, va_list args) {
    int ptr = 0;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;

            bool is_long = (*fmt == 'l') ? (fmt++, true) : false;

            switch (*fmt) {
                case 'u':
                    parse_num(s, &ptr, is_long ? va_arg(args, long) : va_arg(args, int), 10, false);
                    break;
                case 'd':
                    parse_num(s, &ptr, is_long ? va_arg(args, long) : va_arg(args, int), 10, true);
                    break;
                case 'x':
                    parse_hex(s, &ptr, is_long ? va_arg(args, uint64_t) : va_arg(args, uint32_t), 0);
                    break;
                case 'p':
                    parse_hex(s, &ptr, va_arg(args, uint64_t), 16);
                    break;
                case 's':
                    parse_string(s, &ptr, va_arg(args, char *));
                    break;
                case 'c':
                    s[ptr++] = (char)va_arg(args, int);
                    break;
            }
        } else {
            s[ptr++] = *fmt;
        }
        fmt++;
    }

    return 0;
}

int vprintf(const char *fmt, va_list args) {
    char buf[1024] = {-1};
    int ret = vsprintf(buf, fmt, args);
    
    puts(buf);
    return ret;
}

int sprintf(char *str, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[1024] = {0};
    int ret = vsprintf(buf, fmt, args);
    strcpy(str, buf);
    va_end(args);

    return ret;
}

int fprintf(int stream, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[1024] = {0};
    int ret = vsprintf(buf, fmt, args);
    va_end(args);

    vfs_write(this_core()->current_proc->fd_table[stream].node, buf, 0, strlen(buf));

    return ret;
}

char *fgets(char *str, int n, int stream) {
    int i = 0;
    while (i < n) {
        vfs_read(this_core()->current_proc->fd_table[stream].node, str + i, 0, 1);

        switch (str[i]) {
            case '\0':
                break;
            case '\n':
            case '\r':
                fprintf(stdout, "\n");
                str[i] = '\0';
                return str;
            case '\b':
            case 127:
                if (i > 0) {
                    fprintf(stdout, "\b \b");
                    i--;
                }
                break;
            default:
                fprintf(stdout, "%c", str[i]);
                i++;
                break;
        }
    }

    return str;
}

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buf[1024] = {0};
    int ret = vsprintf(buf, fmt, args);
    puts(buf);
    va_end(args);

    return ret;
}