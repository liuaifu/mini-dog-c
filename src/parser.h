/**
 * mini-dog-c 编译器 - 语法分析器
 * 
 * 实现递归下降解析器，将 Token 流转换为 AST
 */

#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lexer.h"
#include "ast.h"

/* ===== 解析器 ===== */

typedef struct {
    TokenList *tokens;      // Token 列表
    int current;            // 当前解析位置
    Error error;            // 错误信息
} Parser;

/**
 * 创建解析器
 */
Parser *parser_create(TokenList *tokens);

/**
 * 释放解析器
 */
void parser_free(Parser *parser);

/**
 * 执行语法分析，返回 AST
 */
ASTNode *parser_parse(Parser *parser);

/**
 * 检查是否有错误
 */
bool parser_has_error(const Parser *parser);

/**
 * 获取错误信息
 */
const char *parser_error_message(const Parser *parser);

#endif /* PARSER_H */