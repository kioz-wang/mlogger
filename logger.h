/**
 * @file logger.h
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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum mlog_level {
    MLOG_ERRO,
    MLOG_ERROR = MLOG_ERRO,
    MLOG_WARN,
    MLOG_INFO,
    MLOG_VERB,
    MLOG_DEBG,
    MLOG_DEBUG = MLOG_DEBG,
};
typedef uint8_t mlog_level_t;

/**
 * @brief 解析字符串到 mlog_level_t，一般用在命令行解析函数中
 *
 * @param s "ERRO", "WARN", "INFO", "VERB", "DEBG"，或数字（不限进制）
 * @return mlog_level_t 始终返回 mlog_level_t 范围内的值（无法解析或越界的将降为 MLOG_DEBG）
 */
mlog_level_t mlog_level_parse(const char *s);

typedef void (*mlog_f)(const char *msg);

/* 格式化配置：使能颜色，按等级配置在 `colors[]` */
#define MLOG_FMT_COLOR (1u << 0)
/* 格式化配置：行首输出时间戳，格式`[%lx]`，单位 ns */
#define MLOG_FMT_TIMESTAMP (1u << 1)
/* 格式化配置：行首输出等级标识，按等级配置在 `level_heads[]` */
#define MLOG_FMT_LEVEL_HEAD (1u << 2)
/* 格式化配置：行尾换行 */
#define MLOG_FMT_NEWLINE (1u << 3)

struct mlogger {
    mlog_level_t level;
    mlog_f       f;
    mlog_level_t level_stderr;
    bool         enable_stderr;
    uint8_t      fmt_cfg;
    uint8_t      fmt_cfg_stderr;
};
typedef struct mlogger mlogger_t;

/**
 * @brief 初始化 mlogger
 * @note 日志接口初始化为 NULL，此时实际不记录日志。需要通过 `mlog_set_logger` 设置。
 *
 * @param mlogger
 * @param envname 根据环境变量初始化日志等级（默认 MLOG_ERRO）
 * @param envname_stderr 根据环境变量使能 stderr 输出，并初始化日志等级（默认不输出）
 * @param fmt_cfg 格式化配置（`MLOG_FMT_` 按位或）
 * @param fmt_cfg_stderr stderr 输出的格式化配置（`MLOG_FMT_` 按位或）
 */
void mlog_init(mlogger_t *mlogger, const char *envname, const char *envname_stderr, uint8_t fmt_cfg,
               uint8_t fmt_cfg_stderr);
/**
 * @brief 设置日志等级和日志接口
 *
 * @param mlogger
 * @param level 越界的将被降为 MLOG_DEBG
 * @param f （传入 NULL 时维持原接口）
 */
void mlog_set_logger(mlogger_t *mlogger, mlog_level_t level, mlog_f f);
/**
 * @brief 使能 stderr 输出，并设置日志等级
 *
 * @param mlogger
 * @param level 越界的将被降为 MLOG_DEBG
 */
void mlog_enable_stderr(mlogger_t *mlogger, mlog_level_t level);
/**
 * @brief 记录日志
 * @note
 * 日志格式化后通过接口输出，格式化可能会截断日志，但当使能了颜色或换行时，颜色始终会在尾部重置，换行始终有效。格式化栈内存为
 * MLOGGER_FMT_MAX（默认 1024），可在编译时通过 `-DMLOGGER_FMT_MAX=?` 覆盖
 *
 * @param mlogger
 * @param level
 * @param fmt
 * @param ...
 */
void mlogf(mlogger_t *mlogger, mlog_level_t level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __LOGGER_H__ */
