FROM debian:testing-slim AS debian-testing-base-system
LABEL Description="debian base development environment"

ARG DEBIAN_FRONTEND=noninteractive
ARG COLORTERM=truecolor
ARG DOCKER_NAME=DebianTesting
ENV TZ Europe/Berlin

RUN apt-get -y update
RUN apt-get -y full-upgrade
RUN apt-get -y install tree

# RUN mkdir -p /etc/apt/sources.list.d/
#
# RUN apt-add-repository 'deb http://deb.debian.org/debian/ experimental main contrib non-free'
# RUN echo 'deb http://deb.debian.org/debian/ experimental main contrib non-free' > /etc/apt/sources.list.d/debian-experimental.list
#
# RUN echo 'deb http://deb.debian.org/debian/ experimental main contrib non-free' > /etc/apt/sources.list.d/debian-experimental.list
# RUN cat /etc/apt/sources.list.d/debian-experimental.list
#
# RUN echo '\
# Package: *\n\
# Pin: release a=testing\n\
# Pin-Priority: 650\n\
# \n\
# Package: *\n\
# Pin: release a=unstable\n\
# Pin-Priority: 600\n\
# \n\
# Package: *\n\
# Pin: release a=experimental\n\
# Pin-Priority: 600\n\
# \n\
# Package: *\n\
# Pin: release a=sid\n\
# Pin-Priority: 600\n\
# ' > /etc/apt/preferences
#
# RUN tree /etc/apt
# RUN cat /etc/apt/sources.list
# RUN grep deb /etc/apt/sources.list.d/*
# RUN cat /etc/apt/sources.list.d/*

RUN apt-get -y update
RUN apt-get -y full-upgrade
RUN apt-cache policy

RUN apt-get update
RUN apt-get install -y tree
RUN tree /etc/apt

RUN apt-get -y full-upgrade

RUN cat /etc/apt/preferences || true

RUN apt-get -y install \
    apt-utils \
    aptitude \
    build-essential \
    cmake \
    curl \
    ccache \
    doxygen \
    gcovr \
    gdb \
    git \
    meson \
    ninja-build \
    tmux \
    vim-nox \
    wget \
    zsh

RUN apt-get -y install \
    libasound2-dev \
    libboost-dev \
    libcppunit-dev \
    libfftw3-dev \
    libfmt-dev \
    libgtest-dev \
    libsndfile1-dev \
    libsoapysdr-dev \
    libspdlog-dev \
    libssl-dev \
    libyaml-cpp-dev \
    libzmq3-dev

RUN apt-get -y install \
    libvolk2-dev \
    || apt-get -y install \
    libvolk-dev

RUN apt-get -y install \
    pybind11-dev \
    python3-dev \
    python3-gi-cairo \
    python3-mako \
    python3-numpy \
    python3-pip

RUN apt-get -y install \
    bat \
    eza \
    fd-find \
    fzf \
    hyperfine \
    kdesdk-scripts \
    lsd \
    most \
    nnn \
    ranger \
    vifm \
    ripgrep

RUN apt-get -y install \
    cpplint \
    cppcheck

RUN ln -s /usr/bin/fdfind /usr/bin/fd
RUN ln -s /usr/bin/batcat /usr/bin/bat

RUN apt-get -y install debian-goodies
RUN apt-get -y install libnotify-bin

RUN apt-get -y update
USER root
RUN mkdir /docker_opt/development/editors -p ; \
    cd /docker_opt/development/editors ; \
    wget https://github.com/neovim/neovim-releases/releases/download/v0.10.4/nvim-linux-x86_64.tar.gz -O nvim-0.10-amd64.tar.gz ; \
    tar xvf nvim-0.10-amd64.tar.gz ; \
    mv nvim-linux-x86_64 nvim-0.10-amd64 ; \
    rm -f nvim-0.10-amd64.tar.gz ; \
    ln -s /docker_opt/development/editors/nvim-0.10-amd64/bin/nvim /usr/bin

# Set the locale
# RUN echo 'en_GB.UTF-8 UTF-8' > /etc/locale.gen && locale-gen
RUN apt-get -y install locales locales-all
ENV LANG en_GB.UTF-8
ENV LANGUAGE en_GB:en
ENV LC_ALL en_GB.UTF-8
# RUN dpkg-reconfigure locales

RUN useradd -m -r -u 1000 -g users user -s /bin/zsh || true
RUN groupadd -g 1000 user --users user

RUN ln -s /usr/bin/python3 /usr/bin/python
