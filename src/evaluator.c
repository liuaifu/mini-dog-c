/**
 * mini-dog-c 编译器 - 解释器实现
 * 
 * 简化版本：每个表达式求值返回的值由调用者负责释放
 */

#include "evaluator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ===== 变量环境 ===== */

#define ENV_VAR_MAX 256

struct Env {
    Env *parent;
    char *names[ENV_VAR_MAX];
    Value *values[ENV_VAR_MAX];
    int count;
};

Env *env_create(Env *parent) {
    Env *env = (Env *)malloc(sizeof(Env));
    env->parent = parent;
    env->count = 0;
    for (int i = 0; i < ENV_VAR_MAX; i++) {
        env->names[i] = NULL;
        env->values[i] = NULL;
    }
    return env;
}

void env_free(Env *env) {
    if (!env) return;
    for (int i = 0; i < env->count; i++) {
        if (env->names[i]) free(env->names[i]);
        // 不释放 values，因为可能有多重引用
    }
    free(env);
}

bool env_define(Env *env, const char *name, Value *value) {
    if (env->count >= ENV_VAR_MAX) return false;
    env->names[env->count] = strdup_custom(name);
    env->values[env->count] = value;
    env->count++;
    return true;
}

Value *env_get(Env *env, const char *name) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], name) == 0) return env->values[i];
    }
    if (env->parent) return env_get(env->parent, name);
    return NULL;
}

bool env_set(Env *env, const char *name, Value *value) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->names[i], name) == 0) {
            env->values[i] = value;
            return true;
        }
    }
    return env_define(env, name, value);
}

/* ===== 值操作 ===== */

static Value *alloc_value(void) {
    return (Value *)malloc(sizeof(Value));
}

Value *value_create_int(int v) {
    Value *value = alloc_value();
    value->type = VALUE_INT;
    value->data.int_value = v;
    return value;
}

Value *value_create_double(double v) {
    Value *value = alloc_value();
    value->type = VALUE_DOUBLE;
    value->data.double_value = v;
    return value;
}

Value *value_create_char(char v) {
    Value *value = alloc_value();
    value->type = VALUE_CHAR;
    value->data.char_value = v;
    return value;
}

Value *value_create_bool(bool v) {
    Value *value = alloc_value();
    value->type = VALUE_BOOL;
    value->data.bool_value = v;
    return value;
}

Value *value_create_string(const char *str) {
    Value *value = alloc_value();
    value->type = VALUE_STRING;
    value->data.string_value.string = strdup_custom(str);
    value->data.string_value.length = strlen(str);
    return value;
}

Value *value_create_null(void) {
    Value *value = alloc_value();
    value->type = VALUE_NULL;
    return value;
}

Value *value_create_function(const char *name, char **params, int param_count, ASTNode *body, void *closure) {
    Value *value = alloc_value();
    value->type = VALUE_FUNCTION;
    value->data.function.name = strdup_custom(name);
    value->data.function.params = params;
    value->data.function.param_count = param_count;
    value->data.function.body = body;
    value->data.function.closure = closure;
    return value;
}

void value_free(Value *value) {
    if (!value) return;
    if (value->type == VALUE_STRING) free(value->data.string_value.string);
    else if (value->type == VALUE_FUNCTION) free(value->data.function.name);
    free(value);
}

void value_print(const Value *value) {
    if (!value) { printf("null"); return; }
    switch (value->type) {
        case VALUE_INT: printf("%d", value->data.int_value); break;
        case VALUE_DOUBLE: printf("%f", value->data.double_value); break;
        case VALUE_CHAR: printf("%c", value->data.char_value); break;
        case VALUE_BOOL: printf("%s", value->data.bool_value ? "true" : "false"); break;
        case VALUE_STRING: printf("%s", value->data.string_value.string); break;
        case VALUE_FUNCTION: printf("<function %s>", value->data.function.name); break;
        case VALUE_NULL: printf("null"); break;
    }
}

/* ===== 前置声明 ===== */
static Value *eval_expression(Evaluator *eval, Env *env, ASTNode *node);
static Value *eval_block(Evaluator *eval, Env *env, ASTNode *block);

/**
 * 求值二元表达式
 */
static Value *eval_binary(Evaluator *eval, Env *env, TokenType op, ASTNode *left_node, ASTNode *right_node) {
    Value *left = eval_expression(eval, env, left_node);
    Value *right = eval_expression(eval, env, right_node);
    if (!left || !right) {
        if (left) value_free(left);
        if (right) value_free(right);
        return value_create_null();
    }
    
    Value *result = NULL;
    switch (op) {
        case TOKEN_PLUS:
            if (left->type == VALUE_STRING && right->type == VALUE_STRING) {
                size_t len = strlen(left->data.string_value.string) + strlen(right->data.string_value.string) + 1;
                char *buf = (char *)malloc(len);
                strcpy(buf, left->data.string_value.string);
                strcat(buf, right->data.string_value.string);
                result = value_create_string(buf);
                free(buf);
            } else if (left->type == VALUE_INT && right->type == VALUE_INT) {
                result = value_create_int(left->data.int_value + right->data.int_value);
            } else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_double(lv + rv);
            }
            break;
        case TOKEN_MINUS:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_int(left->data.int_value - right->data.int_value);
            else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_double(lv - rv);
            }
            break;
        case TOKEN_STAR:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_int(left->data.int_value * right->data.int_value);
            else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_double(lv * rv);
            }
            break;
        case TOKEN_SLASH:
            if (right->type == VALUE_INT && right->data.int_value == 0)
                result = value_create_null();
            else if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_int(left->data.int_value / right->data.int_value);
            else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_double(lv / rv);
            }
            break;
        case TOKEN_EQ:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_bool(left->data.int_value == right->data.int_value);
            else if (left->type == VALUE_BOOL && right->type == VALUE_BOOL)
                result = value_create_bool(left->data.bool_value == right->data.bool_value);
            else
                result = value_create_bool(false);
            break;
        case TOKEN_NE:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_bool(left->data.int_value != right->data.int_value);
            else if (left->type == VALUE_BOOL && right->type == VALUE_BOOL)
                result = value_create_bool(left->data.bool_value != right->data.bool_value);
            else
                result = value_create_bool(true);
            break;
        case TOKEN_LT:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_bool(left->data.int_value < right->data.int_value);
            else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_bool(lv < rv);
            }
            break;
        case TOKEN_GT:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_bool(left->data.int_value > right->data.int_value);
            else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_bool(lv > rv);
            }
            break;
        case TOKEN_LE:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_bool(left->data.int_value <= right->data.int_value);
            else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_bool(lv <= rv);
            }
            break;
        case TOKEN_GE:
            if (left->type == VALUE_INT && right->type == VALUE_INT)
                result = value_create_bool(left->data.int_value >= right->data.int_value);
            else {
                double lv = left->type == VALUE_INT ? left->data.int_value : left->data.double_value;
                double rv = right->type == VALUE_INT ? right->data.int_value : right->data.double_value;
                result = value_create_bool(lv >= rv);
            }
            break;
        default:
            result = value_create_null();
    }
    // 统一释放操作数
    value_free(left);
    value_free(right);
    return result;
}

/**
 * 求值一元表达式
 */
static Value *eval_unary(Evaluator *eval, Env *env, TokenType op, ASTNode *operand_node) {
    Value *operand = eval_expression(eval, env, operand_node);
    if (!operand) return value_create_null();
    
    Value *result = NULL;
    if (op == TOKEN_NOT) {
        if (operand->type == VALUE_BOOL)
            result = value_create_bool(!operand->data.bool_value);
        else if (operand->type == VALUE_INT)
            result = value_create_bool(operand->data.int_value == 0);
        else
            result = value_create_bool(true);
    } else if (op == TOKEN_MINUS) {
        if (operand->type == VALUE_INT)
            result = value_create_int(-operand->data.int_value);
        else if (operand->type == VALUE_DOUBLE)
            result = value_create_double(-operand->data.double_value);
        else
            result = value_create_int(0);
    } else {
        result = value_create_null();
    }
    value_free(operand);
    return result;
}

/* ===== 内建函数 ===== */

static bool streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

/**
 * 求值内建函数，返回 NULL 表示不是内建函数
 */
static Value *eval_builtin(Evaluator *eval, Env *env, const char *name, ASTNode **args, int arg_count) {
    if (streq(name, "print")) {
        for (int i = 0; i < arg_count; i++) {
            Value *v = eval_expression(eval, env, args[i]);
            if (v) { value_print(v); value_free(v); }
        }
        return value_create_null();
    }
    if (streq(name, "println")) {
        for (int i = 0; i < arg_count; i++) {
            Value *v = eval_expression(eval, env, args[i]);
            if (v) { value_print(v); value_free(v); }
        }
        printf("\n");
        return value_create_null();
    }
    return NULL;  // 不是内建函数
}

/**
 * 求值函数调用 - 简化版：使用调用者的环境
 */
static Value *eval_call(Evaluator *eval, Env *env, const char *name, ASTNode **args, int arg_count) {
    // 先尝试内建函数
    Value *builtin = eval_builtin(eval, env, name, args, arg_count);
    if (builtin != NULL) return builtin;

    Value *func = env_get(env, name);
    if (!func || func->type != VALUE_FUNCTION) return value_create_null();
    
    // 创建新的局部环境，父环境指向调用者的环境
    Env *local_env = env_create(env);
    
    // 绑定参数
    for (int i = 0; i < arg_count && i < func->data.function.param_count; i++) {
        Value *arg_val = eval_expression(eval, env, args[i]);
        env_define(local_env, func->data.function.params[i], arg_val);
    }
    
    // 执行函数体
    Value *result = eval_block(eval, local_env, func->data.function.body);
    
    env_free(local_env);
    return result ? result : value_create_null();
}

/**
 * 求值代码块
 */
static Value *eval_block(Evaluator *eval, Env *env, ASTNode *block) {
    if (!block || block->type != AST_BLOCK_STMT) return value_create_null();
    
    Value *result = NULL;
    for (int i = 0; i < block->data.block.statement_count; i++) {
        ASTNode *stmt = block->data.block.statements[i];
        
        switch (stmt->type) {
            case AST_VAR_DECL: {
                Value *init = eval_expression(eval, env, stmt->data.var_decl.initializer);
                if (result) value_free(result);
                result = NULL;  // var decl 没有返回值
                env_define(env, stmt->data.var_decl.name, init);
                break;
            }
            case AST_RETURN_STMT:
                if (result) value_free(result);
                return stmt->data.return_stmt.value 
                    ? eval_expression(eval, env, stmt->data.return_stmt.value)
                    : value_create_null();
            case AST_IF_STMT: {
                Value *cond = eval_expression(eval, env, stmt->data.if_stmt.condition);
                bool cond_true = (cond && cond->type == VALUE_BOOL && cond->data.bool_value);
                if (cond) value_free(cond);
                ASTNode *branch = cond_true ? stmt->data.if_stmt.then_branch : stmt->data.if_stmt.else_branch;
                if (result) value_free(result);
                if (branch) {
                    return eval_block(eval, env, branch);
                } else {
                    result = value_create_null();
                }
                break;
            }
            case AST_EXPR_STMT:
                if (result) value_free(result);
                result = stmt->data.return_stmt.value 
                    ? eval_expression(eval, env, stmt->data.return_stmt.value)
                    : value_create_null();
                break;
            case AST_FN_DECL: {
                Value *fn = value_create_function(
                    stmt->data.fn_decl.name,
                    stmt->data.fn_decl.params,
                    stmt->data.fn_decl.param_count,
                    stmt->data.fn_decl.body,
                    NULL  // 简化：不保存闭包
                );
                env_define(env, stmt->data.fn_decl.name, fn);
                break;
            }
            default:
                if (result) value_free(result);
                result = value_create_null();
        }
    }
    return result ? result : value_create_null();
}

/**
 * 求值表达式
 */
static Value *eval_expression(Evaluator *eval, Env *env, ASTNode *node) {
    if (!node) return value_create_null();
    
    switch (node->type) {
        case AST_LITERAL_INT: return value_create_int(node->data.int_value);
        case AST_LITERAL_DOUBLE: return value_create_double(node->data.double_value);
        case AST_LITERAL_CHAR: return value_create_char(node->data.char_value);
        case AST_LITERAL_BOOL: return value_create_bool(node->data.bool_value);
        case AST_LITERAL_STRING: return value_create_string(node->data.string_value);
        case AST_IDENTIFIER: {
            Value *v = env_get(env, node->data.identifier.name);
            if (!v) return value_create_null();
            // 返回值的副本，所有权归调用者
            switch (v->type) {
                case VALUE_INT: return value_create_int(v->data.int_value);
                case VALUE_DOUBLE: return value_create_double(v->data.double_value);
                case VALUE_CHAR: return value_create_char(v->data.char_value);
                case VALUE_BOOL: return value_create_bool(v->data.bool_value);
                case VALUE_STRING: return value_create_string(v->data.string_value.string);
                case VALUE_FUNCTION: {
                    Value *copy = value_create_function(
                        v->data.function.name,
                        v->data.function.params,
                        v->data.function.param_count,
                        v->data.function.body,
                        v->data.function.closure
                    );
                    return copy;
                }
                default: return value_create_null();
            }
        }
        case AST_BINARY_EXPR:
            return eval_binary(eval, env, node->data.binary_expr.op, 
                              node->data.binary_expr.left, node->data.binary_expr.right);
        case AST_UNARY_EXPR:
            return eval_unary(eval, env, node->data.unary_expr.op, node->data.unary_expr.operand);
        case AST_ASSIGN_EXPR: {
            Value *v = eval_expression(eval, env, node->data.assign_expr.value);
            if (env_set(env, node->data.assign_expr.name, v)) {
                // env_set 成功，值已存入环境，caller 不 free，直接返回环境中的值
                Value *stored = env_get(env, node->data.assign_expr.name);
                return stored ? stored : value_create_null();
            } else {
                // env_set 失败，需要自己 free
                value_free(v);
                return value_create_null();
            }
        }
        case AST_CALL_EXPR:
            return eval_call(eval, env, node->data.call_expr.name,
                           node->data.call_expr.args, node->data.call_expr.arg_count);
        default: return value_create_null();
    }
}

/* ===== 解释器生命周期 ===== */

Evaluator *evaluator_create(ASTNode *ast) {
    Evaluator *eval = (Evaluator *)malloc(sizeof(Evaluator));
    eval->ast = ast;
    eval->global_env = env_create(NULL);
    eval->error.has_error = false;
    eval->error.message[0] = '\0';
    return eval;
}

void evaluator_free(Evaluator *eval) {
    if (!eval) return;
    env_free(eval->global_env);
    free(eval);
}

Value *evaluator_run(Evaluator *eval) {
    if (!eval || !eval->ast) return NULL;
    
    for (int i = 0; i < eval->ast->data.program.statement_count; i++) {
        ASTNode *stmt = eval->ast->data.program.statements[i];
        
        switch (stmt->type) {
            case AST_VAR_DECL: {
                Value *init = eval_expression(eval, eval->global_env, stmt->data.var_decl.initializer);
                env_define(eval->global_env, stmt->data.var_decl.name, init);
                break;
            }
            case AST_FN_DECL: {
                Value *fn = value_create_function(
                    stmt->data.fn_decl.name,
                    stmt->data.fn_decl.params,
                    stmt->data.fn_decl.param_count,
                    stmt->data.fn_decl.body,
                    NULL
                );
                env_define(eval->global_env, stmt->data.fn_decl.name, fn);
                break;
            }
            case AST_EXPR_STMT: {
                Value *r = eval_expression(eval, eval->global_env, stmt->data.return_stmt.value);
                // 不要 free r，因为赋值表达式的结果可能已被存入环境，env 会负责释放
                break;
            }
            case AST_RETURN_STMT:
                return stmt->data.return_stmt.value
                    ? eval_expression(eval, eval->global_env, stmt->data.return_stmt.value)
                    : value_create_null();
            case AST_IF_STMT: {
                Value *cond = eval_expression(eval, eval->global_env, stmt->data.if_stmt.condition);
                bool cond_true = (cond && cond->type == VALUE_BOOL && cond->data.bool_value);
                if (cond) value_free(cond);
                ASTNode *branch = cond_true ? stmt->data.if_stmt.then_branch : stmt->data.if_stmt.else_branch;
                if (branch) return eval_block(eval, eval->global_env, branch);
                break;
            }
            case AST_BLOCK_STMT:
                return eval_block(eval, eval->global_env, stmt);
            default: break;
        }
    }

    // 自动调用 main 函数（如果存在）
    Value *main_fn = env_get(eval->global_env, "main");
    if (main_fn && main_fn->type == VALUE_FUNCTION) {
        Value *main_result = eval_call(eval, eval->global_env, "main", NULL, 0);
        if (main_result) { value_free(main_result); }
    }

    return value_create_null();
}

bool evaluator_has_error(const Evaluator *eval) {
    return eval && eval->error.has_error;
}

const char *evaluator_error_message(const Evaluator *eval) {
    return eval ? eval->error.message : "";
}