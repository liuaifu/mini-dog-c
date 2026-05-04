# mini-dog-c

一个用 C 语言实现的小型 C 语言编译器，用于理解编译器的核心工作原理。采用**词法分析 → 语法分析 → 解释执行**的流程，代码简洁，适合作为编译器入门的学习项目。

## 功能特性

- **词法分析器**：支持标识符、关键词、整数/浮点数字面量、字符/字符串字面量、算术/逻辑/比较操作符
- **语法分析器**：递归下降解析器，支持函数定义、变量声明、条件分支、代码块、函数调用
- **解释器**：基于 AST 的解释执行，支持环境作用域链
- **内建函数**：`print(...)`、`println(...)`

## 支持的语法

### 变量与函数

```c
let x = 10;
let name = "mini-dog-c";

fn add(a, b) {
    return a + b;
}
```

### 表达式与操作符

```c
// 算术
let sum = 1 + 2 * 3;        // 优先级：* / 高于 + -

// 比较
if (x >= 0 && x <= 100) {  // < > <= >= == !=
    return 1;
}

// 字符串连接
let greeting = "hello" + " " + "world";  // "hello world"
```

### 控制流

```c
fn factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

### 内建函数

```c
print("x =", x);     // 不换行
println("Result:", x); // 末尾换行
```

## 快速开始

### 编译

```bash
mkdir build
cd build
cmake ..
make
```

### 运行示例

```bash
./mini-dog-c ../tests/example.mdc
```

输出：

```
=== 词法分析完成 ===
Hello, mini-dog-c!
x =10
y =20
x + y =30
factorial(5) =120
factorial(5) > 100

=== 执行完成 ===
```

### 命令行选项

| 选项 | 说明 |
|------|------|
| `-t, --tokens` | 仅进行词法分析，显示 Token 序列 |
| `-a, --ast` | 显示抽象语法树 |
| `-v, --verbose` | 详细输出（显示源代码） |
| `-h, --help` | 显示帮助信息 |

### 运行测试

```bash
make test_lexer      # 词法分析器测试
make test_parser      # 语法分析器测试
make test_evaluator   # 解释器测试
make test             # 全部测试
```

## 项目结构

```
mini-dog-c/
├── src/
│   ├── token.h / token.c    # Token 类型定义
│   ├── lexer.h / lexer.c     # 词法分析器
│   ├── ast.h / ast.c         # 抽象语法树
│   ├── parser.h / parser.c   # 递归下降语法分析器
│   ├── evaluator.h / evaluator.c  # 解释器
│   └── main.c                # 主程序
├── include/
│   └── common.h              # 公共类型、宏、错误处理
├── tests/
│   ├── test_lexer.c
│   ├── test_parser.c
│   ├── test_evaluator.c
│   └── example.mdc           # 示例程序
├── 01-功能需求与设计.md    # 开发博客（系列文章）
├── 02-词法分析器实现.md
├── 03-抽象语法树.md
├── 04-递归下降解析器.md
├── 05-解释器与代码生成.md
└── 06-测试与总结.md
├── CMakeLists.txt
└── README.md
```

## 编译流程

```
源代码 (.mdc)
    ↓
┌─────────────┐
│  词法分析    │  lexer.c
│  源代码 → Token 流  │
└─────────────┘
    ↓
┌─────────────┐
│  语法分析    │  parser.c
│  Token 流 → AST   │
└─────────────┘
    ↓
┌─────────────┐
│  解释执行    │  evaluator.c
│  AST → 程序输出  │
└─────────────┘
```

## 设计决策

- **递归下降解析器**：代码即语法，简单直观，不需要外部工具
- **解释执行**：无需处理目标平台的寄存器、内存布局等问题，专注于展示编译器原理
- **轻量测试框架**：无外部依赖，每个测试独立运行，不互相干扰

## 已知局限

- 不支持负数字面量（`-5` 是一元操作符，不是字面量的一部分）
- 不支持 `&&` 和 `||` 逻辑操作符
- 无类型系统，所有值动态类型
- 无代码优化

## 许可

MIT
