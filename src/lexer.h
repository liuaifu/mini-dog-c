/**
 * mini-dog-c 编译器 - 词法分析器
 * 
 * 将源代码字符串分解为 Token 序列
 */

#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include "common.h"

/* ===== Token 列表 ===== */

DECLARE_ARRAY(Token *, TokenList);

/**
 * 初始化 Token 列表
 */
void token_list_init(TokenList *list);

/**
 * 添加 Token 到列表
 */
void token_list_add(TokenList *list, Token *token);

/**
 * 释放 Token 列表
 */
void token_list_free(TokenList *list);

/**
 * 打印 Token 列表
 */
void token_list_print(const TokenList *list);

/* ===== 词法分析器 ===== */

typedef struct {
    const char *source;      // 源代码
    int length;              // 源代码长度
    int position;           // 当前扫描位置
    int line;               // 当前行号
    int column;             // 当前列号
    Error error;            // 错误信息
} Lexer;

/**
 * 创建词法分析器
 */
Lexer *lexer_create(const char *source);

/**
 * 释放词法分析器
 */
void lexer_free(Lexer *lexer);

/**
 * 执行词法分析，返回 Token 列表
 */
TokenList *lexer_tokenize(Lexer *lexer);

/**
 * 获取当前错误
 */
bool lexer_has_error(const Lexer *lexer);
const char *lexer_error_message(const Lexer *lexer);
int lexer_error_line(const Lexer *lexer);
int lexer_error_column(const Lexer *lexer);

#endif /* LEXER_H */