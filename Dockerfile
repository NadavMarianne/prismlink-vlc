# PrismLink C development Dockerfile
FROM ubuntu:24.04

# Avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install minimal C build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# Set project root inside the container
ENV PRISMLINK_ROOT=/workspaces/prismlink-vlc
WORKDIR $PRISMLINK_ROOT

# Default shell
CMD ["/bin/bash"]