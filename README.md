# Olive Lang
## Prerequisites
- cmake
- libboost-dev
- libspdlog-dev
### Building unit tests
- cmake-format
- gcovr
- libgtest-dev
- valgrind (See [known issues](#known-issues))

## CMake Configuration
### Debug
From the root of the project run the following command, replacing the compiler filepaths with your desired compiler.
```bash
cmake \
    -DCMAKE_BUILD_TYPE:STRING=Debug \
    -DCMAKE_INSTALL_PREFIX:STRING=./install \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc-12 \
    -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++-12 \
    --no-warn-unused-cli \
    -G "Unix Makefiles" \
    -S. -Bbuild
```

### Release
From the root of the project run the following command, replacing the compiler filepaths with your desired compiler.
```bash
cmake \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DCMAKE_INSTALL_PREFIX:STRING=./install \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc-12 \
    -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++-12 \
    --no-warn-unused-cli \
    -G "Unix Makefiles" \
    -S. -Bbuild
```

### Configuring to build unit tests
Add the following option to one of the above configuration commands.
```bash
-DBUILD_TESTING:BOOL=TRUE
```

## Targets
- all - Build targets
- install - Install targets
- coverage - Generate code coverage report

## Building
### Debug
From the root of the project, run the following command:
```bash
cmake --build build --config Debug --target all --
```

### Release
From the root of the project, run the following command:
```bash
cmake --build build --config Release --target all --
```

## Installing
### Debug
From the root of the project, run the following command:
```bash
cmake --build build --config Debug --target install --
```

### Release
From the root of the project, run the following command:
```bash
cmake --build build --config Release --target install --
```

## Running tests
From the build directory run ctest:
```bash
ctest -V
```

### Generating Test Coverage Report
From the root of the project, build the coverage target  
**Note:** Run unit tests before generating coverage report
```bash
cmake --build build --config Debug --target coverage --
```


## Running
### Not Installed
From the root of the project, run the following command:
```bash
./build/pimento --help
```

### Installed
From the root of the project, run the following command:
```bash
./install/bin/pimento --help
```

## Known Issues
- Precedence climbing for order of operations not currently working quite right.
- If building with Clang >= 14, you need valgrind >= 3.20 in order to run the memcheck unit tests.
    - https://bugs.kde.org/show_bug.cgi?id=452758

# Grammar
## Lexical Grammar (Regular)
**Subject to change**
$$
\begin{align}
    \text{exit} &\to \text{EXIT} \\
    \text{if} &\to \text{IF} \\
    \text{elif} &\to \text{ELIF} \\
    \text{else} &\to \text{ELSE} \\
    \text{while} &\to \text{WHILE} \\
    \text{uint8} &\to \text{UINT8} \\
    \text{uint16} &\to \text{UINT16} \\
    \text{uint32} &\to \text{UINT32} \\
    \text{uint64} &\to \text{UINT64} \\
    \text{int8} &\to \text{INT8} \\
    \text{int16} &\to \text{INT16} \\
    \text{int32} &\to \text{INT32} \\
    \text{int64} &\to \text{INT64} \\
    \text{float} &\to \text{FLOAT} \\
    \text{string} &\to \text{STRING} \\
    \text{func} &\to \text{FUNCTION} \\
    \text{class} &\to \text{CLASS} \\
    \text{iface} &\to \text{INTERFACE} \\
    \text{mut} &\to \text{MUTABLE} \\
    \text{\{} &\to \text{LEFT_CURLY} \\
    \text{(} &\to \text{LEFT_PAREN} \\
    \text{[} &\to \text{LEFT_SQUARE} \\
    \text{\}} &\to \text{RIGHT_CURLY} \\
    \text{)} &\to \text{RIGHT_PAREN} \\
    \text{]} &\to \text{RIGHT_SQUARE} \\
    \text{;} &\to \text{SEMI} \\
    \text{^^=} &\to \text{EXPONENT_ASSIGN} \\
    \text{^^} &\to \text{EXPONENT} \\
    \text{^=} &\to \text{XOR_ASSIGN} \\
    \text{^} &\to \text{XOR} \\
    \text{%=} &\to \text{MOD_ASSIGN} \\
    \text{%} &\to \text{MOD} \\
    \text{*=} &\to \text{MUL_ASSIGN} \\
    \text{*} &\to \text{MUL} \\
    \text{//=} &\to \text{INTEGER_DIV_ASSIGN} \\
    \text{//} &\to \text{INTEGER_DIV} \\
    \text{/=} &\to \text{DIV_ASSIGN} \\
    \text{/} &\to \text{DIV} \\
    \text{++} &\to \text{INC} \\
    \text{+=} &\to \text{ADD_ASSIGN} \\
    \text{+} &\to \text{ADD} \\
    \text{--} &\to \text{DEC} \\
    \text{-=} &\to \text{SUB_ASSIGN} \\
    \text{-} &\to \text{SUB} \\
    \text{<<} &\to \text{SHIFT_LEFT} \\
    \text{<=} &\to \text{LE_OP} \\
    \text{<} &\to \text{LT_OP} \\
    \text{>>} &\to \text{SHIFT_RIGHT} \\
    \text{>=} &\to \text{GE_OP} \\
    \text{>} &\to \text{GT_OP} \\
    \text{==} &\to \text{EQ_OP} \\
    \text{=} &\to \text{ASSIGN} \\
    \text{&&} &\to \text{LOGICAL_AND} \\
    \text{&=} &\to \text{AND_ASSIGN} \\
    \text{&} &\to \text{AND} \\
    \text{||} &\to \text{LOGICAL_OR} \\
    \text{|=} &\to \text{OR_ASSIGN} \\
    \text{|} &\to \text{OR} \\
    \text{.} &\to \text{DOT} \\
    \text{[a-zA-Z_][a-zA-Z0-9_]+} &\to \text{IDENTIFIER} \\
    \text{[-]?[0-9]+} &\to \text{SIGNED_INT_LITERAL} \\
    \text{[0-9]+} &\to \text{UNSIGNED_INT_LITERAL} \\
    \text{[-]?([0-9]+([.][0-9]*)?|[.][0-9]+)} &\to \text{FLOAT_LITERAL} \\
    \text{".*"} &\to \text{STRING_LITERAL} \\
\end{align}
$$

## Parser Grammar
**Subject to change**
$$
\begin{align}
    [\text{Prog}] &\to [\text{Stmt}]^* \\
    [\text{Stmt}] &\to
    \begin{cases}
        \text{exit}([\text{Expr}]); \\
        \text{let}\space\text{ident} = [\text{Expr}]; \\
        \text{ident} = \text{[Expr]}; \\
        \text{if} [\text{Expr}][\text{Scope}]\text{[IfPred]}\\
        \text{while}[\text{Expr}][\text{Scope}]\\
        [\text{Scope}]
    \end{cases} \\
    \text{[Scope]} &\to \{[\text{Stmt}]^*\} \\
    \text{[IfPred]} &\to 
    \begin{cases}
        \text{elif}\text{[Expr]}\text{[Scope]}\text{[IfPred]} \\
        \text{else}\text{[Scope]} \\
        \epsilon
    \end{cases} \\
    [\text{Expr}] &\to
    \begin{cases}
        [\text{Term}] \\
        [\text{BinExpr}]
    \end{cases} \\
    [\text{BinExpr}] &\to
    \begin{cases}
        [\text{Expr}] &\text{^^} &[\text{Expr}] &\text{prec} = 3; &\text{associativity} = \text{right} \\
        [\text{Expr}] &\text{\%} &[\text{Expr}] &\text{prec} = 2; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{*}  &[\text{Expr}] &\text{prec} = 2; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{/}  &[\text{Expr}] &\text{prec} = 2; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{+}  &[\text{Expr}] &\text{prec} = 1; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{-}  &[\text{Expr}] &\text{prec} = 1; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{<}  &[\text{Expr}] &\text{prec} = 0; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{>}  &[\text{Expr}] &\text{prec} = 0; &\text{associativity} = \text{left}  \\
    \end{cases} \\ 
    [\text{Term}] &\to
    \begin{cases}
        \text{intlit} \\
        \text{ident} \\
        ([\text{Expr}])
    \end{cases}
\end{align}
$$
