# Base Image
FROM gcc:latest

# Set working directory
WORKDIR /APP

# Copy chat.c to working directory
COPY chat.c .

# Set execute permission
RUN chmod +x chat.c

# Compile chat.c
RUN gcc -o chat chat.c -pthread

# Create a Volume shared (POIN C DAN D)
VOLUME /shared

# Set entrypoint to run an interactive shell (POIN C DAN D)
ENTRYPOINT ["/bin/sh"]