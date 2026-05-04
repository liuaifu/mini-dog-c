/**
 * mini-dog-c 编译器 - Token 定义
 * 
 * 定义词法分析器产生的所有 Token 类型
 */

#ifndef TOKEN_H
#define TOKEN_H

#include "common.h"

/* ===== Token 类型 ===== */

typedef enum {
    /* 字面量 */
    TOKEN_IDENT,         // 标识符
    TOKEN_CHAR_LITERAL,  // 字符字面量: 'a'
    TOKEN_INT_LITERAL,   // 整数字面量: 42
    TOKEN_DOUBLE_LITERAL,// 浮点数字面量: 3.14
    TOKEN_STRING_LITERAL,// 字符串字面量: "hello"
    TOKEN_BOOL_LITERAL,  // 布尔字面量: true, false
    
    /* 操作符 */
    TOKEN_ASSIGN,        // =
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_STAR,          // *
    TOKEN_SLASH,         // /
    TOKEN_NOT,           // !
    TOKEN_EQ,            // ==
    TOKEN_NE,            // !=
    TOKEN_LE,            // <=
    TOKEN_GE,            // >=
    TOKEN_LT,            // <
    TOKEN_GT,            // >
    
    /* 分隔符 */
    TOKEN_COMMA,         // ,
    TOKEN_SEMICOLON,     // ;
    TOKEN_COLON,         // :
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_LBRACE,        // {
    TOKEN_RBRACE,        // }
    TOKEN_LBRACKET,      // [
    TOKEN_RBRACKET,      // ]
    
    /* 关键词 */
    TOKEN_FN,            // fn
    TOKEN_LET,           // let
    TOKEN_IF,            // if
    TOKEN_ELSE,          // else
    TOKEN_RETURN,        // return
    
    /* 特殊 */
    TOKEN_EOF,           // 文件结束
    TOKEN_UNKNOWN        // 未知 token
} TokenType;

/* ===== Token 结构 ===== */

typedef struct {
    TokenType type;
    char *lexeme;        // Token 的文本表示
    int line;            // 行号
    int column;          // 列号
    
    // 语义值（用于字面量）
    union {
        int int_value;           // 整数
        double double_value;      // 浮点数
        char char_value;          // 字符
        bool bool_value;          // 布尔
        char *string_value;       // 字符串
    } value;
} Token;

/* ===== Token 类型字符串 ===== */

static inline const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_IDENT:         return "IDENT";
        case TOKEN_CHAR_LITERAL:  return "CHAR_LITERAL";
        case TOKEN_INT_LITERAL:   return "INT_LITERAL";
        case TOKEN_DOUBLE_LITERAL:return "DOUBLE_LITERAL";
        case TOKEN_STRING_LITERAL:return "STRING_LITERAL";
        case TOKEN_BOOL_LITERAL:  return "BOOL_LITERAL";
        case TOKEN_ASSIGN:        return "ASSIGN";
        case TOKEN_PLUS:          return "PLUS";
        case TOKEN_MINUS:         return "MINUS";
        case TOKEN_STAR:          return "STAR";
        case TOKEN_SLASH:         return "SLASH";
        case TOKEN_NOT:           return "NOT";
        case TOKEN_EQ:            return "EQ";
        case TOKEN_NE:            return "NE";
        case TOKEN_LE:            return "LE";
        case TOKEN_GE:            return "GE";
        case TOKEN_LT:            return "LT";
        case TOKEN_GT:            return "GT";
        case TOKEN_COMMA:          return "COMMA";
        case TOKEN_SEMICOLON:     return "SEMICOLON";
        case TOKEN_COLON:         return "COLON";
        case TOKEN_LPAREN:        return "LPAREN";
        case TOKEN_RPAREN:        return "RPAREN";
        case TOKEN_LBRACE:        return "LBRACE";
        case TOKEN_RBRACE:        return "RBRACE";
        case TOKEN_LBRACKET:      return "LBRACKET";
        case TOKEN_RBRACKET:      return "RBRACKET";
        case TOKEN_FN:            return "FN";
        case TOKEN_LET:           return "LET";
        case TOKEN_IF:            return "IF";
        case TOKEN_ELSE:          return "ELSE";
        case TOKEN_RETURN:        return "RETURN";
        case TOKEN_EOF:           return "EOF";
        case TOKEN_UNKNOWN:       return "UNKNOWN";
        default:                  return "INVALID";
    }
}

/* ===== Token 创建与释放 ===== */

/**
 * 创建一个新 Token
 */
Token *token_create(TokenType type, const char *lexeme, int line, int column);

/**
 * 释放 Token
 */
void token_free(Token *token);

/**
 * 打印 Token 信息
 */
void token_print(const Token *token);

#endif /* TOKEN_H */