import requests

if __name__ == '__main__':
    response = requests.get('http://127.0.0.1:4242')
    print(response.text)
    with open("../www/index.html") as f:
        file_text = f.read()
    print(file_text)
