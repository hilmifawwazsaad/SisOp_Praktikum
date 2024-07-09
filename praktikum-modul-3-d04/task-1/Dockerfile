# Base Image
FROM ubuntu:latest

# Instal Dependencies
RUN apt-get update
RUN apt-get install -y \
    gcc \
    unzip \
    python3 \
    python3-pip \
    python3-venv
RUN apt-get clean

# Create and activate virtual environment
RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:${PATH}"

# Install Kaggle CLI
RUN pip install kaggle

# Generate Kaggle API Key
RUN mkdir -p /root/.kaggle && \
    echo '{"username":"hfwzsd","key":"1be2d13700ab981c07f30afcca6d057d"}' > /root/.kaggle/kaggle.json && \
    chmod 600 /root/.kaggle/kaggle.json

# Set working directory
WORKDIR /APP

# Copy source code to working directory
COPY ayamgoreng.c .

# Set execute permission
RUN chmod +x ayamgoreng.c

# Compile source code
RUN gcc -o ayamgoreng ayamgoreng.c