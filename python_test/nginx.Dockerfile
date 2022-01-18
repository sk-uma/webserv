FROM webserv_test_base:latest

RUN apt-get install -y nginx-full
COPY python_test/www-data /www-data

CMD service nginx start && tail -f /dev/null
