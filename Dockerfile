# Use the official Ubuntu 20.04 LTS as the base image
FROM ubuntu:20.04

# Set environment variables to avoid user interaction during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Update the package list and install the required packages
RUN apt-get update && \
    apt-get install -y vim git gcc g++ ctags && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN useradd -ms /bin/bash ohsj3781
USER ohsj3781
WORKDIR /home/ohsj3781


# Set the default command to run when starting the container
CMD ["bash"]