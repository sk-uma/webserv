FROM webserv_test_base:latest

RUN apt-get update
RUN apt-get install -y clang

COPY ./ /
COPY python_test/www-data /www-data
WORKDIR /
RUN make re

CMD ./webserv python_test/test.conf
