# Olive Lang
## Prerequisites
- cmake
- spdlog

## CMake Configuration
### Debug
From the root of the project run
```bash
cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_INSTALL_PREFIX:STRING=./install -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc-12 -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++-12 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --no-warn-unused-cli -S. -Bbuild -G Ninja
```

### Release
From the root of the project run
```bash
cmake -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_INSTALL_PREFIX:STRING=./install -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc-12 -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++-12 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --no-warn-unused-cli -S. -Bbuild -G Ninja
```

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
CD into the build directory and run ctest:
```bash
ctest -V
```

## Running
### Not Installed
From the root of the project, run the following command:
```bash
./build/oliveoil
```

### Installed
From the root of the project, run the following command:
```bash
./install/bin/oliveoil
```

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
        \text{if} ([\text{Expr}])[\text{Scope}]\text{[IfPred]}\\
        [\text{Scope}]
    \end{cases} \\
    \text{[Scope]} &\to \{[\text{Stmt}]^*\} \\
    \text{[IfPred]} &\to 
    \begin{cases}
        \text{elif}(\text{[Expr]})\text{[Scope]}\text{[IfPred]} \\
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
        [\text{Expr}]\enspace \^{} \enspace \^{} &[\text{Expr}] \quad \text{prec} = 2; \enspace \text{associativity} = \text{right} \\
        [\text{Expr}]\enspace               \%   &[\text{Expr}] \quad \text{prec} = 1; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               *    &[\text{Expr}] \quad \text{prec} = 1; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               /    &[\text{Expr}] \quad \text{prec} = 1; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               +    &[\text{Expr}] \quad \text{prec} = 0; \enspace \text{associativity} = \text{left}  \\
        [\text{Expr}]\enspace               -    &[\text{Expr}] \quad \text{prec} = 0; \enspace \text{associativity} = \text{left}  \\
    \end{cases} \\ 
    [\text{Term}] &\to
    \begin{cases}
        \text{int\_lit} \\
        \text{ident} \\
        ([\text{Expr}])
    \end{cases}
\end{align}
$$