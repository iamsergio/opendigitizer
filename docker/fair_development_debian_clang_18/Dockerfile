FROM base_debian_testing AS fair_development
LABEL Description="fair: build environment"

# RUN apt-get -y -o Acquire::Check-Valid-Until=false update
# RUN echo 'deb http://deb.debian.org/debian-debug/ experimental-debug main contrib non-free' > /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb https://snapshot.debian.org/archive/debian-debug/20230610T205516Z testing-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb http://deb.debian.org/debian-debug/ stable-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb http://deb.debian.org/debian-debug/ proposed-updates-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb http://deb.debian.org/debian-debug/ stretch-backports-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb http://deb.debian.org/debian-debug/ testing-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb http://deb.debian.org/debian-debug/ testing-proposed-updates-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb http://deb.debian.org/debian-debug/ unstable-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list
# RUN echo 'deb http://deb.debian.org/debian-debug/ experimental-debug main contrib non-free' >> /etc/apt/sources.list.d/debian-debug.list

USER root

# RUN echo 'deb http://deb.debian.org/debian/ sid main' >> /etc/apt/sources.list.d/debian-sid.list
# RUN apt-get update

# RUN tree /etc/apt
# RUN cat /etc/apt/sources.list.d/*

RUN apt-get -y install \
    gcc-13 \
    'g++-13' \
    libstdc++-13-dev

# RUN rm /etc/apt/sources.list.d/debian-sid.list
# RUN apt-get update

USER root

# RUN echo 'deb http://deb.debian.org/debian/ sid main' >> /etc/apt/sources.list.d/debian-sid.list
# RUN apt-get update

# RUN tree /etc/apt
# RUN cat /etc/apt/sources.list.d/*

RUN apt-get -y install \
    gcc-14 \
    'g++-14' \
    libstdc++-14-dev

# RUN rm /etc/apt/sources.list.d/debian-sid.list
# RUN apt-get update

USER root

# RUN echo 'deb http://deb.debian.org/debian/ sid main' >> /etc/apt/sources.list.d/debian-sid.list
# RUN apt-get update

# RUN tree /etc/apt
# RUN cat /etc/apt/sources.list.d/*

RUN apt-get -y install \
    clang-18 \
    clangd-18 \
    clang-format-18 \
    clang-tidy-18 \
    clang-tools-18 \
    libc\+\+-18-dev \
    libclang-18-dev \
    llvm-18-dev \
    sudo strace lld

RUN ln -s /usr/bin/clangd-18 /usr/bin/clangd
RUN ln -s /usr/bin/clang-18 /usr/bin/clang
RUN ln -s /usr/bin/clang++-18 /usr/bin/clang++
RUN ln -s /usr/bin/clang-format-18 /usr/bin/clang-format
RUN ln -s /usr/bin/clang-tidy-18 /usr/bin/clang-tidy
RUN ln -s /usr/bin/llvm-config-18 /usr/bin/llvm-config

# RUN rm /etc/apt/sources.list.d/debian-sid.list
# RUN apt-get update

RUN find-dbgsym-packages /usr/bin/clang++-18

RUN apt-get -y install \
    freeglut3-dev \
    libsdl2-dev

RUN apt-get -y install \
    libusb-1.0-0-dev

RUN apt-get -y install \
    nodejs

# Install timing system dependencies: etherbone
RUN git clone --branch v2.1.3 --depth=1 https://ohwr.org/project/etherbone-core.git && cd etherbone-core/api && touch ChangeLog && sed -e "s%AC_MSG_ERROR%AC_MSG_NOTICE%g" -i configure.ac && autoreconf -i && ./configure && make -j && make install

# Install timing system dependencies: saftlib
RUN apt-get -y install libsigc++-2.0-dev libxslt1-dev libboost-all-dev
RUN git clone --branch v3.0.3 --depth=1 https://github.com/GSI-CS-CO/saftlib.git && cd saftlib && ./autogen.sh && ./configure && make && make install

USER root
# RUN apt-get -y install \
#     nodejs \
#     npm

# RUN apt-get -y install \
#     chromium-driver \
#     firefox-esr \
#     python3-selenium

# RUN mkdir -p /docker_opt/development/
# RUN wget -qO- https://github.com/mozilla/geckodriver/releases/download/v0.32.2/geckodriver-v0.32.2-linux64.tar.gz -O - | tar xvz -C /docker_opt
# RUN chmod +x /docker_opt/geckodriver

RUN mkdir -p /docker_opt/development/emscripten
RUN chown user /docker_opt/development/emscripten

USER user

# RUN npm install selenium-webdriver || true

RUN cd /docker_opt/development/emscripten && ls -alh
RUN cd /docker_opt/development/emscripten && git clone https://github.com/emscripten-core/emsdk.git
RUN cd /docker_opt/development/emscripten/emsdk && ./emsdk install 3.1.66 # latest
RUN cd /docker_opt/development/emscripten/emsdk && ./emsdk activate 3.1.66 # latest
RUN mkdir -p /docker_opt/development/emscripten

USER root


# PicoScope
RUN mkdir /etc/apt/keyrings/ -p | true
RUN echo '1d393bc11356c602300e420d7acbfe64 /etc/apt/keyrings/picotech.gpg.key' > /etc/apt/keyrings/picotech.gpg.key.md5
RUN wget -O /etc/apt/keyrings/picotech.gpg.key https://labs.picotech.com/Release.gpg.key
RUN md5sum -c /etc/apt/keyrings/picotech.gpg.key.md5

RUN echo 'deb [signed-by=/etc/apt/keyrings/picotech.gpg.key] https://labs.picotech.com/rc/picoscope7/debian/ picoscope main' > /etc/apt/sources.list.d/picoscope.list
RUN apt-get update
RUN apt-get install -y udev libusb-1.0-0-dev
RUN apt-get install -y -oDebug::pkgAcquire::Worker=1  libps3000a libps4000a libps5000a libps6000 libps6000a || true


SHELL ["/bin/zsh", "-ec"]
USER user
