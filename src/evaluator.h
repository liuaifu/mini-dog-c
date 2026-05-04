/**
 * mini-dog-c 编译器 - 解释器
 * 
 * 通过遍历 AST 执行程序
 */

#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "common.h"

/* ===== 运行时值 ===== */

typedef enum {
    VALUE_INT,
    VALUE_DOUBLE,
    VALUE_CHAR,
    VALUE_BOOL,
    VALUE_STRING,
    VALUE_FUNCTION,
    VALUE_NULL
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        int int_value;
        double double_value;
        char char_value;
        bool bool_value;
        struct {
            char *string;
            int length;
        } string_value;
        struct {
            char *name;
            char **params;
            int param_count;
            ASTNode *body;
            struct Env *closure;
        } function;
    } data;
} Value;

/**
 * 创建值
 */
Value *value_create_int(int v);
Value *value_create_double(double v);
Value *value_create_char(char v);
Value *value_create_bool(bool v);
Value *value_create_string(const char *str);
Value *value_create_null(void);

/**
 * 创建函数值
 */
Value *value_create_function(const char *name, char **params, int param_count, ASTNode *body, void *closure);

/**
 * 释放值
 */
void value_free(Value *value);

/**
 * 打印值
 */
void value_print(const Value *value);

/**
 * 值转为字符串（返回新分配的字符串）
 */
char *value_to_string(const Value *value);

/* ===== 运行时错误 ===== */

typedef struct {
    bool has_error;
    char message[256];
} RuntimeError;

/* ===== 解释器 ===== */

typedef struct Env Env;

typedef struct {
    ASTNode *ast;              // AST 根节点
    Env *global_env;           // 全局环境
    RuntimeError error;        // 运行时错误
} Evaluator;

/**
 * 创建解释器
 */
Evaluator *evaluator_create(ASTNode *ast);

/**
 * 释放解释器
 */
void evaluator_free(Evaluator *eval);

/**
 * 执行程序
 */
Value *evaluator_run(Evaluator *eval);

/**
 * 检查是否有错误
 */
bool evaluator_has_error(const Evaluator *eval);
const char *evaluator_error_message(const Evaluator *eval);

/* ===== 环境（变量作用域） ===== */

/**
 * 创建环境
 */
Env *env_create(Env *parent);

/**
 * 释放环境
 */
void env_free(Env *env);

/**
 * 定义变量
 */
bool env_define(Env *env, const char *name, Value *value);

/**
 * 获取变量值
 */
Value *env_get(Env *env, const char *name);

/**
 * 设置变量值
 */
bool env_set(Env *env, const char *name, Value *value);

#endif /* EVALUATOR_H */