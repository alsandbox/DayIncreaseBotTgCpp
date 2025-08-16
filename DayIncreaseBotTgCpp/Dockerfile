FROM ubuntu:latest

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

RUN curl -fsSL https://cmake.org/files/v3.30/cmake-3.30.1-linux-x86_64.sh -o cmake.sh && \
    chmod +x cmake.sh && \
    ./cmake.sh --prefix=/usr/local --skip-license && \
    rm cmake.sh

RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg && \
    /vcpkg/bootstrap-vcpkg.sh

WORKDIR /app

COPY . .

RUN mkdir -p build \
    && cd build \
    && cmake --log-level=DEBUG .. | tee /debug_cmake.log \
    && make

CMD ["./build/DayIncreaseBotTgCpp"]
