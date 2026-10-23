# Builds emit-key once per architecture rather than once per device. It needs
# nothing beyond libc and the kernel's uinput header, so linking it statically
# against musl gives a binary that runs on any device of that architecture,
# including the ones whose MinUI toolchain lacks UI_DEV_SETUP.
ARG BASE_IMAGE=alpine:3.20

FROM --platform=linux/arm/v7 ${BASE_IMAGE} AS build-arm
RUN apk add --no-cache gcc musl-dev linux-headers
COPY emit-key.c /src/emit-key.c
RUN gcc -static -s -O2 -Wall -o /emit-key /src/emit-key.c
# a binary that asks for a shared library is not portable across devices
RUN ! readelf -d /emit-key | grep -q NEEDED

FROM --platform=linux/arm64 ${BASE_IMAGE} AS build-arm64
RUN apk add --no-cache gcc musl-dev linux-headers
COPY emit-key.c /src/emit-key.c
RUN gcc -static -s -O2 -Wall -o /emit-key /src/emit-key.c
RUN ! readelf -d /emit-key | grep -q NEEDED

FROM scratch AS export
COPY --from=build-arm /emit-key /emit-key-linux-arm
COPY --from=build-arm64 /emit-key /emit-key-linux-arm64
