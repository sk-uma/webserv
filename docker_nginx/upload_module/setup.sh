mkdir -p /var/www/html/{a..c}
echo "a file" > /var/www/html/a/index.html
echo "b file" > /var/www/html/b/index.html
echo "c file" > /var/www/html/c/index.html

service nginx start;
service fcgiwrap start;

tail -f /dev/null