FROM alpine:3.19 AS builder

RUN apk add --no-cache \
    build-base \
    cmake \
    git \
    curl \
    python3 \
    pkgconfig \
    linux-headers

WORKDIR /app
COPY . .

RUN cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCDD_EXTREME_CHECKS=OFF \
    && cmake --build build -j$(nproc)

FROM alpine:3.19

RUN apk add --no-cache libstdc++ libgcc

WORKDIR /app
COPY --from=builder /app/build/cdd-cpp /usr/local/bin/cdd-cpp
COPY --from=builder /app/build/cdd-tests /usr/local/bin/cdd-tests

EXPOSE 8082

ENTRYPOINT ["cdd-cpp", "serve_json_rpc"]
