FROM ubuntu:latest

# Install dependencies
RUN apt update && apt upgrade -y && apt install -y \
    g++ \
    cmake \
    libboost-all-dev \
    libssl-dev \
    curl \
    libcurl4-openssl-dev \
    zip \
    unzip \
    tar \
    git \
    libgtest-dev \
    libjsoncpp-dev \
    pkg-config \
    --fix-missing && \
    apt clean 

# Install CMake
RUN curl -fsSL https://cmake.org/files/v3.30/cmake-3.30.1-linux-x86_64.sh -o cmake.sh && \
    chmod +x cmake.sh && \
    ./cmake.sh --prefix=/usr/local --skip-license && \
    rm cmake.sh

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg && \
    /vcpkg/bootstrap-vcpkg.sh

# Set working directory
WORKDIR /app

# Copy bot source code and CMakeLists.txt
COPY . .

# Create a build directory and configure CMake
RUN mkdir -p build \
    && cd build \
    && cmake --log-level=DEBUG .. | tee /debug_cmake.log \
    && make

# Run the bot
CMD ["./build/DayIncreaseBotTgCpp"]
