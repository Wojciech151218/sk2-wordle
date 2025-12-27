FROM ubuntu:22.04

WORKDIR /app

# Install all dependencies (build + runtime)
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    libssl-dev \
    libssl3 \
    ca-certificates \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Expose ports
EXPOSE 8080 4040

# Build and run the server at container startup
CMD mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE:-Release} .. && \
    cmake --build . --target wordle-server -j$(nproc) && \
    cd .. && \
    chmod +x ./build/wordle-server && \
    ./build/wordle-server
