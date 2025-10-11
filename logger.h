/**
 * @file logger.h
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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdint.h>

#ifndef MLOGGER_PREFIX
#define MLOGGER_PREFIX _
#endif
#define __SYMBOL_CONCAT__(a, b) a##b
#define __MLOGGER_FUNC__(prefix, name) __SYMBOL_CONCAT__(prefix, name)
#define MLOGGER_FUNC(name) __MLOGGER_FUNC__(MLOGGER_PREFIX, name)

#ifdef __cplusplus
extern "C" {
#endif

enum mlog_level {
  MLOG_ERROR,
  MLOG_WARN,
  MLOG_INFO,
  MLOG_VERB,
  MLOG_DEBUG,
};
typedef uint8_t mlog_level_t;

typedef void (*mlogger_f)(const char *msg);

void MLOGGER_FUNC(logger)(mlog_level_t lvl, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void MLOGGER_FUNC(set_logger)(mlog_level_t lvl, mlogger_f f);

#define logfE(fmt, ...) MLOGGER_FUNC(logger)(MLOG_ERROR, fmt "\n", ##__VA_ARGS__)
#define logfW(fmt, ...) MLOGGER_FUNC(logger)(MLOG_WARN, fmt "\n", ##__VA_ARGS__)
#define logfI(fmt, ...) MLOGGER_FUNC(logger)(MLOG_INFO, fmt "\n", ##__VA_ARGS__)
#define logfV(fmt, ...) MLOGGER_FUNC(logger)(MLOG_VERB, fmt "\n", ##__VA_ARGS__)
#define logfD(fmt, ...) MLOGGER_FUNC(logger)(MLOG_DEBUG, fmt "\n", ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H__ */
