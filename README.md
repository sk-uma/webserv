# webserv
like in nginx

# 現状できること
・引数なしでwebserv起動  
・localhost:5050 or 6060 or 7070 で接続  
・パスなしで起動させた場合はwww/index.htmlを開く  
・パスを指定した場合はwwwをルートディレクトリとして、ファイルを開く  
・ディレクトリを指定した場合は、そのディレクトリのファイル構成がレスポンスとして帰る  
・www/cgi-binディレクトリのpythonファイルを指定した場合、pythonファイルが実行される  
・curl -X POST localhost:5050/Upload -F "upfile=<filepath>" のような指定方法でファイルアップロードされる
・DELETE メソッドでパスを指定すると、そのパスのファイルが消去される curl -X DELETE localhost:5050/cgi-bin/test.py  なら、cgi-binディレクトリのtest.pyファイルが消去される。
