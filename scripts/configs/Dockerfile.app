FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install development tools
RUN apt update && apt install -y \
    build-essential \
    gcc g++ make cmake \
    git \
    python3 python3-pip \
    pkg-config \
    vim nano \
    curl wget \
    gdb \
    sudo \
    iputils-ping \
    libpq-dev \
    && apt clean


# This will be your workspace where your code is mounted
CMD ["/bin/bash"]

