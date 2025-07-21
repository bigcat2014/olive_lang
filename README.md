# Olive Lang
## Prerequisites
- cmake
- libboost-dev
- libspdlog-dev
### Building unit tests
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
    -G Ninja \
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
    -G Ninja \
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
- Coverage not currently working with Clang.

# Grammar
## Subject to change
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
        [\text{Expr}]\enspace \^{} \enspace \^{} &[\text{Expr}] \quad \text{prec} = 3; \enspace \text{associativity} = \text{right} \\
        [\text{Expr}]\enspace               \%   &[\text{Expr}] \quad \text{prec} = 2; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               *    &[\text{Expr}] \quad \text{prec} = 2; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               /    &[\text{Expr}] \quad \text{prec} = 2; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               +    &[\text{Expr}] \quad \text{prec} = 1; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               -    &[\text{Expr}] \quad \text{prec} = 1; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               <    &[\text{Expr}] \quad \text{prec} = 0; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               >    &[\text{Expr}] \quad \text{prec} = 0; \enspace \text{associativity} = \text{left}  \\
    \end{cases} \\ 
    [\text{Term}] &\to
    \begin{cases}
        \text{intlit} \\
        \text{ident} \\
        ([\text{Expr}])
    \end{cases}
\end{align}
$$