# libcurl2rtt
The curl library ported on the RT-Thread platform. | 基于 RT-Thread 移植的 curl 库. 

注意测试的命令行的时候，需要讲shell线程的栈空间改大，否则系统线程会写穿，导致进入HF

```
msh /etc/certs>curl --verbose --capath ./ https://www.rt-thread.com/service/rt-thread.txt
* timeout on name lookup is not supported
*   Trying 118.31.15.152:443...
* TCP_NODELAY set
* Connected to www.rt-thread.com () port 443 (#0)
* mbedTLS: Connecting to www.rt-thread.com:443
* mbedTLS: Set min SSL version to TLS 1.0
* mbedTLS: Handshake complete, cipher is TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256
* Dumping cert info:
* cert. version     : 3
* serial number     : 02:1C:94:E5:9C:4C:CE:18:54:87:B2:A0:FF:85:F4:0C
* issuer name       : C=US, O=DigiCert Inc, OU=www.digicert.com, CN=Encryption Everywhere DV TLS CA - G1
* subject name      : CN=www.rt-thread.com
* issued  on        : 2019-06-23 00:00:00
* expires on        : 2020-06-22 12:00:00
* signed using      : RSA with SHA-256
* RSA key size      : 2048 bits
* basic constraints : CA=false
* subject alt name  : www.rt-thread.com, rt-thread.com
* key usage         : Digital Signature, Key Encipherment
* ext key usage     : TLS Web Server Authentication, TLS Web Client Authentication

* SSL connected
> GET /service/rt-thread.txt HTTP/1.1
> Host: www.rt-thread.com
> User-Agent: curl/7.67.0-DEV
> Accept: */*
> 
* Mark bundle as not supporting multiuse
< HTTP/1.1 200 OK
< Server: nginx/1.10.3 (Ubuntu)
< Date: Sun, 29 Dec 2019 17:21:10 GMT
< Content-Type: text/plain
< Content-Length: 267
< Connection: keep-alive
< Last-Modified: Tue, 18 Sep 2018 03:55:48 GMT
< ETag: "10b-5761d43d95900"
< Accept-Ranges: bytes
< Vary: Accept-Encoding
< 
RT-Thread is an open source IoT operating system from China, which has strong scalability: from a tiny kernel running on a tiny core, for example ARM Cortex-M0, or Cortex-M3/4/7, to a rich feature system running on MIPS32, ARM Cortex-A8, ARM Cortex-A9 DualCore etc.
* Connection #0 to host www.rt-thread.com left intact
msh /etc/certs>
```
