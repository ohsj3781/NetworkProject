FROM ubuntu:20.04

# Set non-interactive mode to prevent prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt update && apt install -y vim git gcc g++ ctags wget sudo python-is-python3 tcpdump bzip2

ARG USER_PASSWORD

# Create user
RUN useradd -ms /bin/bash ohsj3781 && echo "ohsj3781:$USER_PASSWORD" | chpasswd

# Create workspace directory
RUN mkdir -p /home/ohsj3781/workspace

# Download and extract NS-3
RUN wget -O /tmp/ns-allinone-3.29.tar.bz2 https://www.nsnam.org/release/ns-allinone-3.29.tar.bz2 && \
    tar -xvf /tmp/ns-allinone-3.29.tar.bz2 -C /tmp

# Move NS-3 to workspace and change ownership
RUN mv /tmp/ns-allinone-3.29 /home/ohsj3781/workspace/ && \
    chown -R ohsj3781:ohsj3781 /home/ohsj3781/workspace/ns-allinone-3.29

# Switch to user and build NS-3
USER ohsj3781
WORKDIR /home/ohsj3781/workspace/ns-allinone-3.29
RUN ./build.py --enable-examples --enable-tests

# Default to bash
CMD ["bash"]
