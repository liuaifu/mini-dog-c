/**
 * mini-dog-c 编译器 - 主程序
 *
 * 读取源代码文件，编译并执行
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"

#define MAX_SOURCE_SIZE 65536

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "错误: 无法打开文件 %s\n", filename);
        return NULL;
    }

    char *source = (char *)malloc(MAX_SOURCE_SIZE);
    if (!source) {
        fclose(f);
        return NULL;
    }

    int size = fread(source, 1, MAX_SOURCE_SIZE - 1, f);
    source[size] = '\0';
    fclose(f);

    return source;
}

static void print_usage(const char *prog) {
    printf("用法: %s [选项] <源文件>\n", prog);
    printf("选项:\n");
    printf("  -h, --help    显示帮助信息\n");
    printf("  -t, --tokens  只进行词法分析\n");
    printf("  -a, --ast     显示抽象语法树\n");
    printf("  -v, --verbose 详细输出\n");
}

int main(int argc, char **argv) {
    const char *filename = NULL;
    bool show_tokens = false;
    bool show_ast = false;
    bool verbose = false;

    // 解析命令行参数
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tokens") == 0) {
            show_tokens = true;
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ast") == 0) {
            show_ast = true;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else {
            filename = argv[i];
        }
    }

    if (!filename) {
        fprintf(stderr, "错误: 请指定源文件\n\n");
        print_usage(argv[0]);
        return 1;
    }

    // 读取源代码
    char *source = read_file(filename);
    if (!source) {
        return 1;
    }

    if (verbose) {
        printf("=== 源代码 ===\n%s\n", source);
    }

    // ===== 词法分析 =====
    if (verbose) printf("=== 开始词法分析 ===\n");

    Lexer *lexer = lexer_create(source);
    TokenList *tokens = lexer_tokenize(lexer);

    if (lexer_has_error(lexer)) {
        fprintf(stderr, "词法错误 [%d:%d]: %s\n",
                lexer_error_line(lexer), lexer_error_column(lexer),
                lexer_error_message(lexer));
        token_list_free(tokens);
        lexer_free(lexer);
        free(source);
        return 1;
    }

    if (show_tokens) {
        printf("=== Token 序列 ===\n");
        token_list_print(tokens);
        printf("\n");
        token_list_free(tokens);
        lexer_free(lexer);
        free(source);
        return 0;
    }

    printf("=== 词法分析完成 ===\n");

    // ===== 语法分析 =====
    if (verbose) printf("=== 开始语法分析 ===\n");

    Parser *parser = parser_create(tokens);
    ASTNode *ast = parser_parse(parser);

    if (parser_has_error(parser)) {
        fprintf(stderr, "语法错误: %s\n", parser_error_message(parser));
        ast_free(ast);
        parser_free(parser);
        token_list_free(tokens);
        lexer_free(lexer);
        free(source);
        return 1;
    }

    if (show_ast) {
        printf("\n=== 抽象语法树 ===\n");
        ast_print(ast, 0);
        printf("\n");
    }

    // ===== 解释执行 =====
    if (verbose) printf("=== 开始解释执行 ===\n");

    Evaluator *eval = evaluator_create(ast);
    Value *result = evaluator_run(eval);

    if (evaluator_has_error(eval)) {
        fprintf(stderr, "运行时错误: %s\n", evaluator_error_message(eval));
        if (result) value_free(result);
        evaluator_free(eval);
        ast_free(ast);
        parser_free(parser);
        token_list_free(tokens);
        lexer_free(lexer);
        free(source);
        return 1;
    }

    if (result && result->type != VALUE_NULL) {
        printf("\n=== 执行结果 ===\n");
        printf("> ");
        value_print(result);
        printf("\n");
    } else {
        printf("\n=== 执行完成 ===\n");
    }

    // ===== 清理 =====
    if (result) value_free(result);
    evaluator_free(eval);
    ast_free(ast);
    parser_free(parser);
    token_list_free(tokens);
    lexer_free(lexer);
    free(source);

    return 0;
}
