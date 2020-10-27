FROM debian:buster-slim AS base

FROM base AS setup

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  curl \
  g++ \
  git \
  ninja-build \
  tar \
  unzip \
  wget \
  zip

FROM setup AS vcpkg

COPY . /usr/src/vcpkg-libsodium-avx512

WORKDIR /usr/src/vcpkg-libsodium-avx512/vcpkg
RUN ./bootstrap-vcpkg.sh
RUN ./vcpkg install libsodium

FROM vcpkg AS generate

WORKDIR /usr/src/vcpkg-libsodium-avx512/build
RUN cmake -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  ..

FROM generate AS build

RUN cmake --build . -j

FROM build AS test

RUN ./vcpkg-libsodium-avx512

FROM base AS final

COPY --from=test /usr/src/vcpkg-libsodium-avx512/build/vcpkg-libsodium-avx512 /opt/bin/vcpkg-libsodium-avx512
WORKDIR /opt/bin

ENTRYPOINT ["/opt/bin/vcpkg-libsodium-avx512"]
CMD [ "1024000", "100" ]
