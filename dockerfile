# Build stage
FROM ubuntu:22.04 AS builder

WORKDIR /build

# Install build dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy CMakeLists.txt and external dependencies first for better caching
COPY CMakeLists.txt ./
COPY external/ ./external/

# Copy source files
COPY server/ ./server/
COPY logic/ ./logic/
COPY main.cpp ./

# Build the project
RUN mkdir -p build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --target wordle-server -j$(nproc)

# Runtime stage
FROM ubuntu:22.04

WORKDIR /app

# Install only runtime dependencies
RUN apt-get update && apt-get install -y \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Copy the built binary from builder stage
COPY --from=builder /build/build/wordle-server /app/wordle-server

# Make binary executable
RUN chmod +x /app/wordle-server

# Copy configuration file (will be overridden by volume mount in docker-compose)
COPY conf.json /app/conf.json

# Expose ports
EXPOSE 8080 4040

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=40s --retries=3 \
    CMD timeout 1 bash -c "echo > /dev/tcp/localhost/8080" || exit 1

# Run the server
CMD ["./wordle-server"]
