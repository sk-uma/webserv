import docker
import os


WEBSERV_CONTAINER_NAME = "webserv_comparison_container"
NGINX_CONTAINER_NAME = "nginx_comparison_container"


def main():
    client = docker.from_env()
    setup(client)


def setup(client):
    build_images(client)
    run_containers(client)


def run_containers(client):
    client.containers.run("webserv_test:latest", name="webserv_comparison_container", detach=True)
    client.containers.run("nginx_test:latest", name="nginx_comparison_container", detach=True)


def build_images(client):
    image, generator = client.images.build(
        path="./",
        dockerfile=os.path.dirname(os.path.abspath(__file__))+"/base.Dockerfile",
        tag="webserv_test_base:latest"
    )
    print('\033[32m' + 'webserv_test_base:latest image created' + '\033[0m')
    ws_image, ws_generator = client.images.build(
        path="../",
        dockerfile=os.path.dirname(os.path.abspath(__file__))+"/webserv.Dockerfile",
        tag="webserv_test:latest"
    )
    print('\033[32m' + 'webserv_test:latest image created' + '\033[0m')
    nx_image, nx_generator = client.images.build(
        path="../",
        dockerfile=os.path.dirname(os.path.abspath(__file__))+"/nginx.Dockerfile",
        tag="nginx_test:latest"
    )
    print('\033[32m' + 'nginx_test:latest image created' + '\033[0m')


if __name__ == '__main__':
    main()
