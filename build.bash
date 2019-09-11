#!/bin/bash
# 
# File:   build.bash
# Author: harry
#
# Created on Dec 11, 2018, 6:56:33 PM
#

#udo apt-get install -y libtool autoconf automake gcc g++ automake pkg-config yasm nasm

mkdir -p workspace && cd workspace

export LC_CTYPE="en_US.UTF-8"
export INSTALL_PATH=${PWD}
export PATH=$INSTALL_PATH/bin:$PATH
export JOBS=$(nproc)
export PKG_CONFIG_PATH=$INSTALL_PATH/lib/pkgconfig

NASM="2.14"
[ ! -f "nasm-${NASM}.tar.bz2" ] && curl -Lk -o "nasm-${NASM}.tar.bz2" https://www.nasm.us/pub/nasm/releasebuilds/"${NASM}"/"nasm-${NASM}.tar.bz2"
[ ! -d "nasm-${NASM}" ] && mkdir -p "nasm-${NASM}" && tar -xf "nasm-${NASM}.tar.bz2" -C "nasm-${NASM}" --strip-components=1
if [ ! -f "${INSTALL_PATH}/bin/nasm" ]; then
    mkdir -p "nasm-${NASM}/build" && cd "nasm-${NASM}/build"
    ../configure --prefix="${INSTALL_PATH}"
    make -j "${JOBS}" && make install
    cd "${INSTALL_PATH}"
fi

ZLIB="zlib-1.2.11"
[ ! -f "${ZLIB}.tar.gz" ] && curl -Lk -o "${ZLIB}.tar.gz" https://zlib.net/"${ZLIB}.tar.gz"
[ ! -d "${ZLIB}" ] && mkdir -p "${ZLIB}" && tar -xf "${ZLIB}.tar.gz" -C "${ZLIB}" --strip-components=1
if [ ! -f "${INSTALL_PATH}/lib/libz.a" ]; then
    mkdir -p "${ZLIB}/build" && cd "${ZLIB}/build"
    ../configure --prefix="${INSTALL_PATH}" --static
    make -j "${JOBS}" && make install
    cd "${INSTALL_PATH}"
fi

FDKAAC="v2.0.0"
[ ! -f "fdk-aac-${FDKAAC}.tar.gz" ] && curl -Lk -o "fdk-aac-${FDKAAC}.tar.gz" https://github.com/mstorsjo/fdk-aac/archive/"${FDKAAC}.tar.gz"
[ ! -d "fdk-aac-${FDKAAC}" ] && mkdir -p "fdk-aac-${FDKAAC}" && tar -xf "fdk-aac-${FDKAAC}.tar.gz" -C "fdk-aac-${FDKAAC}" --strip-components=1
if [ ! -f "${INSTALL_PATH}/lib/libfdk-aac.a" ]; then
    cd "fdk-aac-${FDKAAC}" && [ ! -f "configure" ] && ./autogen.sh
    mkdir -p "build" && cd "build"
    ../configure --prefix="${INSTALL_PATH}" --disable-shared --enable-static
    make -j "${JOBS}" && make install
    cd "${INSTALL_PATH}"
fi

X264="x264-snapshot-20181210-2245-stable"
[ ! -f "${X264}.tar.bz2" ] && curl -Lk -o "${X264}.tar.bz2" ftp://ftp.videolan.org/pub/x264/snapshots/"${X264}.tar.bz2"
[ ! -d "${X264}" ] && mkdir -p "${X264}" && tar -xf "${X264}.tar.bz2" -C "${X264}" --strip-components=1
if [ ! -f "${INSTALL_PATH}/lib/libx264.a" ]; then
    mkdir -p "${X264}/build" && cd "${X264}/build"
    ../configure --prefix="${INSTALL_PATH}" --enable-static --disable-opencl --enable-pic
    make -j "${JOBS}" && make install
    cd "${INSTALL_PATH}"
fi

X265="x265_2.9"
[ ! -f "${X265}.tar.gz" ] && curl -Lk -o "${X265}.tar.gz" https://bitbucket.org/multicoreware/x265/downloads/"${X265}.tar.gz"
[ ! -d "${X265}" ] && mkdir -p "${X265}" && tar -xf "${X265}.tar.gz" -C "${X265}" --strip-components=1
if [ ! -f "${INSTALL_PATH}/lib/libx265.a" ]; then
    mkdir -p "${X265}/build" && cd "${X265}/build"
    cmake -D ENABLE_SHARED:BOOL=OFF -DCMAKE_INSTALL_PREFIX:PATH="${INSTALL_PATH}" -DENABLE_LIBNUMA:BOOL=OFF -DHIGH_BIT_DEPTH:BOOL=ON -H"${INSTALL_PATH}/${X265}"/source -B. -G "Unix Makefiles"
    make -j "${JOBS}" && make install
    cd "${INSTALL_PATH}"
fi

OPENSSL="OpenSSL_1_1_1a"
[ ! -f "${OPENSSL}.tar.gz" ] && curl -Lk -o "${OPENSSL}.tar.gz" https://github.com/openssl/openssl/archive/"${OPENSSL}.tar.gz"
[ ! -d "${OPENSSL}" ] && mkdir -p "${OPENSSL}" && tar -xf "${OPENSSL}.tar.gz" -C "${OPENSSL}" --strip-components=1
if [ ! -f "${INSTALL_PATH}/lib/libssl.a" ]; then
    mkdir -p "${OPENSSL}/build" && cd "${OPENSSL}/build"
    ../config -no-shared --prefix="${INSTALL_PATH}"
    make -j "${JOBS}" && make install
    cd "${INSTALL_PATH}"
fi

[ ! -f "${INSTALL_PATH}/include/picojson.h" ] && curl -Lk -o "${INSTALL_PATH}/include/picojson.h" https://raw.githubusercontent.com/kazuho/picojson/master/picojson.h

FFMPEG="ffmpeg-4.1"
[ ! -f "${FFMPEG}.tar.bz2" ] && curl -Lk -o "${FFMPEG}.tar.bz2" https://ffmpeg.org/releases/"${FFMPEG}.tar.bz2"
[ ! -d "${FFMPEG}" ] && mkdir -p "${FFMPEG}" && tar -xf "${FFMPEG}.tar.bz2" -C "${FFMPEG}" --strip-components=1
[ ! -d "${FFMPEG}-origin" ] && mkdir -p "${FFMPEG}-origin" && tar -xf "${FFMPEG}.tar.bz2" -C "${FFMPEG}-origin" --strip-components=1
[ -z "$(grep api.h ${FFMPEG}/fftools/ffmpeg.h)" ] && cd ${FFMPEG} && patch -p1 -f < "${INSTALL_PATH}/../${FFMPEG}.patch" && cd ..
if [ ! -f "${INSTALL_PATH}/lib/libavformat.a" ]; then
    mkdir -p "${FFMPEG}_build" && cd "${FFMPEG}_build"
    ../${FFMPEG}/configure --enable-static --disable-shared --prefix="${INSTALL_PATH}" --disable-doc \
        --enable-gpl --enable-nonfree --disable-iconv --disable-libxcb_shape --disable-libxcb_xfixes \
        --disable-xlib --disable-libxcb --disable-libxcb_shm --disable-lzma \
        --enable-postproc --enable-runtime-cpudetect --disable-ffplay --disable-ffprobe \
        --enable-libfdk-aac --enable-libx264 --enable-libx265 --enable-openssl \
        --extra-cflags="-I${INSTALL_PATH}/include" \
        --extra-ldflags="-L${INSTALL_PATH}/lib" --extra-libs="-lstdc++ -lpthread -ldl -lm"
    make -j "${JOBS}" && make install
    cd "${INSTALL_PATH}"
fi

#[ ! -f "${INSTALL_PATH}/../${FFMPEG}.patch" ] &&  cd ${FFMPEG} && diff -ruN --exclude=build ../${FFMPEG}-origin . > "${INSTALL_PATH}/../${FFMPEG}.patch"
