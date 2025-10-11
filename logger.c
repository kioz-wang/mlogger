/**
 * @file logger.c
 * @author kioz.wang
 * @brief
 * @version 0.1
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
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MLOGGER_FMT_MAX
#define MLOGGER_FMT_MAX (1024)
#endif /* MLOGGER_FMT_MAX */

static mlog_level_t g_log_level = MLOG_DEBUG;

static void default_logger(const char *msg) { fputs(msg, stdout); }

static mlogger_f g_logger = default_logger;

static const char *log_level_names[MLOG_DEBUG + 1] = {
    "ERROR", "WARN", "INFO", "VERB", "DEBUG",
};

static int32_t check_stderr_level() {
  static int32_t value = -2;
  const char *value_s = NULL;

  if (value == -2) {
#ifdef MLOGGER_ENV
    value_s = getenv(MLOGGER_ENV);
#endif /* MLOGGER_ENV */
    if (!value_s || value_s[0] == '\0') {
      value = -1;
    }
  }
  if (value != -2)
    return value;

  for (int32_t i = 0; i < MLOG_DEBUG + 1; i++) {
    if (!strcmp(value_s, log_level_names[i])) {
      value = i;
      return value;
    }
  }

  char *endptr = NULL;
  unsigned long parsed = strtoul(value_s, &endptr, 0);
  if (value_s == endptr || parsed == ULONG_MAX) {
    value = MLOG_DEBUG;
  } else {
    value = (parsed > MLOG_DEBUG) ? MLOG_DEBUG : (int32_t)parsed;
  }
  return value;
}

static void logger(mlog_level_t lvl, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  int32_t stderr_level = check_stderr_level();

  if ((stderr_level >= 0 && lvl <= stderr_level) || lvl <= g_log_level) {
    char line[MLOGGER_FMT_MAX] = {0};
    (void)vsnprintf(line, sizeof(line) - 1, fmt, args);

    if (stderr_level >= 0 && lvl <= stderr_level)
      fputs(line, stderr);
    if (lvl <= g_log_level)
      g_logger(line);
  }

  va_end(args);
}

static void set_logger(mlog_level_t lvl, mlogger_f f) {
  g_log_level = (lvl > MLOG_DEBUG) ? MLOG_DEBUG : lvl;
  if (f)
    g_logger = f;
}

void MLOGGER_FUNC(logger)(mlog_level_t lvl, const char *fmt, ...) __attribute__((alias("logger")));
void MLOGGER_FUNC(set_logger)(mlog_level_t lvl, mlogger_f f) __attribute__((alias("set_logger")));

#ifdef __TEST_LOGGER__

int32_t main(int32_t argc, const char *argv[]) {
  set_logger(MLOG_INFO, NULL);
  for (int32_t i = 0; i < MLOG_DEBUG + 1; i++) {
    logger(i, "message level %s\n", log_level_names[i]);
  }
}

#endif /* __TEST_LOGGER__ */
