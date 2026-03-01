FROM alpine:latest AS builder

RUN apk update && apk add --no-cache \
    build-base \
    cmake \
    pkgconf \
    curl-dev \
    doxygen \
    bash \
    git \
    python3

WORKDIR /app
COPY . .

RUN make build

FROM alpine:latest

RUN apk update && apk add --no-cache libstdc++ libgcc libcurl curl

WORKDIR /app
COPY --from=builder /app/custom_bin/cdd-cpp /app/cdd-cpp

ENTRYPOINT ["/app/cdd-cpp", "serve_json_rpc", "--port", "8082", "--listen", "0.0.0.0"]
