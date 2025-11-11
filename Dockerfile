# Use Ubuntu as base image
FROM ubuntu:22.04

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source files
COPY CMakeLists.txt ./
COPY include/ ./include/
COPY src/ ./src/
COPY data/ ./data/

# Create build directory and build the project
RUN mkdir build && cd build && \
    cmake .. && \
    cmake --build . && \
    cp stock_server /app/stock_server

# Set the working directory to /app (where stock_server expects to find data/)
WORKDIR /app

# Expose port 3000
EXPOSE 3000

# Set default environment variable
ENV PORT=3000

# Run the application
CMD ["./stock_server"]
