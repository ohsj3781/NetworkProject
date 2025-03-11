FROM ubuntu:20.04

# Set non-interactive mode to prevent prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt update && apt install -y vim git gcc g++ ctags wget sudo python-is-python3 tcpdump bzip2

ARG USER_ID USER_PASSWORD

# Create user
RUN useradd -ms /bin/bash $USER_ID && echo "$USER_ID:$USER_PASSWORD" | chpasswd

# Create workspace directory
RUN mkdir -p /home/$USER_ID/workspace

# Switch to user and build NS-3
USER ${USER_ID}

# Default to bash
CMD ["bash"]
