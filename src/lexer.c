/**
 * mini-dog-c 编译器 - 词法分析器实现
 */

#include "lexer.h"
#include <ctype.h>

/* ===== Token 列表实现 ===== */

void token_list_init(TokenList *list) {
    ARRAY_INIT(*list);
}

void token_list_add(TokenList *list, Token *token) {
    ARRAY_PUSH(*list, token);
}

void token_list_free(TokenList *list) {
    for (int i = 0; i < list->size; i++) {
        token_free(list->data[i]);
    }
    ARRAY_FREE(*list);
}

void token_list_print(const TokenList *list) {
    printf("TokenList (%d tokens):\n", list->size);
    for (int i = 0; i < list->size; i++) {
        printf("  [%d] ", i);
        token_print(list->data[i]);
    }
}

/* ===== 辅助函数 ===== */

/**
 * 检查是否到达源代码末尾
 */
static bool is_at_end(const Lexer *lexer) {
    return lexer->position >= lexer->length;
}

/**
 * 获取当前字符
 */
static char current_char(const Lexer *lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->source[lexer->position];
}

/**
 * 查看下一个字符（不前进）
 */
static char peek_next(const Lexer *lexer) {
    if (lexer->position + 1 >= lexer->length) return '\0';
    return lexer->source[lexer->position + 1];
}

/**
 * 消费当前字符并前进
 */
static char advance(Lexer *lexer) {
    char c = lexer->source[lexer->position++];
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

/**
 * 检查当前字符是否匹配
 */
static bool match_char(Lexer *lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (lexer->source[lexer->position] != expected) return false;
    lexer->position++;
    lexer->column++;
    return true;
}

/**
 * 创建错误 Token
 */
static Token *make_error(Lexer *lexer, const char *message) {
    error_set(&lexer->error, message, lexer->line, lexer->column);
    return token_create(TOKEN_UNKNOWN, "", lexer->line, lexer->column);
}

/**
 * 创建 Token
 */
static Token *make_token(Lexer *lexer, TokenType type, const char *start, int length) {
    char *lexeme = (char *)malloc(length + 1);
    if (!lexeme) return NULL;
    strncpy(lexeme, start, length);
    lexeme[length] = '\0';
    
    Token *token = token_create(type, lexeme, lexer->line, lexer->column - length);
    free(lexeme);
    
    return token;
}

/* ===== 词法分析函数 ===== */

/**
 * 跳过空白字符
 */
static void skip_whitespace(Lexer *lexer) {
    while (!is_at_end(lexer)) {
        char c = current_char(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
                advance(lexer);
                break;
            case '/':
                if (peek_next(lexer) == '/') {
                    // 单行注释，跳到行尾
                    while (!is_at_end(lexer) && current_char(lexer) != '\n') {
                        advance(lexer);
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

/**
 * 识别标识符或关键词
 */
static Token *scan_identifier(Lexer *lexer) {
    const char *start = &lexer->source[lexer->position];
    int length = 0;
    
    while (!is_at_end(lexer)) {
        char c = current_char(lexer);
        if (isalnum(c) || c == '_') {
            advance(lexer);
            length++;
        } else {
            break;
        }
    }
    
    Token *token = make_token(lexer, TOKEN_UNKNOWN, start, length);
    
    // 检查是否是关键词
    if (strcmp(token->lexeme, "fn") == 0) {
        token->type = TOKEN_FN;
    } else if (strcmp(token->lexeme, "let") == 0) {
        token->type = TOKEN_LET;
    } else if (strcmp(token->lexeme, "if") == 0) {
        token->type = TOKEN_IF;
    } else if (strcmp(token->lexeme, "else") == 0) {
        token->type = TOKEN_ELSE;
    } else if (strcmp(token->lexeme, "return") == 0) {
        token->type = TOKEN_RETURN;
    } else if (strcmp(token->lexeme, "true") == 0) {
        token->type = TOKEN_BOOL_LITERAL;
        token->value.bool_value = true;
    } else if (strcmp(token->lexeme, "false") == 0) {
        token->type = TOKEN_BOOL_LITERAL;
        token->value.bool_value = false;
    } else {
        token->type = TOKEN_IDENT;
    }
    
    return token;
}

/**
 * 识别数字
 */
static Token *scan_number(Lexer *lexer) {
    const char *start = &lexer->source[lexer->position];
    int length = 0;
    bool has_dot = false;
    
    while (!is_at_end(lexer)) {
        char c = current_char(lexer);
        if (isdigit(c)) {
            advance(lexer);
            length++;
        } else if (c == '.' && !has_dot) {
            // 检查小数点后是否有数字
            if (peek_next(lexer) && isdigit(peek_next(lexer))) {
                has_dot = true;
                advance(lexer);
                length++;
                // 继续读取小数部分
                while (!is_at_end(lexer) && isdigit(current_char(lexer))) {
                    advance(lexer);
                    length++;
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    Token *token = make_token(lexer, TOKEN_INT_LITERAL, start, length);
    
    if (has_dot) {
        token->type = TOKEN_DOUBLE_LITERAL;
        token->value.double_value = atof(start);
    } else {
        token->type = TOKEN_INT_LITERAL;
        token->value.int_value = atoi(start);
    }
    
    return token;
}

/**
 * 识别字符字面量
 */
static Token *scan_char_literal(Lexer *lexer) {
    const char *start = &lexer->source[lexer->position];
    advance(lexer); // 跳过左单引号
    
    char ch;
    if (is_at_end(lexer)) {
        return make_error(lexer, "未闭合的字符字面量");
    }
    
    if (current_char(lexer) == '\\') {
        advance(lexer);
        if (is_at_end(lexer)) {
            return make_error(lexer, "未闭合的转义字符");
        }
        char esc = advance(lexer);
        switch (esc) {
            case 'n': ch = '\n'; break;
            case 't': ch = '\t'; break;
            case 'r': ch = '\r'; break;
            case '\\': ch = '\\'; break;
            case '\'': ch = '\''; break;
            case '0': ch = '\0'; break;
            default: ch = esc; break;
        }
    } else {
        ch = advance(lexer);
    }
    
    if (current_char(lexer) != '\'') {
        return make_error(lexer, "字符字面量缺少右单引号");
    }
    advance(lexer); // 跳过右单引号
    
    Token *token = make_token(lexer, TOKEN_CHAR_LITERAL, start, (int)(&lexer->source[lexer->position] - start));
    token->value.char_value = ch;
    
    return token;
}

/**
 * 识别字符串字面量
 */
static Token *scan_string_literal(Lexer *lexer) {
    const char *start = &lexer->source[lexer->position];
    advance(lexer); // 跳过左双引号
    
    char *buffer = (char *)malloc(256);
    int length = 0;
    int capacity = 256;
    
    while (!is_at_end(lexer) && current_char(lexer) != '"') {
        if (length >= capacity - 1) {
            capacity *= 2;
            buffer = (char *)realloc(buffer, capacity);
        }
        
        char c = current_char(lexer);
        if (c == '\\') {
            advance(lexer);
            if (is_at_end(lexer)) break;
            char esc = advance(lexer);
            switch (esc) {
                case 'n': buffer[length++] = '\n'; break;
                case 't': buffer[length++] = '\t'; break;
                case 'r': buffer[length++] = '\r'; break;
                case '\\': buffer[length++] = '\\'; break;
                case '"': buffer[length++] = '"'; break;
                case '0': buffer[length++] = '\0'; break;
                default: buffer[length++] = esc; break;
            }
        } else {
            buffer[length++] = advance(lexer);
        }
    }
    
    buffer[length] = '\0';
    
    if (is_at_end(lexer)) {
        free(buffer);
        return make_error(lexer, "未闭合的字符串字面量");
    }
    
    advance(lexer); // 跳过右双引号
    
    Token *token = make_token(lexer, TOKEN_STRING_LITERAL, start, (int)(&lexer->source[lexer->position] - start));
    token->value.string_value = buffer;
    
    return token;
}

/**
 * 识别单个字符 Token
 */
static Token *scan_single_token(Lexer *lexer) {
    char c = advance(lexer);
    
    switch (c) {
        case '+': return make_token(lexer, TOKEN_PLUS, &lexer->source[lexer->position - 1], 1);
        case '-': return make_token(lexer, TOKEN_MINUS, &lexer->source[lexer->position - 1], 1);
        case '*': return make_token(lexer, TOKEN_STAR, &lexer->source[lexer->position - 1], 1);
        case '/': return make_token(lexer, TOKEN_SLASH, &lexer->source[lexer->position - 1], 1);
        case '(': return make_token(lexer, TOKEN_LPAREN, &lexer->source[lexer->position - 1], 1);
        case ')': return make_token(lexer, TOKEN_RPAREN, &lexer->source[lexer->position - 1], 1);
        case '{': return make_token(lexer, TOKEN_LBRACE, &lexer->source[lexer->position - 1], 1);
        case '}': return make_token(lexer, TOKEN_RBRACE, &lexer->source[lexer->position - 1], 1);
        case '[': return make_token(lexer, TOKEN_LBRACKET, &lexer->source[lexer->position - 1], 1);
        case ']': return make_token(lexer, TOKEN_RBRACKET, &lexer->source[lexer->position - 1], 1);
        case ',': return make_token(lexer, TOKEN_COMMA, &lexer->source[lexer->position - 1], 1);
        case ';': return make_token(lexer, TOKEN_SEMICOLON, &lexer->source[lexer->position - 1], 1);
        case ':': return make_token(lexer, TOKEN_COLON, &lexer->source[lexer->position - 1], 1);
        default:
            return make_error(lexer, "无法识别的字符");
    }
}

/**
 * 识别双字符操作符
 */
static Token *scan_double_operator(Lexer *lexer) {
    const char *start = &lexer->source[lexer->position];
    
    switch (current_char(lexer)) {
        case '=':
            advance(lexer);
            // 检查是否是 ==
            if (current_char(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_EQ, start, 2);
            }
            return make_token(lexer, TOKEN_ASSIGN, start, 1);
        case '!':
            advance(lexer);
            // 检查是否是 !=
            if (current_char(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_NE, start, 2);
            }
            return make_token(lexer, TOKEN_NOT, start, 1);
        case '<':
            advance(lexer);
            if (current_char(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_LE, start, 2);
            }
            return make_token(lexer, TOKEN_LT, start, 1);
        case '>':
            advance(lexer);
            if (current_char(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_GE, start, 2);
            }
            return make_token(lexer, TOKEN_GT, start, 1);
        default:
            return make_error(lexer, "无法识别的操作符");
    }
}

/* ===== 主词法分析函数 ===== */

static Token *next_token(Lexer *lexer) {
    skip_whitespace(lexer);
    
    if (is_at_end(lexer)) {
        return token_create(TOKEN_EOF, "", lexer->line, lexer->column);
    }
    
    char c = current_char(lexer);
    
    // 标识符或关键词
    if (isalpha(c) || c == '_') {
        return scan_identifier(lexer);
    }
    
    // 数字
    if (isdigit(c)) {
        return scan_number(lexer);
    }
    
    // 字符字面量
    if (c == '\'') {
        return scan_char_literal(lexer);
    }
    
    // 字符串字面量
    if (c == '"') {
        return scan_string_literal(lexer);
    }
    
    // 操作符（可能双字符）
    if (c == '=' || c == '!' || c == '<' || c == '>') {
        return scan_double_operator(lexer);
    }
    
    // 单字符 Token
    return scan_single_token(lexer);
}

TokenList *lexer_tokenize(Lexer *lexer) {
    TokenList *list = (TokenList *)malloc(sizeof(TokenList));
    token_list_init(list);
    
    while (true) {
        Token *token = next_token(lexer);
        token_list_add(list, token);
        
        if (token->type == TOKEN_EOF || token->type == TOKEN_UNKNOWN) {
            break;
        }
    }
    
    return list;
}

/* ===== 词法分析器生命周期 ===== */

Lexer *lexer_create(const char *source) {
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->length = strlen(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    error_init(&lexer->error);
    
    return lexer;
}

void lexer_free(Lexer *lexer) {
    if (lexer) free(lexer);
}

bool lexer_has_error(const Lexer *lexer) {
    return lexer->error.has_error;
}

const char *lexer_error_message(const Lexer *lexer) {
    return lexer->error.message;
}

int lexer_error_line(const Lexer *lexer) {
    return lexer->error.line;
}

int lexer_error_column(const Lexer *lexer) {
    return lexer->error.column;
}