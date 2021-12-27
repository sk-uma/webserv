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
                       python3-pip

WORKDIR /root

# nginx setup with upload_module

RUN git clone -b release-1.12.0 https://github.com/nginx/nginx.git
RUN git clone https://github.com/vkholodkov/nginx-upload-module.git
RUN git clone https://github.com/Fleshgrinder/nginx-sysvinit-script.git

WORKDIR /root/nginx

RUN auto/configure \
    --with-cc-opt="-O2 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Werror=format-security -D_FORTIFY_SOURCE=2" \
    --with-ld-opt="-Wl,-Bsymbolic-functions -Wl,-z,relro" \
    --prefix=/usr/local \
    --conf-path=/etc/nginx/nginx.conf \
    --http-log-path=/var/log/nginx/access.log \
    --error-log-path=/var/log/nginx/error.log \
    --lock-path=/var/lock/nginx.lock \
    --pid-path=/run/nginx.pid \
    --http-client-body-temp-path=/var/lib/nginx/body \
    --http-fastcgi-temp-path=/var/lib/nginx/fastcgi \
    --http-proxy-temp-path=/var/lib/nginx/proxy \
    --http-scgi-temp-path=/var/lib/nginx/scgi \
    --http-uwsgi-temp-path=/var/lib/nginx/uwsgi \
    --add-module=../nginx-upload-module \
    --build=$USER

RUN sed -i -e 's/ -Werror / /g' objs/Makefile

RUN make -j4
RUN make install

WORKDIR /root/nginx-sysvinit-script
RUN make

# nginx setup end

RUN mkdir -p /var/lib/nginx
RUN mkdir -p /var/www/html

WORKDIR /usr/local/html

RUN mkdir -p /etc/nginx/conf.d/
RUN cp /usr/local/html/index.html /var/www/html/index.html

COPY nginx.conf /etc/nginx/nginx.conf
COPY default.conf /etc/nginx/conf.d/default.conf
COPY setup.sh /root/setup.sh
COPY upload.html /var/www/html/upload.html

COPY cgifile /usr/lib/cgi-bin
RUN chmod -R 755 /usr/lib/cgi-bin
RUN sed -i -e 's/#user  nobody/user www-data/g' /etc/nginx/nginx.conf

CMD /bin/bash /root/setup.sh
