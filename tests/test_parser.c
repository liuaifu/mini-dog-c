/**
 * mini-dog-c 编译器 - 语法分析器测试
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/lexer.h"
#include "../src/parser.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    printf("  %s... ", #name); \
    test_##name(); \
    printf("OK\n"); \
    tests_run++; \
    tests_passed++; \
} while(0)

static ASTNode *parse_code(const char *code) {
    Lexer *lexer = lexer_create(code);
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
    
    return ast;
}

#define assert_ast_type(node, kind) do { \
    if ((node)->type != (kind)) { \
        printf("FAILED: expected %d, got %d\n", (kind), (node)->type); \
        exit(1); \
    } \
} while(0)

/* ===== 测试用例 ===== */

// 测试空程序
TEST(parser_empty_program) {
    ASTNode *ast = parse_code("");
    
    assert_ast_type(ast, AST_PROGRAM);
    assert(ast->data.program.statement_count == 0);
    
    ast_free(ast);
}

// 测试简单表达式 - 整数
TEST(parser_simple_int) {
    ASTNode *ast = parse_code("42;");
    
    assert_ast_type(ast, AST_PROGRAM);
    assert(ast->data.program.statement_count == 1);
    
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_EXPR_STMT);
    
    // 表达式语句中的表达式是 return_stmt.value
    assert(stmt->data.return_stmt.value != NULL);
    assert_ast_type(stmt->data.return_stmt.value, AST_LITERAL_INT);
    
    ast_free(ast);
}

// 测试简单表达式 - 浮点数
TEST(parser_simple_double) {
    ASTNode *ast = parse_code("3.14;");
    
    assert_ast_type(ast, AST_PROGRAM);
    assert(ast->data.program.statement_count == 1);
    
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_EXPR_STMT);
    assert_ast_type(stmt->data.return_stmt.value, AST_LITERAL_DOUBLE);
    
    ast_free(ast);
}

// 测试简单表达式 - 布尔字面量
TEST(parser_simple_bool) {
    ASTNode *ast = parse_code("true;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_LITERAL_BOOL);
    
    ast_free(ast);
}

// 测试简单表达式 - 字符串字面量
TEST(parser_simple_string) {
    ASTNode *ast = parse_code("\"hello\";");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_LITERAL_STRING);
    
    ast_free(ast);
}

// 测试标识符表达式
TEST(parser_identifier) {
    ASTNode *ast = parse_code("x;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_IDENTIFIER);
    assert(strcmp(stmt->data.return_stmt.value->data.identifier.name, "x") == 0);
    
    ast_free(ast);
}

// 测试二元表达式 - 加法
TEST(parser_binary_plus) {
    ASTNode *ast = parse_code("a + b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_PLUS);
    assert_ast_type(stmt->data.return_stmt.value->data.binary_expr.left, AST_IDENTIFIER);
    assert_ast_type(stmt->data.return_stmt.value->data.binary_expr.right, AST_IDENTIFIER);
    
    ast_free(ast);
}

// 测试二元表达式 - 减法
TEST(parser_binary_minus) {
    ASTNode *ast = parse_code("a - b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_MINUS);
    
    ast_free(ast);
}

// 测试二元表达式 - 乘法
TEST(parser_binary_star) {
    ASTNode *ast = parse_code("a * b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_STAR);
    
    ast_free(ast);
}

// 测试二元表达式 - 除法
TEST(parser_binary_slash) {
    ASTNode *ast = parse_code("a / b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_SLASH);
    
    ast_free(ast);
}

// 测试二元表达式 - 优先级
TEST(parser_binary_precedence) {
    ASTNode *ast = parse_code("a + b * c;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    
    // 应该是 a + (b * c)
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_PLUS);
    
    // 左边是 a
    assert_ast_type(stmt->data.return_stmt.value->data.binary_expr.left, AST_IDENTIFIER);
    
    // 右边是 b * c
    assert_ast_type(stmt->data.return_stmt.value->data.binary_expr.right, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.right->data.binary_expr.op == TOKEN_STAR);
    
    ast_free(ast);
}

// 测试比较操作符 - 小于
TEST(parser_relational_lt) {
    ASTNode *ast = parse_code("a < b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_LT);
    
    ast_free(ast);
}

// 测试比较操作符 - 大于
TEST(parser_relational_gt) {
    ASTNode *ast = parse_code("a > b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_GT);
    
    ast_free(ast);
}

// 测试相等操作符 - ==
TEST(parser_equality_eq) {
    ASTNode *ast = parse_code("a == b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_EQ);
    
    ast_free(ast);
}

// 测试相等操作符 - !=
TEST(parser_equality_ne) {
    ASTNode *ast = parse_code("a != b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_BINARY_EXPR);
    assert(stmt->data.return_stmt.value->data.binary_expr.op == TOKEN_NE);
    
    ast_free(ast);
}

// 测试一元表达式 - 负号
TEST(parser_unary_minus) {
    ASTNode *ast = parse_code("-a;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_UNARY_EXPR);
    assert(stmt->data.return_stmt.value->data.unary_expr.op == TOKEN_MINUS);
    
    ast_free(ast);
}

// 测试一元表达式 - 逻辑非
TEST(parser_unary_not) {
    ASTNode *ast = parse_code("!flag;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_UNARY_EXPR);
    assert(stmt->data.return_stmt.value->data.unary_expr.op == TOKEN_NOT);
    
    ast_free(ast);
}

// 测试 let 变量声明
TEST(parser_let_decl) {
    ASTNode *ast = parse_code("let x = 42;");
    
    assert_ast_type(ast, AST_PROGRAM);
    assert(ast->data.program.statement_count == 1);
    
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_VAR_DECL);
    assert(strcmp(stmt->data.var_decl.name, "x") == 0);
    assert_ast_type(stmt->data.var_decl.initializer, AST_LITERAL_INT);
    
    ast_free(ast);
}

// 测试 let 变量声明（带表达式初始化）
TEST(parser_let_decl_expr) {
    ASTNode *ast = parse_code("let x = a + b;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_VAR_DECL);
    assert_ast_type(stmt->data.var_decl.initializer, AST_BINARY_EXPR);
    
    ast_free(ast);
}

// 测试赋值表达式
TEST(parser_assign_expr) {
    ASTNode *ast = parse_code("x = 42;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_ASSIGN_EXPR);
    assert(strcmp(stmt->data.return_stmt.value->data.assign_expr.name, "x") == 0);
    assert_ast_type(stmt->data.return_stmt.value->data.assign_expr.value, AST_LITERAL_INT);
    
    ast_free(ast);
}

// 测试 return 语句
TEST(parser_return) {
    ASTNode *ast = parse_code("return 42;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_RETURN_STMT);
    assert_ast_type(stmt->data.return_stmt.value, AST_LITERAL_INT);
    
    ast_free(ast);
}

// 测试 return 语句（无值）
TEST(parser_return_empty) {
    ASTNode *ast = parse_code("return;");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_RETURN_STMT);
    assert(stmt->data.return_stmt.value == NULL);
    
    ast_free(ast);
}

// 测试空代码块
TEST(parser_empty_block) {
    ASTNode *ast = parse_code("{}");
    
    assert_ast_type(ast, AST_PROGRAM);
    assert(ast->data.program.statement_count == 1);
    
    // 块被包装在 expr_stmt 中
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_EXPR_STMT);
    assert_ast_type(stmt->data.return_stmt.value, AST_BLOCK_STMT);
    assert(stmt->data.return_stmt.value->data.block.statement_count == 0);
    
    ast_free(ast);
}

// 测试带语句的代码块
TEST(parser_block_with_stmts) {
    ASTNode *ast = parse_code("{ let x = 1; return x; }");
    
    assert_ast_type(ast, AST_PROGRAM);
    // 块被包装在 expr_stmt 中
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_EXPR_STMT);
    assert_ast_type(stmt->data.return_stmt.value, AST_BLOCK_STMT);
    assert(stmt->data.return_stmt.value->data.block.statement_count == 2);
    
    ast_free(ast);
}

// 测试简单 if 语句
TEST(parser_if_simple) {
    ASTNode *ast = parse_code("if (x > 0) { return 1; }");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_IF_STMT);
    
    // condition
    assert_ast_type(stmt->data.if_stmt.condition, AST_BINARY_EXPR);
    assert(stmt->data.if_stmt.condition->data.binary_expr.op == TOKEN_GT);
    
    // then branch
    assert_ast_type(stmt->data.if_stmt.then_branch, AST_BLOCK_STMT);
    
    // else branch
    assert(stmt->data.if_stmt.else_branch == NULL);
    
    ast_free(ast);
}

// 测试 if-else 语句
TEST(parser_if_else) {
    ASTNode *ast = parse_code("if (x > 0) { return 1; } else { return 0; }");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_IF_STMT);
    assert(stmt->data.if_stmt.else_branch != NULL);
    
    ast_free(ast);
}

// 测试简单函数声明
TEST(parser_fn_simple) {
    ASTNode *ast = parse_code("fn add(a, b) { return a + b; }");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_FN_DECL);
    assert(strcmp(stmt->data.fn_decl.name, "add") == 0);
    assert(stmt->data.fn_decl.param_count == 2);
    assert(strcmp(stmt->data.fn_decl.params[0], "a") == 0);
    assert(strcmp(stmt->data.fn_decl.params[1], "b") == 0);
    assert_ast_type(stmt->data.fn_decl.body, AST_BLOCK_STMT);
    
    ast_free(ast);
}

// 测试无参数函数
TEST(parser_fn_no_params) {
    ASTNode *ast = parse_code("fn main() { return 0; }");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_FN_DECL);
    assert(stmt->data.fn_decl.param_count == 0);
    
    ast_free(ast);
}

// 测试函数调用
TEST(parser_call_expr) {
    ASTNode *ast = parse_code("foo();");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_CALL_EXPR);
    assert(strcmp(stmt->data.return_stmt.value->data.call_expr.name, "foo") == 0);
    assert(stmt->data.return_stmt.value->data.call_expr.arg_count == 0);
    
    ast_free(ast);
}

// 测试带参数的函数调用
TEST(parser_call_with_args) {
    ASTNode *ast = parse_code("add(x, y);");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt->data.return_stmt.value, AST_CALL_EXPR);
    assert(stmt->data.return_stmt.value->data.call_expr.arg_count == 2);
    
    ast_free(ast);
}

// 测试复杂表达式
TEST(parser_complex_expr) {
    ASTNode *ast = parse_code("let result = (a + b) * (c - d);");
    
    assert_ast_type(ast, AST_PROGRAM);
    ASTNode *stmt = ast->data.program.statements[0];
    assert_ast_type(stmt, AST_VAR_DECL);
    assert_ast_type(stmt->data.var_decl.initializer, AST_BINARY_EXPR);
    
    ast_free(ast);
}

// 测试完整程序
TEST(parser_complete_program) {
    ASTNode *ast = parse_code(
        "fn add(a, b) { return a + b; }"
        "fn main() { let x = 10; return add(x, 20); }"
    );
    
    assert_ast_type(ast, AST_PROGRAM);
    assert(ast->data.program.statement_count == 2);
    
    // 第一个是 add 函数
    ASTNode *fn_add = ast->data.program.statements[0];
    assert_ast_type(fn_add, AST_FN_DECL);
    assert(strcmp(fn_add->data.fn_decl.name, "add") == 0);
    
    // 第二个是 main 函数
    ASTNode *fn_main = ast->data.program.statements[1];
    assert_ast_type(fn_main, AST_FN_DECL);
    assert(strcmp(fn_main->data.fn_decl.name, "main") == 0);
    
    ast_free(ast);
}

/* ===== 主函数 ===== */

int main() {
    printf("=== 语法分析器测试 ===\n\n");
    
    RUN_TEST(parser_empty_program);
    RUN_TEST(parser_simple_int);
    RUN_TEST(parser_simple_double);
    RUN_TEST(parser_simple_bool);
    RUN_TEST(parser_simple_string);
    RUN_TEST(parser_identifier);
    RUN_TEST(parser_binary_plus);
    RUN_TEST(parser_binary_minus);
    RUN_TEST(parser_binary_star);
    RUN_TEST(parser_binary_slash);
    RUN_TEST(parser_binary_precedence);
    RUN_TEST(parser_relational_lt);
    RUN_TEST(parser_relational_gt);
    RUN_TEST(parser_equality_eq);
    RUN_TEST(parser_equality_ne);
    RUN_TEST(parser_unary_minus);
    RUN_TEST(parser_unary_not);
    RUN_TEST(parser_let_decl);
    RUN_TEST(parser_let_decl_expr);
    RUN_TEST(parser_assign_expr);
    RUN_TEST(parser_return);
    RUN_TEST(parser_return_empty);
    RUN_TEST(parser_empty_block);
    RUN_TEST(parser_block_with_stmts);
    RUN_TEST(parser_if_simple);
    RUN_TEST(parser_if_else);
    RUN_TEST(parser_fn_simple);
    RUN_TEST(parser_fn_no_params);
    RUN_TEST(parser_call_expr);
    RUN_TEST(parser_call_with_args);
    RUN_TEST(parser_complex_expr);
    RUN_TEST(parser_complete_program);
    
    printf("\n=== 测试结果 ===\n");
    printf("通过: %d / %d\n", tests_passed, tests_run);
    
    return 0;
}