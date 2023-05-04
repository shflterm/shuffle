FROM ubuntu:22.04

RUN apt-get update && apt-get install -y build-essential cmake --no-install-recommends

WORKDIR /app

COPY . .

RUN cmake .

ENTRYPOINT cmake --build . && ./shuffle