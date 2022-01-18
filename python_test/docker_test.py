import docker
import docker_cntl

def main():
    client = docker.from_env()
    ws, nx = get_container(client)
    res: bool = ws.exec_run(cmd="pgrep webserv").exit_code
    res = res or nx.exec_run(cmd="service nginx status").exit_code
    if res:
        print("Error", res)
        exit(1)


def get_container(client):
    try:
        webserv_container = client.containers.get(docker_cntl.WEBSERV_CONTAINER_NAME)
        nginx_container = client.containers.get(docker_cntl.NGINX_CONTAINER_NAME)
    except docker.errors.NotFound as e:
        docker_cntl.setup(client)
        webserv_container = client.containers.get(docker_cntl.WEBSERV_CONTAINER_NAME)
        nginx_container = client.containers.get(docker_cntl.NGINX_CONTAINER_NAME)
    return webserv_container, nginx_container


if __name__ == '__main__':
    main()
