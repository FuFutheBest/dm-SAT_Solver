FROM gcc:12-bookworm
RUN apt-get update && apt-get install -y --no-install-recommends python3 zlib1g-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /work
CMD ["bash"]
