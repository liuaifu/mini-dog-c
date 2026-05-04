/**
 * mini-dog-c 编译器 - AST 实现
 */

#include "ast.h"

/* ===== 内存分配 ===== */

static void *ast_alloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "AST: 内存分配失败\n");
        exit(1);
    }
    return ptr;
}

/* ===== AST 节点创建 ===== */

ASTNode *ast_create_program(void) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_PROGRAM;
    node->token = NULL;
    node->parent = NULL;
    node->data.program.statements = (ASTNode **)ast_alloc(sizeof(ASTNode *) * 16);
    node->data.program.statement_count = 0;
    return node;
}

ASTNode *ast_create_var_decl(const char *name, ASTNode *initializer) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_VAR_DECL;
    node->token = NULL;
    node->parent = NULL;
    node->data.var_decl.name = strdup_custom(name);
    node->data.var_decl.initializer = initializer;
    if (initializer) initializer->parent = node;
    return node;
}

ASTNode *ast_create_fn_decl(const char *name, char **params, int param_count, ASTNode *body) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_FN_DECL;
    node->token = NULL;
    node->parent = NULL;
    node->data.fn_decl.name = strdup_custom(name);
    node->data.fn_decl.param_count = param_count;
    node->data.fn_decl.params = (char **)ast_alloc(sizeof(char *) * param_count);
    for (int i = 0; i < param_count; i++) {
        node->data.fn_decl.params[i] = strdup_custom(params[i]);
    }
    node->data.fn_decl.body = body;
    if (body) body->parent = node;
    return node;
}

ASTNode *ast_create_return(ASTNode *value) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_RETURN_STMT;
    node->token = NULL;
    node->parent = NULL;
    node->data.return_stmt.value = value;
    if (value) value->parent = node;
    return node;
}

ASTNode *ast_create_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_IF_STMT;
    node->token = NULL;
    node->parent = NULL;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    if (condition) condition->parent = node;
    if (then_branch) then_branch->parent = node;
    if (else_branch) else_branch->parent = node;
    return node;
}

ASTNode *ast_create_block(ASTNode **statements, int count) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_BLOCK_STMT;
    node->token = NULL;
    node->parent = NULL;
    node->data.block.statements = (ASTNode **)ast_alloc(sizeof(ASTNode *) * (count > 0 ? count : 16));
    node->data.block.statement_count = 0;
    for (int i = 0; i < count; i++) {
        node->data.block.statements[node->data.block.statement_count++] = statements[i];
        if (statements[i]) statements[i]->parent = node;
    }
    return node;
}

ASTNode *ast_create_binary(TokenType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_BINARY_EXPR;
    node->token = NULL;
    node->parent = NULL;
    node->data.binary_expr.op = op;
    node->data.binary_expr.left = left;
    node->data.binary_expr.right = right;
    if (left) left->parent = node;
    if (right) right->parent = node;
    return node;
}

ASTNode *ast_create_unary(TokenType op, ASTNode *operand) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_UNARY_EXPR;
    node->token = NULL;
    node->parent = NULL;
    node->data.unary_expr.op = op;
    node->data.unary_expr.operand = operand;
    if (operand) operand->parent = node;
    return node;
}

ASTNode *ast_create_assign(const char *name, ASTNode *value) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_ASSIGN_EXPR;
    node->token = NULL;
    node->parent = NULL;
    node->data.assign_expr.name = strdup_custom(name);
    node->data.assign_expr.value = value;
    if (value) value->parent = node;
    return node;
}

ASTNode *ast_create_call(const char *name, ASTNode **args, int arg_count) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_CALL_EXPR;
    node->token = NULL;
    node->parent = NULL;
    node->data.call_expr.name = strdup_custom(name);
    node->data.call_expr.arg_count = arg_count;
    node->data.call_expr.args = (ASTNode **)ast_alloc(sizeof(ASTNode *) * arg_count);
    for (int i = 0; i < arg_count; i++) {
        node->data.call_expr.args[i] = args[i];
        if (args[i]) args[i]->parent = node;
    }
    return node;
}

ASTNode *ast_create_identifier(const char *name) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->token = NULL;
    node->parent = NULL;
    node->data.identifier.name = strdup_custom(name);
    return node;
}

ASTNode *ast_create_int(int value) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_LITERAL_INT;
    node->token = NULL;
    node->parent = NULL;
    node->data.int_value = value;
    return node;
}

ASTNode *ast_create_double(double value) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_LITERAL_DOUBLE;
    node->token = NULL;
    node->parent = NULL;
    node->data.double_value = value;
    return node;
}

ASTNode *ast_create_char(char value) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_LITERAL_CHAR;
    node->token = NULL;
    node->parent = NULL;
    node->data.char_value = value;
    return node;
}

ASTNode *ast_create_bool(bool value) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_LITERAL_BOOL;
    node->token = NULL;
    node->parent = NULL;
    node->data.bool_value = value;
    return node;
}

ASTNode *ast_create_string(const char *value) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_LITERAL_STRING;
    node->token = NULL;
    node->parent = NULL;
    node->data.string_value = strdup_custom(value);
    return node;
}

ASTNode *ast_create_expr_stmt(ASTNode *expr) {
    ASTNode *node = (ASTNode *)ast_alloc(sizeof(ASTNode));
    node->type = AST_EXPR_STMT;
    node->token = NULL;
    node->parent = NULL;
    node->data.return_stmt.value = expr;  // 复用 union 中的 value 字段
    if (expr) expr->parent = node;
    return node;
}

/* ===== AST 节点操作 ===== */

static int program_capacity = 16;

void ast_program_add_stmt(ASTNode *program, ASTNode *stmt) {
    if (!program || program->type != AST_PROGRAM) return;
    
    // 扩容
    if (program->data.program.statement_count >= program_capacity) {
        program_capacity *= 2;
        program->data.program.statements = (ASTNode **)realloc(
            program->data.program.statements,
            sizeof(ASTNode *) * program_capacity
        );
    }
    
    program->data.program.statements[program->data.program.statement_count++] = stmt;
    if (stmt) stmt->parent = program;
}

static int block_capacity = 16;

void ast_block_add_stmt(ASTNode *block, ASTNode *stmt) {
    if (!block || block->type != AST_BLOCK_STMT) return;
    
    if (block->data.block.statement_count >= block_capacity) {
        block_capacity *= 2;
        block->data.block.statements = (ASTNode **)realloc(
            block->data.block.statements,
            sizeof(ASTNode *) * block_capacity
        );
    }
    
    block->data.block.statements[block->data.block.statement_count++] = stmt;
    if (stmt) stmt->parent = block;
}

/* ===== AST 释放 ===== */

void ast_free(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_PROGRAM:
            for (int i = 0; i < node->data.program.statement_count; i++) {
                ast_free(node->data.program.statements[i]);
            }
            free(node->data.program.statements);
            break;
            
        case AST_VAR_DECL:
            free(node->data.var_decl.name);
            ast_free(node->data.var_decl.initializer);
            break;
            
        case AST_FN_DECL:
            free(node->data.fn_decl.name);
            for (int i = 0; i < node->data.fn_decl.param_count; i++) {
                free(node->data.fn_decl.params[i]);
            }
            free(node->data.fn_decl.params);
            ast_free(node->data.fn_decl.body);
            break;
            
        case AST_RETURN_STMT:
            ast_free(node->data.return_stmt.value);
            break;
            
        case AST_IF_STMT:
            ast_free(node->data.if_stmt.condition);
            ast_free(node->data.if_stmt.then_branch);
            ast_free(node->data.if_stmt.else_branch);
            break;
            
        case AST_BLOCK_STMT:
            for (int i = 0; i < node->data.block.statement_count; i++) {
                ast_free(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
            
        case AST_BINARY_EXPR:
            ast_free(node->data.binary_expr.left);
            ast_free(node->data.binary_expr.right);
            break;
            
        case AST_UNARY_EXPR:
            ast_free(node->data.unary_expr.operand);
            break;
            
        case AST_ASSIGN_EXPR:
            free(node->data.assign_expr.name);
            ast_free(node->data.assign_expr.value);
            break;
            
        case AST_CALL_EXPR:
            free(node->data.call_expr.name);
            for (int i = 0; i < node->data.call_expr.arg_count; i++) {
                ast_free(node->data.call_expr.args[i]);
            }
            free(node->data.call_expr.args);
            break;
            
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
            
        case AST_LITERAL_STRING:
            free(node->data.string_value);
            break;
            
        default:
            break;
    }
    
    free(node);
}

/* ===== AST 打印（调试用） ===== */

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

static const char *ast_node_type_to_string(ASTNodeType type) {
    switch (type) {
        case AST_PROGRAM: return "Program";
        case AST_VAR_DECL: return "VarDecl";
        case AST_FN_DECL: return "FnDecl";
        case AST_EXPR_STMT: return "ExprStmt";
        case AST_RETURN_STMT: return "ReturnStmt";
        case AST_IF_STMT: return "IfStmt";
        case AST_BLOCK_STMT: return "Block";
        case AST_IDENTIFIER: return "Ident";
        case AST_LITERAL_INT: return "IntLiteral";
        case AST_LITERAL_DOUBLE: return "DoubleLiteral";
        case AST_LITERAL_CHAR: return "CharLiteral";
        case AST_LITERAL_BOOL: return "BoolLiteral";
        case AST_LITERAL_STRING: return "StringLiteral";
        case AST_BINARY_EXPR: return "BinaryExpr";
        case AST_UNARY_EXPR: return "UnaryExpr";
        case AST_ASSIGN_EXPR: return "AssignExpr";
        case AST_CALL_EXPR: return "CallExpr";
        default: return "Unknown";
    }
}

static const char *op_to_string(TokenType op) {
    switch (op) {
        case TOKEN_PLUS: return "+";
        case TOKEN_MINUS: return "-";
        case TOKEN_STAR: return "*";
        case TOKEN_SLASH: return "/";
        case TOKEN_NOT: return "!";
        case TOKEN_EQ: return "==";
        case TOKEN_NE: return "!=";
        case TOKEN_LT: return "<";
        case TOKEN_GT: return ">";
        default: return "?";
    }
}

void ast_print(const ASTNode *node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }
    
    print_indent(indent);
    
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program (%d statements):\n", node->data.program.statement_count);
            for (int i = 0; i < node->data.program.statement_count; i++) {
                ast_print(node->data.program.statements[i], indent + 1);
            }
            break;
            
        case AST_VAR_DECL:
            printf("VarDecl: %s\n", node->data.var_decl.name);
            if (node->data.var_decl.initializer) {
                ast_print(node->data.var_decl.initializer, indent + 1);
            }
            break;
            
        case AST_FN_DECL:
            printf("FnDecl: %s (params: %d)\n", node->data.fn_decl.name, node->data.fn_decl.param_count);
            for (int i = 0; i < node->data.fn_decl.param_count; i++) {
                print_indent(indent + 1);
                printf("  param: %s\n", node->data.fn_decl.params[i]);
            }
            ast_print(node->data.fn_decl.body, indent + 1);
            break;
            
        case AST_RETURN_STMT:
            printf("ReturnStmt:\n");
            ast_print(node->data.return_stmt.value, indent + 1);
            break;
            
        case AST_IF_STMT:
            printf("IfStmt:\n");
            print_indent(indent + 1);
            printf("condition:\n");
            ast_print(node->data.if_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("then:\n");
            ast_print(node->data.if_stmt.then_branch, indent + 2);
            if (node->data.if_stmt.else_branch) {
                print_indent(indent + 1);
                printf("else:\n");
                ast_print(node->data.if_stmt.else_branch, indent + 2);
            }
            break;
            
        case AST_BLOCK_STMT:
            printf("Block (%d statements):\n", node->data.block.statement_count);
            for (int i = 0; i < node->data.block.statement_count; i++) {
                ast_print(node->data.block.statements[i], indent + 1);
            }
            break;
            
        case AST_BINARY_EXPR:
            printf("BinaryExpr: %s\n", op_to_string(node->data.binary_expr.op));
            ast_print(node->data.binary_expr.left, indent + 1);
            ast_print(node->data.binary_expr.right, indent + 1);
            break;
            
        case AST_UNARY_EXPR:
            printf("UnaryExpr: %s\n", op_to_string(node->data.unary_expr.op));
            ast_print(node->data.unary_expr.operand, indent + 1);
            break;
            
        case AST_ASSIGN_EXPR:
            printf("AssignExpr: %s\n", node->data.assign_expr.name);
            ast_print(node->data.assign_expr.value, indent + 1);
            break;
            
        case AST_CALL_EXPR:
            printf("CallExpr: %s (args: %d)\n", node->data.call_expr.name, node->data.call_expr.arg_count);
            for (int i = 0; i < node->data.call_expr.arg_count; i++) {
                ast_print(node->data.call_expr.args[i], indent + 1);
            }
            break;
            
        case AST_IDENTIFIER:
            printf("Ident: %s\n", node->data.identifier.name);
            break;
            
        case AST_LITERAL_INT:
            printf("IntLiteral: %d\n", node->data.int_value);
            break;
            
        case AST_LITERAL_DOUBLE:
            printf("DoubleLiteral: %f\n", node->data.double_value);
            break;
            
        case AST_LITERAL_CHAR:
            printf("CharLiteral: '%c'\n", node->data.char_value);
            break;
            
        case AST_LITERAL_BOOL:
            printf("BoolLiteral: %s\n", node->data.bool_value ? "true" : "false");
            break;
            
        case AST_LITERAL_STRING:
            printf("StringLiteral: \"%s\"\n", node->data.string_value);
            break;
            
        default:
            printf("Unknown node type: %d\n", node->type);
            break;
    }
}