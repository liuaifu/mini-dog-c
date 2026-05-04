/**
 * mini-dog-c 编译器 - 语法分析器实现
 */

#include "parser.h"

/* ===== 前置声明 ===== */
static ASTNode *parse_statement(Parser *parser);
static ASTNode *parse_expression(Parser *parser);

/* ===== 辅助函数 ===== */

/**
 * 检查是否到达 Token 列表末尾
 */
static bool is_at_end(Parser *parser) {
    return parser->current >= parser->tokens->size ||
           parser->tokens->data[parser->current]->type == TOKEN_EOF;
}

/**
 * 获取当前 Token
 */
static Token *peek(Parser *parser) {
    if (is_at_end(parser)) return NULL;
    return parser->tokens->data[parser->current];
}

/**
 * 查看下一个 Token（不前进）
 */
static Token *peek_next(Parser *parser) {
    if (parser->current + 1 >= parser->tokens->size) return NULL;
    return parser->tokens->data[parser->current + 1];
}

/**
 * 消费当前 Token 并前进
 */
static Token *advance(Parser *parser) {
    if (!is_at_end(parser)) {
        parser->current++;
    }
    return parser->tokens->data[parser->current - 1];
}

/**
 * 检查当前 Token 类型是否匹配
 */
static bool check(Parser *parser, TokenType type) {
    if (is_at_end(parser)) return false;
    return peek(parser)->type == type;
}

/**
 * 检查当前 Token 类型并消费
 */
static bool match(Parser *parser, TokenType type) {
    if (check(parser, type)) {
        advance(parser);
        return true;
    }
    return false;
}

/**
 * 消费指定类型的 Token
 */
static Token *consume(Parser *parser, TokenType type, const char *message) {
    if (check(parser, type)) return advance(parser);
    
    // 错误：期望的类型不匹配
    Token *current = peek(parser);
    char buf[256];
    snprintf(buf, sizeof(buf), "%s (got %s)", message, 
             current ? token_type_to_string(current->type) : "EOF");
    error_set(&parser->error, buf, 
              current ? current->line : 0,
              current ? current->column : 0);
    return NULL;
}

/**
 * 设置错误信息
 */
static void error_at_current(Parser *parser, const char *message) {
    Token *current = peek(parser);
    if (current) {
        error_set(&parser->error, message, current->line, current->column);
    }
}

/* ===== 表达式解析 ===== */

/**
 * 前置声明
 */
static ASTNode *parse_expression(Parser *parser);
static ASTNode *parse_assignment_expr(Parser *parser);
static ASTNode *parse_logical_or_expr(Parser *parser);
static ASTNode *parse_logical_and_expr(Parser *parser);
static ASTNode *parse_equality_expr(Parser *parser);
static ASTNode *parse_relational_expr(Parser *parser);
static ASTNode *parse_additive_expr(Parser *parser);
static ASTNode *parse_multiplicative_expr(Parser *parser);
static ASTNode *parse_unary_expr(Parser *parser);
static ASTNode *parse_primary(Parser *parser);
static ASTNode *parse_block(Parser *parser);

/**
 * 解析赋值表达式
 * assignment ::= IDENT "=" assignment | logical_or
 */
static ASTNode *parse_assignment_expr(Parser *parser) {
    ASTNode *expr = parse_logical_or_expr(parser);
    
    if (match(parser, TOKEN_ASSIGN)) {
        if (expr->type == AST_IDENTIFIER) {
            ASTNode *value = parse_assignment_expr(parser);
            return ast_create_assign(expr->data.identifier.name, value);
        } else {
            error_at_current(parser, "赋值操作符左侧必须是标识符");
            return expr;
        }
    }
    
    return expr;
}

/**
 * 解析逻辑或表达式
 * logical_or ::= logical_and ("||" logical_and)*
 */
static ASTNode *parse_logical_or_expr(Parser *parser) {
    ASTNode *expr = parse_logical_and_expr(parser);
    
    while (match(parser, TOKEN_EQ) || match(parser, TOKEN_NE) ||
           match(parser, TOKEN_LT) || match(parser, TOKEN_GT)) {
        // 实际上 || 是 TOKEN_EQ（双等）后面跟 || 形成的...
        // 但我们这里把 || 当作二元操作符处理
        TokenType op = parser->tokens->data[parser->current - 1]->type;
        ASTNode *right = parse_logical_and_expr(parser);
        expr = ast_create_binary(op, expr, right);
    }
    
    return expr;
}

// 注意：我们需要添加对 || 和 && 的支持
static bool match_with_value(Parser *parser, TokenType type) {
    if (check(parser, type)) {
        advance(parser);
        return true;
    }
    return false;
}

/**
 * 解析逻辑与表达式
 * logical_and ::= equality ("&&" equality)*
 */
static ASTNode *parse_logical_and_expr(Parser *parser) {
    ASTNode *expr = parse_equality_expr(parser);
    
    while (match_with_value(parser, TOKEN_EQ)) {
        // 检测 &&：在 TOKEN_NOT 后跟 =
        // 或者我们用 TOKEN_NOT 和 TOKEN_EQ 组合
        // 但更简单的方式是添加两个新 Token 类型
    }
    
    return expr;
}

/**
 * 解析相等性表达式
 * equality ::= relational ("==" | "!=" relational)*
 */
static ASTNode *parse_equality_expr(Parser *parser) {
    ASTNode *expr = parse_relational_expr(parser);
    
    while (match(parser, TOKEN_EQ) || match(parser, TOKEN_NE)) {
        TokenType op = parser->tokens->data[parser->current - 1]->type;
        ASTNode *right = parse_relational_expr(parser);
        expr = ast_create_binary(op, expr, right);
    }
    
    return expr;
}

/**
 * 解析关系表达式
 * relational ::= additive ("<" | ">" additive)*
 */
static ASTNode *parse_relational_expr(Parser *parser) {
    ASTNode *expr = parse_additive_expr(parser);
    
    while (match(parser, TOKEN_LT) || match(parser, TOKEN_GT) ||
           match(parser, TOKEN_LE) || match(parser, TOKEN_GE)) {
        TokenType op = parser->tokens->data[parser->current - 1]->type;
        ASTNode *right = parse_additive_expr(parser);
        expr = ast_create_binary(op, expr, right);
    }
    
    return expr;
}

/**
 * 解析加法表达式
 * additive ::= multiplicative ("+" | "-" multiplicative)*
 */
static ASTNode *parse_additive_expr(Parser *parser) {
    ASTNode *expr = parse_multiplicative_expr(parser);
    
    while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS)) {
        TokenType op = parser->tokens->data[parser->current - 1]->type;
        ASTNode *right = parse_multiplicative_expr(parser);
        expr = ast_create_binary(op, expr, right);
    }
    
    return expr;
}

/**
 * 解析乘法表达式
 * multiplicative ::= unary ("*" | "/" unary)*
 */
static ASTNode *parse_multiplicative_expr(Parser *parser) {
    ASTNode *expr = parse_unary_expr(parser);
    
    while (match(parser, TOKEN_STAR) || match(parser, TOKEN_SLASH)) {
        TokenType op = parser->tokens->data[parser->current - 1]->type;
        ASTNode *right = parse_unary_expr(parser);
        expr = ast_create_binary(op, expr, right);
    }
    
    return expr;
}

/**
 * 解析一元表达式
 * unary ::= ("!" | "-") unary | primary
 */
static ASTNode *parse_unary_expr(Parser *parser) {
    if (match(parser, TOKEN_NOT)) {
        TokenType op = TOKEN_NOT;
        ASTNode *operand = parse_unary_expr(parser);
        return ast_create_unary(op, operand);
    }
    
    if (match(parser, TOKEN_MINUS)) {
        TokenType op = TOKEN_MINUS;
        ASTNode *operand = parse_unary_expr(parser);
        return ast_create_unary(op, operand);
    }
    
    return parse_primary(parser);
}

/**
 * 解析函数调用参数
 */
static ASTNode **parse_arguments(Parser *parser, int *arg_count) {
    *arg_count = 0;
    ASTNode **args = NULL;
    
    if (check(parser, TOKEN_RPAREN)) {
        return args;
    }
    
    args = (ASTNode **)malloc(sizeof(ASTNode *) * 16);
    *arg_count = 0;
    
    while (true) {
        ASTNode *arg = parse_assignment_expr(parser);
        args[*arg_count] = arg;
        (*arg_count)++;
        
        if (!match(parser, TOKEN_COMMA)) break;
    }
    
    return args;
}

/**
 * 解析主表达式
 * primary ::= IDENT | literal | "(" expression ")"
 * literal ::= INT_LITERAL | DOUBLE_LITERAL | CHAR_LITERAL | BOOL_LITERAL | STRING_LITERAL
 */
static ASTNode *parse_primary(Parser *parser) {
    // 布尔字面量
    if (match(parser, TOKEN_BOOL_LITERAL)) {
        Token *token = parser->tokens->data[parser->current - 1];
        return ast_create_bool(token->value.bool_value);
    }
    
    // 整数字面量
    if (match(parser, TOKEN_INT_LITERAL)) {
        Token *token = parser->tokens->data[parser->current - 1];
        return ast_create_int(token->value.int_value);
    }
    
    // 浮点数字面量
    if (match(parser, TOKEN_DOUBLE_LITERAL)) {
        Token *token = parser->tokens->data[parser->current - 1];
        return ast_create_double(token->value.double_value);
    }
    
    // 字符字面量
    if (match(parser, TOKEN_CHAR_LITERAL)) {
        Token *token = parser->tokens->data[parser->current - 1];
        return ast_create_char(token->value.char_value);
    }
    
    // 字符串字面量
    if (match(parser, TOKEN_STRING_LITERAL)) {
        Token *token = parser->tokens->data[parser->current - 1];
        return ast_create_string(token->value.string_value);
    }
    
    // 标识符或函数调用
    if (match(parser, TOKEN_IDENT)) {
        char *name = strdup_custom(parser->tokens->data[parser->current - 1]->lexeme);
        
        // 检查是否是函数调用
        if (match(parser, TOKEN_LPAREN)) {
            int arg_count;
            ASTNode **args = parse_arguments(parser, &arg_count);
            consume(parser, TOKEN_RPAREN, "期望 ')'");
            ASTNode *call = ast_create_call(name, args, arg_count);
            free(name);
            free(args);
            return call;
        }
        
        return ast_create_identifier(name);
    }
    
    // 分组表达式
    if (match(parser, TOKEN_LPAREN)) {
        ASTNode *expr = parse_assignment_expr(parser);
        if (!consume(parser, TOKEN_RPAREN, "期望 ')'")) {
            return expr;
        }
        return expr;
    }
    
    // 代码块作为表达式值
    if (check(parser, TOKEN_LBRACE)) {
        return parse_block(parser);
    }
    
    error_at_current(parser, "期望表达式");
    advance(parser);  // 跳过错误的 Token
    return ast_create_int(0);
}

/**
 * 解析表达式
 */
static ASTNode *parse_expression(Parser *parser) {
    return parse_assignment_expr(parser);
}

/* ===== 语句解析 ===== */

/**
 * 解析 let 声明语句
 * let_decl ::= "let" IDENT "=" expression ";"
 */
static ASTNode *parse_let_decl(Parser *parser) {
    if (!match(parser, TOKEN_LET)) return NULL;
    
    if (!check(parser, TOKEN_IDENT)) {
        error_at_current(parser, "期望变量名");
        return NULL;
    }
    
    Token *name_token = advance(parser);
    char *name = strdup_custom(name_token->lexeme);
    
    if (!consume(parser, TOKEN_ASSIGN, "期望 '='")) {
        free(name);
        return NULL;
    }
    
    ASTNode *initializer = parse_expression(parser);
    
    if (!consume(parser, TOKEN_SEMICOLON, "期望 ';'")) {
        ast_free(initializer);
        free(name);
        return NULL;
    }
    
    return ast_create_var_decl(name, initializer);
}

/**
 * 解析代码块
 * block ::= "{" statement* "}"
 */
static ASTNode *parse_block(Parser *parser) {
    if (!consume(parser, TOKEN_LBRACE, "期望 '{'")) {
        return NULL;
    }
    
    ASTNode **statements = (ASTNode **)malloc(sizeof(ASTNode *) * 64);
    int count = 0;
    
    while (!check(parser, TOKEN_RBRACE) && !is_at_end(parser)) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            statements[count++] = stmt;
        } else {
            // 跳过错误的语句，继续解析
            while (!check(parser, TOKEN_SEMICOLON) && !is_at_end(parser)) {
                advance(parser);
            }
            if (check(parser, TOKEN_SEMICOLON)) advance(parser);
        }
    }
    
    consume(parser, TOKEN_RBRACE, "期望 '}'");
    
    return ast_create_block(statements, count);
}

/**
 * 解析 if 语句
 * if_stmt ::= "if" "(" expression ")" block ("else" block)?
 */
static ASTNode *parse_if_stmt(Parser *parser) {
    if (!match(parser, TOKEN_IF)) return NULL;
    
    if (!consume(parser, TOKEN_LPAREN, "期望 '('")) {
        return NULL;
    }
    
    ASTNode *condition = parse_expression(parser);
    
    if (!consume(parser, TOKEN_RPAREN, "期望 ')'")) {
        ast_free(condition);
        return NULL;
    }
    
    ASTNode *then_branch = parse_block(parser);
    ASTNode *else_branch = NULL;
    
    if (match(parser, TOKEN_ELSE)) {
        else_branch = parse_block(parser);
    }
    
    return ast_create_if(condition, then_branch, else_branch);
}

/**
 * 解析 return 语句
 * return_stmt ::= "return" expression? ";"
 */
static ASTNode *parse_return_stmt(Parser *parser) {
    if (!match(parser, TOKEN_RETURN)) return NULL;
    
    ASTNode *value = NULL;
    
    if (!check(parser, TOKEN_SEMICOLON)) {
        value = parse_expression(parser);
    }
    
    if (!consume(parser, TOKEN_SEMICOLON, "期望 ';'")) {
        if (value) ast_free(value);
        return NULL;
    }
    
    return ast_create_return(value);
}

/**
 * 解析函数声明
 * fn_decl ::= "fn" IDENT "(" params? ")" block
 * params ::= IDENT ("," IDENT)*
 */
static ASTNode *parse_fn_decl(Parser *parser) {
    if (!match(parser, TOKEN_FN)) return NULL;
    
    if (!check(parser, TOKEN_IDENT)) {
        error_at_current(parser, "期望函数名");
        return NULL;
    }
    
    Token *name_token = advance(parser);
    char *name = strdup_custom(name_token->lexeme);
    
    if (!consume(parser, TOKEN_LPAREN, "期望 '('")) {
        free(name);
        return NULL;
    }
    
    // 解析参数列表
    char **params = NULL;
    int param_count = 0;
    
    if (!check(parser, TOKEN_RPAREN)) {
        params = (char **)malloc(sizeof(char *) * 16);
        
        while (check(parser, TOKEN_IDENT)) {
            Token *param_token = advance(parser);
            params[param_count] = strdup_custom(param_token->lexeme);
            param_count++;
            
            if (!match(parser, TOKEN_COMMA)) break;
        }
    }
    
    if (!consume(parser, TOKEN_RPAREN, "期望 ')'")) {
        for (int i = 0; i < param_count; i++) free(params[i]);
        free(params);
        free(name);
        return NULL;
    }
    
    ASTNode *body = parse_block(parser);
    
    return ast_create_fn_decl(name, params, param_count, body);
}

/**
 * 解析语句
 * statement ::= let_decl | fn_decl | return_stmt | if_stmt | expr_stmt
 * expr_stmt ::= expression ";"
 */
static ASTNode *parse_statement(Parser *parser) {
    // let 声明
    if (check(parser, TOKEN_LET)) {
        return parse_let_decl(parser);
    }
    
    // fn 声明
    if (check(parser, TOKEN_FN)) {
        return parse_fn_decl(parser);
    }
    
    // return 语句
    if (check(parser, TOKEN_RETURN)) {
        return parse_return_stmt(parser);
    }
    
    // if 语句
    if (check(parser, TOKEN_IF)) {
        return parse_if_stmt(parser);
    }
    
    // 表达式语句
    ASTNode *expr = parse_expression(parser);
    
    // 检查是否是 block（block 不需要分号）
    if (expr && expr->type == AST_BLOCK_STMT) {
        // block 作为语句，不需要分号
        return ast_create_expr_stmt(expr);
    }
    
    if (!consume(parser, TOKEN_SEMICOLON, "期望 ';-'")) {
        ast_free(expr);
        return NULL;
    }
    
    return ast_create_expr_stmt(expr);
}

/* ===== 主解析函数 ===== */

ASTNode *parser_parse(Parser *parser) {
    ASTNode *program = ast_create_program();
    
    while (!is_at_end(parser)) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            ast_program_add_stmt(program, stmt);
        } else {
            // 跳过错误的 Token，继续解析
            if (!is_at_end(parser)) {
                advance(parser);
            }
        }
        
        // 如果发生了错误，尝试恢复
        if (parser->error.has_error) {
            // 跳过直到分号或 EOF
            while (!check(parser, TOKEN_SEMICOLON) && !is_at_end(parser)) {
                advance(parser);
            }
            if (check(parser, TOKEN_SEMICOLON)) advance(parser);
            parser->error.has_error = false;  // 清除错误，继续解析
        }
    }
    
    return program;
}

/* ===== 解析器生命周期 ===== */

Parser *parser_create(TokenList *tokens) {
    Parser *parser = (Parser *)malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->current = 0;
    error_init(&parser->error);
    return parser;
}

void parser_free(Parser *parser) {
    if (parser) free(parser);
}

bool parser_has_error(const Parser *parser) {
    return parser->error.has_error;
}

const char *parser_error_message(const Parser *parser) {
    return parser->error.message;
}