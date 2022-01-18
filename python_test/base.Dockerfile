FROM debian:buster

RUN apt-get update
RUN apt-get install -y curl \
                       vim \
                       git \
                       gcc \
                       libpcre3 \
                       libpcre3-dev \
                       zlib1g \
                       zlib1g-dev \
                       openssl \
                       libssl-dev \
                       make \
                       build-essential \
                       fcgiwrap \
                       python \
                       python3 \
                       python3-pip \
                       procps

CMD tail -f /dev/null
