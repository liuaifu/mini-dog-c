/**
 * mini-dog-c 编译器 - 抽象语法树 (AST)
 * 
 * 定义编译器的 AST 节点类型和操作函数
 */

#ifndef AST_H
#define AST_H

#include "token.h"
#include "common.h"

/* ===== AST 节点类型 ===== */

typedef enum {
    AST_PROGRAM,         // 程序节点
    
    // 声明
    AST_VAR_DECL,        // 变量声明
    AST_FN_DECL,         // 函数声明
    
    // 语句
    AST_EXPR_STMT,       // 表达式语句
    AST_RETURN_STMT,     // 返回语句
    AST_IF_STMT,         // 条件语句
    AST_BLOCK_STMT,       // 代码块
    
    // 表达式
    AST_IDENTIFIER,      // 标识符引用
    AST_LITERAL_INT,     // 整数字面量
    AST_LITERAL_DOUBLE,   // 浮点字面量
    AST_LITERAL_CHAR,    // 字符字面量
    AST_LITERAL_BOOL,     // 布尔字面量
    AST_LITERAL_STRING,  // 字符串字面量
    AST_BINARY_EXPR,     // 二元表达式
    AST_UNARY_EXPR,      // 一元表达式
    AST_CALL_EXPR,       // 函数调用
    AST_ASSIGN_EXPR,     // 赋值表达式
} ASTNodeType;

/* ===== AST 节点结构 ===== */

typedef struct ASTNode {
    ASTNodeType type;
    Token *token;                 // 对应的 Token
    struct ASTNode *parent;       // 父节点
    union {
        // Program
        struct {
            struct ASTNode **statements;
            int statement_count;
        } program;
        
        // Variable Declaration: let x = expr;
        struct {
            char *name;
            struct ASTNode *initializer;
        } var_decl;
        
        // Function Declaration: fn add(a, b) { ... }
        struct {
            char *name;
            char **params;
            int param_count;
            struct ASTNode *body;
        } fn_decl;
        
        // Return Statement: return expr;
        struct {
            struct ASTNode *value;
        } return_stmt;
        
        // If Statement: if (cond) { ... } else { ... }
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;
        
        // Block: { ... }
        struct {
            struct ASTNode **statements;
            int statement_count;
        } block;
        
        // Binary Expression: a + b, a == b, etc.
        struct {
            TokenType op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_expr;
        
        // Unary Expression: -a, !b
        struct {
            TokenType op;
            struct ASTNode *operand;
        } unary_expr;
        
        // Assignment: x = value
        struct {
            char *name;
            struct ASTNode *value;
        } assign_expr;
        
        // Function Call: foo(a, b)
        struct {
            char *name;
            struct ASTNode **args;
            int arg_count;
        } call_expr;
        
        // Identifier Reference
        struct {
            char *name;
        } identifier;
        
        // Literals
        int int_value;           // 用于 AST_LITERAL_INT
        double double_value;     // 用于 AST_LITERAL_DOUBLE
        char char_value;         // 用于 AST_LITERAL_CHAR
        bool bool_value;         // 用于 AST_LITERAL_BOOL
        char *string_value;      // 用于 AST_LITERAL_STRING
    } data;
} ASTNode;

/* ===== AST 节点创建 ===== */

/**
 * 创建程序节点
 */
ASTNode *ast_create_program(void);

/**
 * 创建变量声明节点
 */
ASTNode *ast_create_var_decl(const char *name, ASTNode *initializer);

/**
 * 创建函数声明节点
 */
ASTNode *ast_create_fn_decl(const char *name, char **params, int param_count, ASTNode *body);

/**
 * 创建返回语句节点
 */
ASTNode *ast_create_return(ASTNode *value);

/**
 * 创建 If 语句节点
 */
ASTNode *ast_create_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);

/**
 * 创建代码块节点
 */
ASTNode *ast_create_block(ASTNode **statements, int count);

/**
 * 创建二元表达式节点
 */
ASTNode *ast_create_binary(TokenType op, ASTNode *left, ASTNode *right);

/**
 * 创建一元表达式节点
 */
ASTNode *ast_create_unary(TokenType op, ASTNode *operand);

/**
 * 创建赋值表达式节点
 */
ASTNode *ast_create_assign(const char *name, ASTNode *value);

/**
 * 创建函数调用节点
 */
ASTNode *ast_create_call(const char *name, ASTNode **args, int arg_count);

/**
 * 创建标识符节点
 */
ASTNode *ast_create_identifier(const char *name);

/**
 * 创建整数字面量节点
 */
ASTNode *ast_create_int(int value);

/**
 * 创建浮点字面量节点
 */
ASTNode *ast_create_double(double value);

/**
 * 创建字符字面量节点
 */
ASTNode *ast_create_char(char value);

/**
 * 创建布尔字面量节点
 */
ASTNode *ast_create_bool(bool value);

/**
 * 创建字符串字面量节点
 */
ASTNode *ast_create_string(const char *value);

/**
 * 创建表达式语句节点
 */
ASTNode *ast_create_expr_stmt(ASTNode *expr);

/* ===== AST 节点操作 ===== */

/**
 * 添加语句到程序
 */
void ast_program_add_stmt(ASTNode *program, ASTNode *stmt);

/**
 * 添加语句到代码块
 */
void ast_block_add_stmt(ASTNode *block, ASTNode *stmt);

/**
 * 释放 AST 节点
 */
void ast_free(ASTNode *node);

/**
 * 打印 AST（调试用）
 */
void ast_print(const ASTNode *node, int indent);

#endif /* AST_H */