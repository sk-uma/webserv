FROM debian:buster

RUN apt-get update
RUN apt-get install -y vim \
                       curl \
                       python3 \
                       python3-pip \
                       nginx-full \
                       fcgiwrap

RUN mkdir -p /usr/lib/cgi-bin

COPY default /etc/nginx/sites-available/default
COPY fcgiwrap.conf /etc/nginx/fcgiwrap.conf
COPY python_cgi.py          /usr/lib/cgi-bin/python_cgi.py
COPY without_permission.py  /usr/lib/cgi-bin/without_permission.py
COPY without_shebang.py     /usr/lib/cgi-bin/without_shebang.py
COPY perl_cgi.pl            /usr/lib/cgi-bin/perl_cgi.pl
COPY setup.sh /root/setup.sh

RUN chmod 755 /usr/lib/cgi-bin/python_cgi.py
RUN chmod 755 /usr/lib/cgi-bin/perl_cgi.pl
RUN chmod 755 /usr/lib/cgi-bin/without_shebang.py

CMD /bin/sh /root/setup.sh