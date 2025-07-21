# Olive Lang
## Prerequisites
- cmake

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