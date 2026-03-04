FROM debian:12-slim AS builder

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    python3 \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCDD_EXTREME_CHECKS=OFF \
    && cmake --build build -j$(nproc)

FROM debian:12-slim

WORKDIR /app
COPY --from=builder /app/build/cdd-cpp /usr/local/bin/cdd-cpp
COPY --from=builder /app/build/cdd-tests /usr/local/bin/cdd-tests

EXPOSE 8082

ENTRYPOINT ["cdd-cpp", "serve_json_rpc"]
