# Use the official Ubuntu 20.04 LTS as the base image
FROM ubuntu:20.04

# Set environment variables to avoid user interaction during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update the package list and install the required packages
RUN apt update && \
    apt install -y vim git gcc g++ ctags wget sudo python-is-python3 tcpdump&& \
    wget https://www.nsnam.org/release/ns-allinone-3.29.tar.bz2 && \
    tar -xvf ns-allinone-3.29.tar.bz2 && \
    cd ns-allinone-3.29 && \
    ./build.py --enable-examples --enable-tests && \
    cd ns-3.29 && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -ms /bin/bash ohsj3781 && echo "ohsj3781:3781" | chpasswd
USER ohsj3781
WORKDIR /home/ohsj3781


# Set the default command to run when starting the container
CMD ["bash"]