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
    && apt clean

# Create a normal user (optional but recommended)
RUN useradd -ms /bin/bash devuser && \
    echo "devuser ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER devuser
WORKDIR /home/devuser

# This will be your workspace where your code is mounted
CMD ["/bin/bash"]

