/**
 * mini-dog-c 编译器 - Token 实现
 */

#include "token.h"

Token *token_create(TokenType type, const char *lexeme, int line, int column) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) return NULL;
    
    token->type = type;
    token->lexeme = strdup_custom(lexeme);
    token->line = line;
    token->column = column;
    
    return token;
}

void token_free(Token *token) {
    if (!token) return;
    if (token->lexeme) free(token->lexeme);
    if (token->type == TOKEN_STRING_LITERAL && token->value.string_value) {
        free(token->value.string_value);
    }
    free(token);
}

void token_print(const Token *token) {
    if (!token) {
        printf("Token: NULL\n");
        return;
    }
    
    printf("Token { type: %s, lexeme: \"%s\", line: %d, column: %d",
           token_type_to_string(token->type), 
           token->lexeme ? token->lexeme : "",
           token->line, token->column);
    
    // 打印语义值
    switch (token->type) {
        case TOKEN_INT_LITERAL:
            printf(", value: %d", token->value.int_value);
            break;
        case TOKEN_DOUBLE_LITERAL:
            printf(", value: %f", token->value.double_value);
            break;
        case TOKEN_CHAR_LITERAL:
            printf(", value: '%c'", token->value.char_value);
            break;
        case TOKEN_BOOL_LITERAL:
            printf(", value: %s", token->value.bool_value ? "true" : "false");
            break;
        case TOKEN_STRING_LITERAL:
            printf(", value: \"%s\"", token->value.string_value ? token->value.string_value : "");
            break;
        default:
            break;
    }
    
    printf(" }\n");
}