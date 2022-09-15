# webserv
HTTP1.1 server affected by nginx.  
It includes the minimum necessary functions and can start the server with simple settings.  
It is also possible to run cgi.  

## usage

### Contexts

#### basic syntax
There are three contexts, main, server, and location, and the settings of each context are inherited by lower contexts.
Semicolons after each setting are ignored. It is possible to omit it.  
Each block is indented by 4 spaces. No other tabs, double spaces, etc. are allowed.  
Statements enclosed in double colons are treated as one token.  

example
```
server {
    location /location/path {
        ${key} ${value};
    }
}
```

#### three contexts
- main context  
Configure common server settings.  

- server context  
Set up one virtual server.  
If there are multiple servers that match the conditions, the setting of the first server will be applied.  
The settings of the main context written before the context are not inherited.  

- location context  
Settings to be applied when path locations match.  
As with the server context, the settings written in the server context after the context are not inherited.  
Location contexts are never contained within location contexts.  

### Directives

#### listen
```
syntax : listen ${address}:${port};
default: listen 0.0.0.0:80;
context: server
```
Specifies the address and port that the server should listen on.  
The address and port must be specified explicitly.  

#### index
```
syntax : index ${file...};
default: index index.html;
context: main, server, location
```
Specifies the file to return for access to the directory.  
The first matching file is returned in order from the beginning.  

#### error_page
```
syntax : error_page ${code...} ${uri};
default: None
context: main, server, location
```
Set the error page to return when an error code is issued.  

#### autoindex
```
syntax : autoindex (on|off);
default: autoindex off;
context: main, server, location
```
Should autoindex run if index doesn't exist?  

#### client_max_body_size
```
syntax : client_max_body_size ${size};
default: client_max_body_size 1048576;
context: main, server, location
```
Specifies the maximum size that the client can include in the body when requesting.  

#### limit_except_by_deny_all
```
syntax : limit_except_by_deny_all ${mothod...};
default: limit_except_by_deny_all HEAD POST DELETE GET;
context: server, location
```
Specifies executable requests for requests from all addresses.  

#### server_name
```
syntax : server_name ${name...};
default: server_name "";
context: server
```
Set the name of the server.  

#### return
```
syntax : return ${code} ${text};
         return ${code} ${URL};
default: None
context: server, location
```
If set, code and text or uri will be returned unconditionally.  

#### upload_path
```
syntax : upload_path ${directory};
default: None
context: server, location
```
Set the save destination of uploaded files when uploading files is permitted.  
If not set, the file will not be saved.  

#### root
```
syntax : root ${path};
default: room /var/www/html;
context: main, server, location
```
Specify the root of the files to be distributed.  
Specify the relative path from the directory where the server's executable file is located.  


#### cgi_extension
```
syntax : cgi_extension ${extension...};
default: None
context: main, server, location
```
Specify the extension to execute cgi.  
You can execute a binary file or a file containing a shebang with execute permission.  
