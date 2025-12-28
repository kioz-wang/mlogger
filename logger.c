/**
 * @file logger.c
 * @author kioz.wang
 * @brief
 * @version 1.0
 * @date 2025-10-09
 *
 * @copyright MIT License
 *
 *  Copyright (c) 2025 kioz.wang
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "logger.h"
#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const char *level_names[MLOG_DEBG + 1] = {
    "ERRO", "WARN", "INFO", "VERB", "DEBG",
};

mlog_level_t mlog_level_parse(const char *s) {
    for (int32_t i = 0; i < MLOG_DEBG + 1; i++) {
        if (!strcmp(s, level_names[i])) {
            return i;
        }
    }
    if (!strcmp(s, "ERROR")) return MLOG_ERROR;
    if (!strcmp(s, "DEBUG")) return MLOG_DEBUG;

    int32_t       value;
    char         *endptr = NULL;
    unsigned long parsed = strtoul(s, &endptr, 0);
    if (s == endptr || parsed == ULONG_MAX) {
        value = MLOG_DEBG;
    } else {
        value = (parsed > MLOG_DEBG) ? MLOG_DEBG : (int32_t)parsed;
    }
    return value;
}

static bool mlog_level_parse_env(const char *envname, mlog_level_t *level) {
    if (envname) {
        const char *level_s = getenv(envname);
        if (level_s && level_s[0]) {
            *level = mlog_level_parse(level_s);
            return true;
        }
    }
    return false;
}

void mlog_init(mlogger_t *mlogger, const char *envname, const char *envname_stderr, uint8_t fmt_cfg,
               uint8_t fmt_cfg_stderr) {
    mlogger->level = MLOG_ERRO;
    mlogger->f     = NULL;
    mlog_level_parse_env(envname, &mlogger->level);
    mlogger->enable_stderr  = mlog_level_parse_env(envname_stderr, &mlogger->level_stderr);
    mlogger->fmt_cfg        = fmt_cfg;
    mlogger->fmt_cfg_stderr = fmt_cfg_stderr;
}

void mlog_set_logger(mlogger_t *mlogger, mlog_level_t level, mlog_f f) {
    mlogger->level = level > MLOG_DEBG ? MLOG_DEBG : level;
    if (f) mlogger->f = f;
}

void mlog_enable_stderr(mlogger_t *mlogger, mlog_level_t level) {
    mlogger->level_stderr  = level > MLOG_DEBG ? MLOG_DEBG : level;
    mlogger->enable_stderr = true;
}

#ifndef MLOGGER_FMT_MAX
#define MLOGGER_FMT_MAX (1024)
#endif /* MLOGGER_FMT_MAX */

#if MLOGGER_FMT_MAX < 32
#error MLOGGER_FMT_MAX too small
#endif

static const char *colors[MLOG_DEBG + 1] = {
    "\e[31m",               /* 红 */
    "\e[33m",               /* 黄 */
    "\e[32m",               /* 绿 */
    "\e[38;2;100;150;100m", /* 浅绿 */
    "\e[90m",               /* 灰 */
};
#define color_reset "\e[0m"
#define color_bold  "\e[1m"

static const char *level_heads[MLOG_DEBG + 1] = {
    "[E]", "[W]", "[I]", "[V]", "[D]",
};

static int64_t timestamp(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000L + ts.tv_nsec / 1000L;
}

static void mlog_format(uint8_t cfg, mlog_level_t level, char *restrict __s, size_t __maxlen,
                        const char *restrict __format, __gnuc_va_list __arg) {
    int pos = 0;

    if (cfg & MLOG_FMT_NEWLINE) __maxlen--;
    if (cfg & MLOG_FMT_COLOR) __maxlen -= sizeof(color_reset) - 1;

    if (cfg & MLOG_FMT_TIMESTAMP) pos += snprintf(&__s[pos], __maxlen - pos, "[%lx]", timestamp());
    if (cfg & MLOG_FMT_COLOR) pos += snprintf(&__s[pos], __maxlen - pos, "%s", colors[level]);
    if (cfg & MLOG_FMT_LEVEL_HEAD) pos += snprintf(&__s[pos], __maxlen - pos, "%s", level_heads[level]);
    if (cfg & MLOG_FMT_COLOR && level == MLOG_ERRO) pos += snprintf(&__s[pos], __maxlen - pos, "%s", color_bold);

    assert(__maxlen - pos);
    int n = vsnprintf(&__s[pos], __maxlen - pos, __format, __arg);
    if (n < __maxlen - pos) pos += n;
    else pos = __maxlen;

    if (cfg & MLOG_FMT_COLOR) pos += sprintf(&__s[pos], "%s", color_reset);
    if (cfg & MLOG_FMT_NEWLINE) sprintf(&__s[pos], "%c", '\n');
}

void mlogf(mlogger_t *mlogger, mlog_level_t level, const char *fmt, ...) {
    char line[MLOGGER_FMT_MAX] = {0};

    if ((mlogger->enable_stderr && level <= mlogger->level_stderr)) {
        va_list args;
        va_start(args, fmt);
        mlog_format(mlogger->fmt_cfg_stderr, level, line, sizeof(line), fmt, args);
        va_end(args);
        fputs(line, stderr);
    }
    if (mlogger->f && level <= mlogger->level) {
        va_list args;
        va_start(args, fmt);
        mlog_format(mlogger->fmt_cfg, level, line, sizeof(line), fmt, args);
        va_end(args);
        mlogger->f(line);
    }
}

#ifdef __TEST_LOGGER__

int32_t main(int32_t argc, const char *argv[]) {
    mlogger_t mlogger;

    mlog_init(&mlogger, NULL, "log2stderr", MLOG_FMT_LEVEL_HEAD,
              MLOG_FMT_COLOR | MLOG_FMT_TIMESTAMP | MLOG_FMT_LEVEL_HEAD | MLOG_FMT_NEWLINE);
    mlog_set_logger(&mlogger, MLOG_INFO, (mlog_f)puts);

    for (int32_t i = 0; i < MLOG_DEBG + 1; i++) {
        mlogf(&mlogger, i, "hello %s", "world");
    }
}

#endif /* __TEST_LOGGER__ */
