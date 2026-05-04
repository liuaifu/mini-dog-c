/**
 * mini-dog-c 编译器 - 词法分析器测试
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/token.h"
#include "../src/lexer.h"

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

#define assert_token_type(tok, expected) do { \
    if ((tok)->type != (expected)) { \
        printf("FAILED: expected %s, got %s\n", \
               token_type_to_string(expected), \
               token_type_to_string((tok)->type)); \
        exit(1); \
    } \
} while(0)

#define assert_token_lexeme(tok, expected) do { \
    if (strcmp((tok)->lexeme, (expected)) != 0) { \
        printf("FAILED: expected '%s', got '%s'\n", \
               (expected), (tok)->lexeme); \
        exit(1); \
    } \
} while(0)

#define assert_token_int(tok, expected) do { \
    if ((tok)->value.int_value != (expected)) { \
        printf("FAILED: expected %d, got %d\n", \
               (expected), (tok)->value.int_value); \
        exit(1); \
    } \
} while(0)

#define assert_token_bool(tok, expected) do { \
    if ((tok)->value.bool_value != (expected)) { \
        printf("FAILED: expected %s, got %s\n", \
               (expected) ? "true" : "false", \
               (tok)->value.bool_value ? "true" : "false"); \
        exit(1); \
    } \
} while(0)

#define assert_token_double(tok, expected) do { \
    if ((tok)->value.double_value != (expected)) { \
        printf("FAILED: expected %f, got %f\n", \
               (expected), (tok)->value.double_value); \
        exit(1); \
    } \
} while(0)

/* ===== 测试用例 ===== */

// 测试简单标识符
TEST(lexer_simple_identifier) {
    Lexer *lexer = lexer_create("foo");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);  // foo + EOF
    assert_token_type(tokens->data[0], TOKEN_IDENT);
    assert_token_lexeme(tokens->data[0], "foo");
    assert_token_type(tokens->data[1], TOKEN_EOF);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试下划线开头的标识符
TEST(lexer_underscore_identifier) {
    Lexer *lexer = lexer_create("_private_var");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_IDENT);
    assert_token_lexeme(tokens->data[0], "_private_var");
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试带数字的标识符
TEST(lexer_identifier_with_digits) {
    Lexer *lexer = lexer_create("var123");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_IDENT);
    assert_token_lexeme(tokens->data[0], "var123");
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试整数
TEST(lexer_integer) {
    Lexer *lexer = lexer_create("42");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_INT_LITERAL);
    assert_token_int(tokens->data[0], 42);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试浮点数
TEST(lexer_double) {
    Lexer *lexer = lexer_create("3.14159");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_DOUBLE_LITERAL);
    assert_token_double(tokens->data[0], 3.14159);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试布尔字面量 - true
TEST(lexer_bool_true) {
    Lexer *lexer = lexer_create("true");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_BOOL_LITERAL);
    assert_token_bool(tokens->data[0], true);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试布尔字面量 - false
TEST(lexer_bool_false) {
    Lexer *lexer = lexer_create("false");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_BOOL_LITERAL);
    assert_token_bool(tokens->data[0], false);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试字符字面量
TEST(lexer_char_literal) {
    Lexer *lexer = lexer_create("'a'");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_CHAR_LITERAL);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试字符串字面量
TEST(lexer_string_literal) {
    Lexer *lexer = lexer_create("\"hello world\"");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_STRING_LITERAL);
    assert(strcmp(tokens->data[0]->value.string_value, "hello world") == 0);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 加法
TEST(lexer_plus) {
    Lexer *lexer = lexer_create("+");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_PLUS);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 减法
TEST(lexer_minus) {
    Lexer *lexer = lexer_create("-");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_MINUS);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 乘法
TEST(lexer_star) {
    Lexer *lexer = lexer_create("*");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_STAR);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 除法
TEST(lexer_slash) {
    Lexer *lexer = lexer_create("/");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_SLASH);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 赋值
TEST(lexer_assign) {
    Lexer *lexer = lexer_create("=");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_ASSIGN);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 双等于
TEST(lexer_eq) {
    Lexer *lexer = lexer_create("==");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_EQ);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 不等于
TEST(lexer_ne) {
    Lexer *lexer = lexer_create("!=");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_NE);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 小于
TEST(lexer_lt) {
    Lexer *lexer = lexer_create("<");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_LT);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 大于
TEST(lexer_gt) {
    Lexer *lexer = lexer_create(">");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_GT);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试操作符 - 逻辑非
TEST(lexer_not) {
    Lexer *lexer = lexer_create("!");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_NOT);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试分隔符 - 逗号
TEST(lexer_comma) {
    Lexer *lexer = lexer_create(",");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_COMMA);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试分隔符 - 分号
TEST(lexer_semicolon) {
    Lexer *lexer = lexer_create(";");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_SEMICOLON);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试分隔符 - 冒号
TEST(lexer_colon) {
    Lexer *lexer = lexer_create(":");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_COLON);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试括号 - 圆括号
TEST(lexer_paren) {
    Lexer *lexer = lexer_create("()");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 3);
    assert_token_type(tokens->data[0], TOKEN_LPAREN);
    assert_token_type(tokens->data[1], TOKEN_RPAREN);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试括号 - 花括号
TEST(lexer_brace) {
    Lexer *lexer = lexer_create("{}");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 3);
    assert_token_type(tokens->data[0], TOKEN_LBRACE);
    assert_token_type(tokens->data[1], TOKEN_RBRACE);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试括号 - 方括号
TEST(lexer_bracket) {
    Lexer *lexer = lexer_create("[]");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 3);
    assert_token_type(tokens->data[0], TOKEN_LBRACKET);
    assert_token_type(tokens->data[1], TOKEN_RBRACKET);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试关键词 - fn
TEST(lexer_fn) {
    Lexer *lexer = lexer_create("fn");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_FN);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试关键词 - let
TEST(lexer_let) {
    Lexer *lexer = lexer_create("let");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_LET);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试关键词 - if
TEST(lexer_if) {
    Lexer *lexer = lexer_create("if");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_IF);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试关键词 - else
TEST(lexer_else) {
    Lexer *lexer = lexer_create("else");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_ELSE);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试关键词 - return
TEST(lexer_return) {
    Lexer *lexer = lexer_create("return");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_RETURN);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试空白字符跳过
TEST(lexer_whitespace) {
    Lexer *lexer = lexer_create("  \t\n\n  foo  ");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 2);
    assert_token_type(tokens->data[0], TOKEN_IDENT);
    assert_token_lexeme(tokens->data[0], "foo");
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试注释跳过
TEST(lexer_comment) {
    Lexer *lexer = lexer_create("foo // 这是注释\nbar");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 3);
    assert_token_type(tokens->data[0], TOKEN_IDENT);
    assert_token_lexeme(tokens->data[0], "foo");
    assert_token_type(tokens->data[1], TOKEN_IDENT);
    assert_token_lexeme(tokens->data[1], "bar");
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试复杂表达式
TEST(lexer_complex_expression) {
    Lexer *lexer = lexer_create("result = a + b * c - d / e");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 12);  // result = a + b * c - d / e + EOF
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试完整函数声明
TEST(lexer_function_decl) {
    Lexer *lexer = lexer_create("fn add(a, b) { return a + b; }");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 15);  // fn add ( a , b ) { return a + b ; }
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试完整的 let 声明
TEST(lexer_let_decl) {
    Lexer *lexer = lexer_create("let x = 42;");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 6);  // let x = 42 ; EOF
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试 if 语句
TEST(lexer_if_stmt) {
    Lexer *lexer = lexer_create("if (x > 0) { return x; }");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 12);
    
    token_list_free(tokens);
    lexer_free(lexer);
}

// 测试 if-else 语句
TEST(lexer_if_else_stmt) {
    Lexer *lexer = lexer_create("if (x > 0) { return x; } else { return 0; }");
    TokenList *tokens = lexer_tokenize(lexer);
    
    assert(tokens->size == 18);  // if ( x > 0 ) { return x ; } else { return 0 ; } EOF
    
    token_list_free(tokens);
    lexer_free(lexer);
}

/* ===== 主函数 ===== */

int main() {
    printf("=== 词法分析器测试 ===\n\n");
    
    RUN_TEST(lexer_simple_identifier);
    RUN_TEST(lexer_underscore_identifier);
    RUN_TEST(lexer_identifier_with_digits);
    RUN_TEST(lexer_integer);
    RUN_TEST(lexer_double);
    RUN_TEST(lexer_bool_true);
    RUN_TEST(lexer_bool_false);
    RUN_TEST(lexer_char_literal);
    RUN_TEST(lexer_string_literal);
    RUN_TEST(lexer_plus);
    RUN_TEST(lexer_minus);
    RUN_TEST(lexer_star);
    RUN_TEST(lexer_slash);
    RUN_TEST(lexer_assign);
    RUN_TEST(lexer_eq);
    RUN_TEST(lexer_ne);
    RUN_TEST(lexer_lt);
    RUN_TEST(lexer_gt);
    RUN_TEST(lexer_not);
    RUN_TEST(lexer_comma);
    RUN_TEST(lexer_semicolon);
    RUN_TEST(lexer_colon);
    RUN_TEST(lexer_paren);
    RUN_TEST(lexer_brace);
    RUN_TEST(lexer_bracket);
    RUN_TEST(lexer_fn);
    RUN_TEST(lexer_let);
    RUN_TEST(lexer_if);
    RUN_TEST(lexer_else);
    RUN_TEST(lexer_return);
    RUN_TEST(lexer_whitespace);
    RUN_TEST(lexer_comment);
    RUN_TEST(lexer_complex_expression);
    RUN_TEST(lexer_function_decl);
    RUN_TEST(lexer_let_decl);
    RUN_TEST(lexer_if_stmt);
    RUN_TEST(lexer_if_else_stmt);
    
    printf("\n=== 测试结果 ===\n");
    printf("通过: %d / %d\n", tests_passed, tests_run);
    
    return 0;
}