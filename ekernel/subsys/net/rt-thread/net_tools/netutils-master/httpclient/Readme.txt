/*#####################################################################
# File Describe:test.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年03月21日 星期六 10时48分06秒
#====================================================================*/
server:
1.http_server用于自己搭建基于tcp的http服务器，测试http协议时可以看到服务器的信息，便于分析问题．
　　　pc上编译gcc -o http_server http_server.c
　　　执行sudo ./http_server  --host 192.168.31.6 --port 80(1024以下端口需要sudo执行)
　　　默认host:127.0.0.1, port:80
2.sudo python2.7 -m SimpleHTTPServer 80(注意根目录为执行该命令时的目录，访问时留意url的路径).

client:
1.可以直接在浏览器输入http://127.0.0.1:80即可访问目录和文件(用自己搭建时的ip)．
2.也可以用usr_http: http测试用例，可以理解为客户端．
3.或者用wget直接下载文件测试．
