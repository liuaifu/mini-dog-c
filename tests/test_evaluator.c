/**
 * mini-dog-c 编译器 - 解释器测试
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/evaluator.h"

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

static Value *eval_code(const char *code) {
    Lexer *lexer = lexer_create(code);
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    // 不释放，因为可能还在使用
    // 简化处理：所有测试共享同一套资源
    
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
    // ast_free(ast);  // 延迟释放
    evaluator_free(eval);
    ast_free(ast);
    
    return result;
}

#define assert_value_type(v, kind) do { \
    if ((v)->type != (kind)) { \
        printf("FAILED: expected type %d, got %d\n", (kind), (v)->type); \
        exit(1); \
    } \
} while(0)

#define assert_int_value(v, expected) do { \
    if ((v)->type != VALUE_INT || (v)->data.int_value != (expected)) { \
        printf("FAILED: expected %d, got ", (expected)); \
        value_print(v); \
        printf("\n"); \
        exit(1); \
    } \
} while(0)

#define assert_double_value(v, expected) do { \
    if ((v)->type != VALUE_DOUBLE || (v)->data.double_value != (expected)) { \
        printf("FAILED: expected %f, got ", (expected)); \
        value_print(v); \
        printf("\n"); \
        exit(1); \
    } \
} while(0)

#define assert_bool_value(v, expected) do { \
    if ((v)->type != VALUE_BOOL || (v)->data.bool_value != (expected)) { \
        printf("FAILED: expected %s, got ", (expected) ? "true" : "false"); \
        value_print(v); \
        printf("\n"); \
        exit(1); \
    } \
} while(0)

/* ===== 测试用例 ===== */

// 测试整数字面量
TEST(eval_int_literal) {
    Lexer *lexer = lexer_create("return 42;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 42);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试浮点数字面量
TEST(eval_double_literal) {
    Lexer *lexer = lexer_create("return 3.14;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_DOUBLE);
    assert_double_value(result, 3.14);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试布尔字面量 - true
TEST(eval_bool_true) {
    Lexer *lexer = lexer_create("return true;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_BOOL);
    assert_bool_value(result, true);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试布尔字面量 - false
TEST(eval_bool_false) {
    Lexer *lexer = lexer_create("return false;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_BOOL);
    assert_bool_value(result, false);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试变量声明和引用
TEST(eval_var_decl) {
    Lexer *lexer = lexer_create("let x = 42; return x;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 42);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试赋值表达式
TEST(eval_assignment) {
    Lexer *lexer = lexer_create("let x = 10; x = 20; return x;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 20);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试加法
TEST(eval_add) {
    Lexer *lexer = lexer_create("return 1 + 2;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 3);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试减法
TEST(eval_sub) {
    Lexer *lexer = lexer_create("return 5 - 3;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 2);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试乘法
TEST(eval_mul) {
    Lexer *lexer = lexer_create("return 3 * 4;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 12);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试除法
TEST(eval_div) {
    Lexer *lexer = lexer_create("return 10 / 2;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 5);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试运算优先级
TEST(eval_precedence) {
    Lexer *lexer = lexer_create("return 2 + 3 * 4;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    // 应该先计算 3*4=12, 再 2+12=14
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 14);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试括号改变优先级
TEST(eval_paren_precedence) {
    Lexer *lexer = lexer_create("return (2 + 3) * 4;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    // 应该先计算 (2+3)=5, 再 5*4=20
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 20);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试小于比较
TEST(eval_lt) {
    Lexer *lexer = lexer_create("return 3 < 5;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_BOOL);
    assert_bool_value(result, true);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试大于比较
TEST(eval_gt) {
    Lexer *lexer = lexer_create("return 5 > 3;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_BOOL);
    assert_bool_value(result, true);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试等于比较
TEST(eval_eq) {
    Lexer *lexer = lexer_create("return 5 == 5;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_BOOL);
    assert_bool_value(result, true);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试不等于比较
TEST(eval_ne) {
    Lexer *lexer = lexer_create("return 3 != 5;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_BOOL);
    assert_bool_value(result, true);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试负号一元操作符
TEST(eval_unary_minus) {
    Lexer *lexer = lexer_create("return -5;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, -5);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试逻辑非操作符
TEST(eval_not) {
    Lexer *lexer = lexer_create("return !false;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_BOOL);
    assert_bool_value(result, true);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试简单 if 语句
TEST(eval_if_true) {
    Lexer *lexer = lexer_create("if (true) { return 1; } return 0;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 1);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试 if 语句条件为假
TEST(eval_if_false) {
    Lexer *lexer = lexer_create("if (false) { return 1; } return 0;");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 0);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试 if-else 语句
TEST(eval_if_else) {
    Lexer *lexer = lexer_create("if (5 > 3) { return 100; } else { return 200; }");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 100);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试简单函数调用
TEST(eval_fn_call) {
    Lexer *lexer = lexer_create("fn add(a, b) { return a + b; } return add(3, 4);");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 7);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试多层函数调用
TEST(eval_nested_fn_call) {
    Lexer *lexer = lexer_create(
        "fn twice(x) { return x * 2; }"
        "fn four_times(x) { return twice(twice(x)); }"
        "return four_times(3);"
    );
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    // twice(3) = 6, twice(6) = 12
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 12);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试函数作为表达式值
TEST(eval_fn_as_value) {
    Lexer *lexer = lexer_create(
        "fn add(a, b) { return a + b; }"
        "fn apply(f, x, y) { return f(x, y); }"
        "return apply(add, 10, 20);"
    );
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 30);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试字符串连接
TEST(eval_string_concat) {
    Lexer *lexer = lexer_create("return \"hello\" + \" \" + \"world\";");
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    assert_value_type(result, VALUE_STRING);
    assert(strcmp(result->data.string_value.string, "hello world") == 0);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试复杂表达式
TEST(eval_complex_expr) {
    Lexer *lexer = lexer_create(
        "let a = 10;"
        "let b = 20;"
        "let c = 5;"
        "return (a + b) * c - 50;"
    );
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    // (10 + 20) * 5 - 50 = 30 * 5 - 50 = 150 - 50 = 100
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 100);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

// 测试阶乘函数
TEST(eval_factorial) {
    Lexer *lexer = lexer_create(
        "fn fact(n) {"
        "  if (n < 2) { return 1; }"
        "  return n * fact(n - 1);"
        "}"
        "return fact(5);"
    );
    TokenList *tokens = lexer_tokenize(lexer);
    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);
    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);
    
    // 5! = 120
    assert_value_type(result, VALUE_INT);
    assert_int_value(result, 120);
    
    value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    token_list_free(tokens);
    lexer_free(lexer);
    parser_free(parser);
}

/* ===== 主函数 ===== */

int main() {
    printf("=== 解释器测试 ===\n\n");
    
    RUN_TEST(eval_int_literal);
    RUN_TEST(eval_double_literal);
    RUN_TEST(eval_bool_true);
    RUN_TEST(eval_bool_false);
    RUN_TEST(eval_var_decl);
    RUN_TEST(eval_assignment);
    RUN_TEST(eval_add);
    RUN_TEST(eval_sub);
    RUN_TEST(eval_mul);
    RUN_TEST(eval_div);
    RUN_TEST(eval_precedence);
    RUN_TEST(eval_paren_precedence);
    RUN_TEST(eval_lt);
    RUN_TEST(eval_gt);
    RUN_TEST(eval_eq);
    RUN_TEST(eval_ne);
    RUN_TEST(eval_unary_minus);
    RUN_TEST(eval_not);
    RUN_TEST(eval_if_true);
    RUN_TEST(eval_if_false);
    RUN_TEST(eval_if_else);
    RUN_TEST(eval_fn_call);
    RUN_TEST(eval_nested_fn_call);
    RUN_TEST(eval_fn_as_value);
    RUN_TEST(eval_string_concat);
    RUN_TEST(eval_complex_expr);
    RUN_TEST(eval_factorial);
    
    printf("\n=== 测试结果 ===\n");
    printf("通过: %d / %d\n", tests_passed, tests_run);
    
    return 0;
}