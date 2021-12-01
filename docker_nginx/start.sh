docker build -t nginx_test .
docker run -p 8080:8080 -p 8081:8081 --name nginx_container -d -it nginx_test
