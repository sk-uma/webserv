import requests
import mimetypes
import os
import shutil


def compare_get_url_file(url: str, path: str, log_level: int = 1):
    response = requests.get(url)
    # print("Response:")
    # print(response.text)
    with open(path) as f:
        file_text = f.read()
    # print("File:")
    # print(file_text)
    file_text += "\r\n"
    if response.text == file_text:
        if log_level >= 1:
            print("\033[32m" + "[OK] " + "\033[0m", end="")
            print(url + ", " + path)
    else:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print(url + ", " + path)
        if log_level >= 2:
            print("\033[33m" + "Response:" + "\033[0m", "status: " + str(response.status_code))
            print(response.text, end="EOF\n")
            print(len(response.text))
            print("\033[33m" + "File:" + "\033[0m")
            print(file_text, end="EOF\n")


def compare_get_status_code(url: str, status_code: int, log_level: int = 1):
    response = requests.get(url)
    if status_code == response.status_code:
        if log_level >= 1:
            print("\033[32m" + "[OK] " + "\033[0m", end="")
            print(url, status_code, sep=", ")
    else:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print(url, status_code, sep=", ")
        if log_level >= 2:
            print("\033[33m" + "Response:" + "\033[0m", "status: " + str(response.status_code))
            print(response.status_code)
            print("\033[33m" + "Expectation:" + "\033[0m")
            print(status_code)


def test_virtualhost(url: str, text: str, host: str, log_level: int = 1):
    headers = {'Host': host}
    response = requests.get(url, headers=headers)
    text += "\r\n"
    if response.text == text:
        if log_level >= 1:
            print("\033[32m" + "[OK] " + "\033[0m", end="")
            print(url, sep=", ")
    else:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print(url, sep=", ")
        if log_level >= 2:
            print("\033[33m" + "Response:" + "\033[0m", "status: " + str(response.status_code))
            print(response.text)
            print("\033[33m" + "Expectation:" + "\033[0m")
            print(text)


def test_cgi(url: str, text: str = None, path: str = None, code: int = -1, log_level: int = 1):
    response = requests.get(url)
    if path is not None:
        with open(path) as f:
            text = f.read()
    if (text is None or text == response.text) and (code == -1 or response.status_code == code):
        if log_level >= 1:
            print("\033[32m" + "[OK] " + "\033[0m", end="")
            print(url, sep=", ")
    else:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print(url, sep=", ")
        if log_level >= 2:
            print("\033[33m" + "Response:" + "\033[0m", "status: " + str(response.status_code))
            print(response.text)
            print("\033[33m" + "Expectation:" + "\033[0m")
            print(text)


def test_upload(url: str, src_path: str, dst_path: str, log_level: int = 1):
    src_filename = os.path.basename(src_path)
    # file_path = "test_contents/" + src_filename
    mine = mimetypes.guess_type(src_filename)
    with open(src_path, 'rb') as f:
        file_data = f.read()
    files = {'Uploadfile': (src_filename, file_data, str(mine[0]))}
    # if src_filename == 'zen.txt':
    #     print(files)
    response = requests.post(url, files=files)
    # print(response.status_code)
    # print(response.text)
    if response.status_code != 200:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print("fail upload", url, sep=", ")
        if log_level >= 2:
            print("\033[33m" + "Response:" + "\033[0m", "status: " + str(response.status_code))
            print(200)
            print("\033[33m" + "Expectation:" + "\033[0m")
            print(response.status_code)
    # dst_path = "tmp/" + src_filename
    with open(dst_path, 'rb') as f:
        upload_data = f.read()
    if file_data == upload_data:
        if log_level >= 1:
            print("\033[32m" + "[OK] " + "\033[0m", end="")
            print(url, src_filename, sep=", ")
    else:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print("difference file", url, sep=", ")
    # os.remove(dst_path)


def test_delete(url: str, del_path: str, log_level: int = 1):
    # shutil.copy("test_contents/"+filename, "tmp/"+filename)
    response = requests.delete(url)
    # print(response.status_code)
    if response.status_code != 200:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print("fail delete", url, sep=", ")
        if log_level >= 2:
            print("\033[33m" + "Response:" + "\033[0m", "status: " + str(response.status_code))
            print(200)
            print("\033[33m" + "Expectation:" + "\033[0m")
            print(response.status_code)
    if not os.path.exists(del_path):
        if log_level >= 1:
            print("\033[32m" + "[OK] " + "\033[0m", end="")
            print(url, del_path, sep=", ")
    else:
        if log_level >= 1:
            print("\033[31m" + "[KO] " + "\033[0m", end="")
            print(url, sep=", ")


if __name__ == '__main__':
    log_level = 1

    # normal Get
    print("\n===>", "\033[34m" + "normal Get" + "\033[0m", "<===", end="\n")

    compare_get_url_file('http://127.0.0.1:8080/hello.html', 'www-data/hello.html', log_level=log_level)
    compare_get_url_file('http://127.0.0.1:8080/contents/world.html', 'www-data/contents/world.html', log_level=log_level)

    # index Get
    print("\n===>", "\033[34m" + "index Get" + "\033[0m", "<===", end="\n")

    compare_get_url_file('http://127.0.0.1:8080', 'www-data/index.html', log_level=log_level)
    compare_get_url_file('http://127.0.0.1:8080/contents', 'www-data/contents/index.html', log_level=log_level)
    compare_get_url_file('http://127.0.0.1:8080/contents/', 'www-data/contents/index.html', log_level=log_level)

    # Autoindex
    print("\n===>", "\033[34m" + "Autoindex" + "\033[0m", "<===", end="\n")

    compare_get_status_code('http://127.0.0.1:8081/contents', 200, log_level=log_level)
    compare_get_status_code('http://127.0.0.1:8081/contents/', 200, log_level=log_level)

    # virtual host
    print("\n===>", "\033[34m" + "virtual host" + "\033[0m", "<===", end="\n")

    test_virtualhost('http://127.0.0.1:5050', 'ServerA', 'serverA.com', log_level=log_level)
    test_virtualhost('http://127.0.0.1:5050', 'ServerB', 'serverB.com', log_level=log_level)
    test_virtualhost('http://127.0.0.1:5050', 'ServerC', 'serverC.com', log_level=log_level)
    test_virtualhost('http://127.0.0.1:5050', 'ServerA', '_', log_level=log_level)
    test_virtualhost('http://0.0.0.0:5051', 'ServerE', 'serverD.com', log_level=log_level)
    test_virtualhost('http://0.0.0.0:5051', 'ServerE', '_', log_level=log_level)
    test_virtualhost('http://127.0.0.42:5051', 'ServerD', 'serverD.com', log_level=log_level)
    test_virtualhost('http://127.0.0.42:5051', 'ServerD', '_', log_level=log_level)
    test_virtualhost('http://127.0.0.1:5051', 'ServerE', 'serverE.com', log_level=log_level)
    test_virtualhost('http://127.0.0.1:5051', 'ServerF', 'serverF.com', log_level=log_level)
    test_virtualhost('http://127.0.0.1:5051', 'ServerE', 'serverZZZ.com', log_level=log_level)

    # CGI
    print("\n===>", "\033[34m" + "CGI" + "\033[0m", "<===", end="\n")

    test_cgi('http://127.0.0.1:8082/cgi-bin/sample.py', text='sample CGI\n', log_level=log_level)
    test_cgi('http://127.0.0.1:8082/cgi-bin/error.py', code=500, log_level=log_level)
    # test_cgi_env('http://127.0.0.1:8082/cgi-bin/env.py?a=4&b=2/echo', log_level=log_level)

    # upload
    print("\n===>", "\033[34m" + "Upload" + "\033[0m", "<===", end="\n")

    if not os.path.exists("tmp"):
        os.mkdir('tmp')
    test_upload('http://127.0.0.1:8083/Upload', 'test_contents/zen.txt', 'tmp/zen.txt', log_level=log_level)
    test_upload('http://127.0.0.1:8083/Upload', 'test_contents/sample.pdf', 'tmp/sample.pdf', log_level=log_level)
    test_upload('http://127.0.0.1:8083/Upload', 'test_contents/animal.png', 'tmp/animal.png', log_level=log_level)

    # delete
    print("\n===>", "\033[34m" + "Delete" + "\033[0m", "<===", end="\n")

    if not os.path.exists("delete_tmp"):
        shutil.copytree("test_contents", "delete_tmp")
    test_delete('http://127.0.0.1:8084/zen.txt', 'delete_tmp/zen.txt', log_level=log_level)
    test_delete('http://127.0.0.1:8084/sample.pdf', 'delete_tmp/sample.pdf', log_level=log_level)
    test_delete('http://127.0.0.1:8084/animal.png', 'delete_tmp/animal.png', log_level=log_level)
    test_delete('http://127.0.0.1:8084/dir/hello.txt', 'delete_tmp/dir/hello.txt', log_level=log_level)
    shutil.rmtree("delete_tmp")
