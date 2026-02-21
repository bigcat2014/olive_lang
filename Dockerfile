# Base image with common dependencies
FROM ubuntu:24.04

# Install common build tools and dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    doxygen \
    git \
    gnupg \
    ninja-build \
    python3-pip \
    software-properties-common \
    texlive-font-utils \
    valgrind \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Install multiple GCC versions (13 through 15)
RUN add-apt-repository ppa:ubuntu-toolchain-r/ppa -y \
    && apt-get update && apt-get install -y \
    gcc-13 g++-13 \
    gcc-14 g++-14 \
    # gcc-15 g++-15 \
    && rm -rf /var/lib/apt/lists/*

# Install multiple Clang versions from LLVM repository (19 through 21)
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-19 main" \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
    clang-19 clang++-19 clang-tidy-19 clang-format-19 llvm-19 \
    && rm -rf /var/lib/apt/lists/*

RUN add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-20 main" \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
    clang-20 clang++-20 clang-tidy-20 clang-format-20 llvm-20 \
    && rm -rf /var/lib/apt/lists/*

RUN add-apt-repository "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-21 main" \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        clang-21 clang++-21 clang-tidy-21 clang-format-21 llvm-21 \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies
RUN pip3 install --no-cache-dir --break-system-packages compdb run-clang-format cmakelang gcovr

# Set defaults
# RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 100 \
#     && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 100
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 200 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 200 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 100 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-20 100 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-21 200 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-20 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-21 200 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-19 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-20 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-21 200 \
    && update-alternatives --install /usr/bin/run-clang-tidy run-clang-tidy /usr/bin/run-clang-tidy-19 100 \
    && update-alternatives --install /usr/bin/run-clang-tidy run-clang-tidy /usr/bin/run-clang-tidy-20 100 \
    && update-alternatives --install /usr/bin/run-clang-tidy run-clang-tidy /usr/bin/run-clang-tidy-21 200 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-19 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-20 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-21 200 \
    && update-alternatives --install /usr/bin/llvm-cov llvm-cov /usr/bin/llvm-cov-19 100 \
    && update-alternatives --install /usr/bin/llvm-cov llvm-cov /usr/bin/llvm-cov-20 100 \
    && update-alternatives --install /usr/bin/llvm-cov llvm-cov /usr/bin/llvm-cov-21 200 \
    && update-alternatives --set gcc /usr/bin/gcc-14 \
    && update-alternatives --set g++ /usr/bin/g++-14 \
    && update-alternatives --set clang /usr/bin/clang-21 \
    && update-alternatives --set clang++ /usr/bin/clang++-21 \
    && update-alternatives --set clang-tidy /usr/bin/clang-tidy-21 \
    && update-alternatives --set run-clang-tidy /usr/bin/run-clang-tidy-21 \
    && update-alternatives --set clang-format /usr/bin/clang-format-21 \
    && update-alternatives --set llvm-cov /usr/bin/llvm-cov-21


WORKDIR /workspace