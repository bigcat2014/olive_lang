# Olive Lang
## Prerequisites
- cmake
### Unit test dependencies
- clang-format
- clang-tidy
- cmake-format
- gcovr
- libgtest-dev
- valgrind (See [known issues](#known-issues))

## CMake Configuration
### Debug
From the root of the project run the following command, replacing the compiler filepaths with your desired compiler.
```bash
cmake --preset debug -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++
```

### Release
From the root of the project run the following command, replacing the compiler filepaths with your desired compiler.
```bash
cmake --preset release -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++
```

### Configuring to build unit tests
Add the following option to one of the above configuration commands.
```bash
-DBUILD_TESTING:BOOL=TRUE
```

## Targets
- all - Build targets
- install - Install targets
- build_tests - Build all unit tests
- run_tests - Run all unit tests
- run_linters - Run the linter subset of unit tests
- coverage - Generate code coverage report
- doxygen_docs - Generate Doxygen documentation

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

## Running All Unit Tests
From the root of the project, run the following commands:
```bash
cmake --build build --config Debug --target build_tests --
cmake --build build --config Debug --target run_tests --
```

## Running Linter Unit Tests
From the root of the project, run the following commands:
```bash
cmake --build build --config Debug --target build_tests --
cmake --build build --config Debug --target run_linters --
```

### Generating Test Coverage Report
From the root of the project, build the coverage target  
**Note:** Run unit tests before generating coverage report
```bash
cmake --build build --config Debug --target coverage --
```

### Generating Doxygen documentation
From the root of the project, build the doxygen_docs target
```bash
cmake --build build --config Debug --target doxygen_docs --
```

## Running
### Not Installed
From the root of the project, run the following command:
```bash
./build/{release|debug}pimento --help
```

### Installed
From the root of the project, run the following command:
```bash
./install/{release|debug}bin/pimento --help
```

## Known Issues
- Precedence climbing for order of operations not currently working quite right.
- If building with Clang >= 14, you need valgrind >= 3.20 in order to run the memcheck unit tests.
    - https://bugs.kde.org/show_bug.cgi?id=452758

# Grammar
## Lexer Grammar
Can be found [here](./docs/lexical_grammar.pdf)


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
        [\text{Expr}] &\hat{}\enspace\hat{} &[\text{Expr}] &\text{prec} = 3; &\text{associativity} = \text{right} \\
        [\text{Expr}] &\text{\%}            &[\text{Expr}] &\text{prec} = 2; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{*}             &[\text{Expr}] &\text{prec} = 2; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{/}             &[\text{Expr}] &\text{prec} = 2; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{+}             &[\text{Expr}] &\text{prec} = 1; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{-}             &[\text{Expr}] &\text{prec} = 1; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{<}             &[\text{Expr}] &\text{prec} = 0; &\text{associativity} = \text{left}  \\
        [\text{Expr}] &\text{>}             &[\text{Expr}] &\text{prec} = 0; &\text{associativity} = \text{left}  \\
    \end{cases} \\ 
    [\text{Term}] &\to
    \begin{cases}
        \text{intlit} \\
        \text{ident} \\
        ([\text{Expr}])
    \end{cases}
\end{align}
$$
