使用Qt开发客户端。
使用nginx作为代理服务器，转发客户端请求给后台FastCGI进行处理。
FastCGI对Nginx转发过来的请求进行解析处理。
FastDFS集群中storage节点保存着客户端的文件。
Redis保存着客户端的信息（token）等。
HTTP接口的使用。
JSON协议解析。
