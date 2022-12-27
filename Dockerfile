#### Base Image
FROM ubuntu AS base

# add setup_cpp
WORKDIR "/"
RUN apt-get update -qq
RUN apt-get install -y --no-install-recommends wget ca-certificates
RUN wget --no-verbose "https://github.com/aminya/setup-cpp/releases/download/v0.23.0/setup_cpp_linux"
RUN chmod +x ./setup_cpp_linux

# install llvm, cmake, ninja, and ccache
RUN ./setup_cpp_linux --compiler llvm --cmake true --ninja true --ccache true --conan true

CMD source ~/.cpprc
ENTRYPOINT [ "/bin/bash" ]

#### Building
FROM base AS builder
ADD . /home/app
WORKDIR /home/app
RUN bash -c 'source ~/.cpprc \
    && rm -rf /home/app/build && cmake -S/home/app -B/home/app/build && cmake --build /home/app/build --config Release --target ledpi -j 14'

### Running environment
# use a distroless image or ubuntu:22.04 if you wish
FROM ubuntu
# copy the built binaries and their runtime dependencies
COPY --from=builder /home/app/build/bin /home/app/
WORKDIR /home/app
ENTRYPOINT ["bash", "ledpi"]