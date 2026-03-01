FROM debian:latest AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    pkg-config \
    libcurl4-openssl-dev \
    doxygen \
    curl \
    git \
    python3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN make build

FROM debian:latest

RUN apt-get update && apt-get install -y --no-install-recommends \
    libcurl4 \
    curl \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/custom_bin/cdd-cpp /app/cdd-cpp

ENTRYPOINT ["/app/cdd-cpp", "serve_json_rpc", "--port", "8082", "--listen", "0.0.0.0"]
