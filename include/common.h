/**
 * mini-dog-c 编译器 - 公共定义
 * 
 * 提供通用的类型定义、宏定义和错误处理
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/* ===== 错误处理 ===== */

#define ERROR_MAX 256

typedef struct {
    bool has_error;
    char message[ERROR_MAX];
    int line;
    int column;
} Error;

/**
 * 初始化错误结构
 */
static inline void error_init(Error *err) {
    err->has_error = false;
    err->message[0] = '\0';
    err->line = 1;
    err->column = 1;
}

/**
 * 设置错误信息
 */
static inline void error_set(Error *err, const char *msg, int line, int column) {
    err->has_error = true;
    snprintf(err->message, ERROR_MAX, "%s", msg);
    err->line = line;
    err->column = column;
}

#define ERROR_SET(err, msg) error_set(err, msg, __LINE__, 0)

/* ===== 动态数组宏 ===== */

/**
 * 动态数组声明 - 简化动态数组的实现
 * type: 元素类型
 * name: 数组名
 */
#define DECLARE_ARRAY(type, name) \
    typedef struct { \
        type *data; \
        int size; \
        int capacity; \
    } name

/**
 * 初始化动态数组
 */
#define ARRAY_INIT(arr) \
    do { \
        (arr).data = NULL; \
        (arr).size = 0; \
        (arr).capacity = 0; \
    } while(0)

/**
 * 释放动态数组
 */
#define ARRAY_FREE(arr) \
    do { \
        if ((arr).data) free((arr).data); \
    } while(0)

/**
 * 添加元素到动态数组
 */
#define ARRAY_PUSH(arr, elem) \
    do { \
        if ((arr).capacity == 0) { \
            (arr).capacity = 16; \
            (arr).data = malloc((arr).capacity * sizeof((arr).data[0])); \
        } else if ((arr).size >= (arr).capacity) { \
            (arr).capacity *= 2; \
            (arr).data = realloc((arr).data, (arr).capacity * sizeof((arr).data[0])); \
        } \
        (arr).data[(arr).size++] = (elem); \
    } while(0)

/* ===== 字符串工具 ===== */

/**
 * 检查字符串是否以指定前缀开头
 */
static inline bool starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

/**
 * 创建字符串副本
 */
static inline char *strdup_custom(const char *str) {
    char *copy = malloc(strlen(str) + 1);
    if (copy) strcpy(copy, str);
    return copy;
}

#endif /* COMMON_H */