/*
 *   Copyright (c) 2014 - 2019 Oleh Kulykov <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include "librws.h"
#include "rws_socket.h"
#include "rws_memory.h"
#include "rws_string.h"
#include <assert.h>

#define READ_TIMEOUT_MS     30000
#define WRITE_TIMEOUT_MS    30000
#define RWS_CONNECT_RETRY_DELAY 200
#define RWS_CONNECT_ATTEMPS     10

#define	WSAEWOULDBLOCK      EAGAIN
#define	WSAEINPROGRESS      EINPROGRESS

// 分片传输开始，持续，结束的状态
typedef enum _rws_rd_frame_st{
    RWS_RD_FRAME_START,
    RWS_RD_FRAME_CONTINUE,
    RWS_RD_FRAME_FINISH
} rws_rd_frame_st;


// EAI_ADDRFAMILY 不支持hostname的地址族
// EAI_AGAIN 名字解析中的暂时失败
// EAI_BADFLAGS ai_flags的值无效
// EAI_FAIL 名字解析中不可恢复的失败
// EAI_FAMILY 不支持ai_family
// EAI_MEMORY 内存分配失败
// EAI_NODATA 没有与hostname相关联的地址
// EAI_NONAME hostname或service未提供，或者不可知
// EAI_SERVICE 不支持ai_socktype类型的service
// EAI_SOCKTYPE 不支持ai_socktype
// EAI_SYSTEM errno中有系统错误返回
const char * strerror_arr[] = {
    "不支持hostname的地址族",
    "名字解析中的暂时失败",
    "ai_flags的值无效",
    "名字解析中不可恢复的失败",
    "不支持ai_family",
    "内存分配失败",
    "没有与hostname相关联的地址",
    "hostname或service未提供，或者不可知",
    "不支持ai_socktype类型的service",
    "不支持ai_socktype",
    "errno中有系统错误返回"
};

static const char *gai_strerror(int ret) {
    switch(ret) {
        case EAI_FAMILY:
            return strerror_arr[4];
        case EAI_FAIL:
            return strerror_arr[3];
        case EAI_MEMORY:
            return strerror_arr[5];
        case EAI_NONAME:
            return strerror_arr[7];
        case EAI_SERVICE:
            return strerror_arr[8];
        default:
            return "";
    }
}

unsigned int rws_socket_get_next_message_id(rws_socket s) {
	const unsigned int mess_id = ++s->next_message_id;
	if (mess_id > 9999999) {
		s->next_message_id = 0;
	}
	return mess_id;
}

void rws_socket_send_ping(rws_socket s) {
	char buff[16];
	size_t len = 0;
	_rws_frame * frame = rws_frame_create();

	len = rws_sprintf(buff, 16, "%u", rws_socket_get_next_message_id(s));

	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_ping;
	rws_frame_fill_with_send_data(frame, buff, len, rws_true);
	rws_socket_append_send_frames(s, frame);
}

static void _on_recvd(rws_socket s, rws_opcode opcode, _rws_frame *frame, rws_rd_frame_st st) {
    bool is_finished = false;

    if (st == RWS_RD_FRAME_FINISH) {
        is_finished = true;
    }

    if (opcode == rws_opcode_text_frame) {
        if (s->on_recvd_text) {
            s->on_recvd_text(s, (const char *)frame->data, (unsigned int)frame->data_size, is_finished);
        }
    } else if (opcode == rws_opcode_binary_frame) {
        if (s->on_recvd_bin) {
            s->on_recvd_bin(s, frame->data, (unsigned int)frame->data_size, is_finished);
        }
    } else {
        DBG("_on_recvd opcode error,%d", opcode);
    }
}

void rws_socket_inform_recvd_frames(rws_socket s) {
	rws_bool is_all_finished = rws_true;
	_rws_frame * frame = NULL;
	_rws_node * cur = s->recvd_frames;
	while (cur) {
		frame = (_rws_frame *)cur->value.object;
        if (frame) {
			//if (frame->is_finished) {
				switch (frame->opcode) {
                    case rws_opcode_continuation:
                        // continue send finish
						if (s->on_recvd_bin) {
                        	//_on_recvd(s, s->opcode_re, frame, RWS_RD_FRAME_FINISH);
							s->on_recvd_bin(s, frame->data, (unsigned int)frame->data_size, frame->is_finished);
						}
                        break;
					case rws_opcode_text_frame:
						if (s->on_recvd_text) {
							s->on_recvd_text(s, (const char *)frame->data, (unsigned int)frame->data_size, frame->is_finished);
						}
						break;
					case rws_opcode_binary_frame:
						if (s->on_recvd_bin) {
							s->on_recvd_bin(s, frame->data, (unsigned int)frame->data_size, frame->is_finished);
						}
						break;
                    case rws_opcode_pong:
						if (s->on_recvd_pong) {
							s->on_recvd_pong(s);
						}
                        break;
					default: break;
				}
				rws_frame_delete(frame);
				cur->value.object = NULL;
			/*} else {
                if (frame->opcode > 0) {
                    // continue send start
                    s->opcode_re = frame->opcode;
                    _on_recvd(s, frame->opcode, frame, RWS_RD_FRAME_START);
                    rws_frame_delete(frame);
                    cur->value.object = NULL;
                } else if (frame->opcode == rws_opcode_continuation) {
                    // continue send continue
                    _on_recvd(s, s->opcode_re, frame, RWS_RD_FRAME_CONTINUE);
                    rws_frame_delete(frame);
                    cur->value.object = NULL;
                }
                is_all_finished = rws_false;
			}*/
		}
		cur = cur->next;
	}
	if (is_all_finished) {
		rws_list_delete_clean(&s->recvd_frames);
	}
}

void rws_socket_read_handshake_responce_value(const char * str, char ** value) {
	const char * s = NULL;
	size_t len = 0;

	while (*str == ':' || *str == ' ') {
		str++;
	}
	s = str;
	while (*s != '\r' && *s != '\n') {
		s++;
		len++;
	}
	if (len > 0) {
		*value = rws_string_copy_len(str, len);
	}
}

rws_bool rws_socket_process_handshake_responce(rws_socket s) {
	const char * str = (const char *)s->received;
	const char * sub = NULL;
	float http_ver = -1;
	int http_code = -1;
#if 0
	rws_error_delete_clean(&s->error);
	sub = strstr(str, "HTTP/");
	if (!sub) {
		return rws_false;
	}

	sub += 5;
	if (rws_sscanf(sub, "%f %i", &http_ver, &http_code) != 2) {
		http_ver = -1;
		http_code = -1;
	}

	sub = strstr(str, k_rws_socket_sec_websocket_accept); // "Sec-WebSocket-Accept"
	if (sub) {
		sub += strlen(k_rws_socket_sec_websocket_accept);
		rws_socket_read_handshake_responce_value(sub, &s->sec_ws_accept);
	}

	if (http_code != 101 || !s->sec_ws_accept) {
		s->error = rws_error_new_code_descr(rws_error_code_parse_handshake,
											(http_code != 101) ? "HTPP code not found or non 101" : "Accept key not found");
		return rws_false;
	}
#endif
	return rws_true;
}

// need close socket on error
rws_bool rws_socket_send(rws_socket s, const void * data, const size_t data_size) {
	int sended = -1, error_number = -1;
	rws_error_delete_clean(&s->error);

	//errno = -1;
#ifdef WEBSOCKET_SSL_ENABLE
    if(s->scheme && strcmp(s->scheme, "wss") == 0)
        sended = mbedtls_ssl_write(&(s->ssl->ssl_ctx), (const unsigned char *)data, (int)data_size);
    else
#endif /* WEBSOCKET_SSL_ENABLE */
	    sended = (int)send(s->socket, data, (int)data_size, 0);
        //sended = s->n.net_write(&s->n, (unsigned char *)data, (int)data_size, WRITE_TIMEOUT_MS);

    error_number = errno;

	if (sended > 0) {
		return rws_true;
	}

	rws_socket_check_write_error(s, error_number);
	if (s->error) {
		rws_socket_close(s);
		return rws_false;
	}
	return rws_true;
}

rws_bool rws_socket_recv(rws_socket s) {
#define BUFF_SIZE 8192
	int is_reading = 1, error_number = -1, len = -1;
	char * received = NULL;
	size_t total_len = 0;
	// char buff[8192];
    char *buffer;
    // static int net_status = 0;
    int ssl_status = 0;

    buffer = rws_malloc_zero(BUFF_SIZE);
    //CHECK_RET_TAG_WITH_RET(buffer, rws_false);

	rws_error_delete_clean(&s->error);
	while (is_reading) {
    #ifdef WEBSOCKET_SSL_ENABLE
        if(s->scheme && strcmp(s->scheme, "wss") == 0)
            len = mbedtls_ssl_read(&(s->ssl->ssl_ctx), (unsigned char *)buffer, BUFF_SIZE);
        else
    #endif /* WEBSOCKET_SSL_ENABLE */
		    len = (int)recv(s->socket, buffer, BUFF_SIZE, 0);
            //len = s->n.net_read(&s->n, (unsigned char *)buffer, BUFF_SIZE, READ_TIMEOUT_MS);

		error_number = errno;
        // DBG("##read len:%d, error_number:%d", len, error_number);
		if (len > 0) {
            // net_status = 0;
			total_len += len;
			if (s->received_size - s->received_len < len) {
				rws_socket_resize_received(s, s->received_size + len);
			}
			received = (char *)s->received;
			if (s->received_len) {
				received += s->received_len;
			}
			memcpy(received, buffer, len);
			s->received_len += len;
                        printf("---->>>> %s-%d, len=%d.\n", __func__, __LINE__, len);
                        for(int i=0; i< s->received_len; i++){
                            printf("%c", *(received+i));
                        }
                        printf("\n");
		} else if (len == 0) {
            /* if ret is 0 and net_status is -2, indicate the connection is closed during last call */
            // return (net_status == -2) ? net_status : readLen;
            is_reading = 0;
        } else {
            is_reading = 0;
#ifdef WEBSOCKET_SSL_ENABLE
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == len) {
                DBG("peer close -0x%04x", -len);
                // net_status = -2; /* connection is closed */
            } else if ((MBEDTLS_ERR_SSL_TIMEOUT == len)
                    || (MBEDTLS_ERR_SSL_CONN_EOF == len)
                    || (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == len)
                    || (MBEDTLS_ERR_SSL_NON_FATAL == len)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
                ssl_status = 1;
            } else {
                DBG("recv -0x%04x", -len);
                // net_status = -1;
                /* Connection error */
            }
#endif
        }
	}
	//if (error_number < 0) return rws_true;
    // DBG("net_status:%d, total_len:%d, error_number:%d", net_status, total_len, error_number);
	if (error_number != WSAEWOULDBLOCK && error_number != WSAEINPROGRESS && ssl_status != 1) {
		s->error = rws_error_new_code_descr(rws_error_code_read_write_socket, "Failed read/write socket");
		rws_socket_close(s);
        rws_free(buffer);
		return rws_false;
	}
    rws_free(buffer);
	return rws_true;
}

_rws_frame * rws_socket_last_unfin_recvd_frame_by_opcode(rws_socket s, const rws_opcode opcode) {
	_rws_frame * last = NULL;
	_rws_frame * frame = NULL;
	_rws_node * cur = s->recvd_frames;
	while (cur) {
		frame = (_rws_frame *)cur->value.object;
		if (frame) {
            //  [FIN=0,opcode !=0 ],[FIN=0,opcode ==0 ],....[FIN=1,opcode ==0 ]
			if (!frame->is_finished /*&& frame->opcode == opcode*/) {
				last = frame;
			}
		}
		cur = cur->next;
	}
	return last;
}

void rws_socket_process_bin_or_text_frame(rws_socket s, _rws_frame * frame) {
	_rws_frame * last_unfin = rws_socket_last_unfin_recvd_frame_by_opcode(s, frame->opcode);
	if (last_unfin) {
		rws_frame_combine_datas(last_unfin, frame);
		last_unfin->is_finished = frame->is_finished;
		rws_frame_delete(frame);
	} else if (frame->data && frame->data_size) {
		rws_socket_append_recvd_frames(s, frame);
	} else {
		rws_frame_delete(frame);
	}
}

void rws_socket_process_pong_frame(rws_socket s, _rws_frame * frame) {
    rws_socket_append_recvd_frames(s, frame);
}

void rws_socket_process_ping_frame(rws_socket s, _rws_frame * frame) {
	_rws_frame * pong_frame = rws_frame_create();
	pong_frame->opcode = rws_opcode_pong;
	pong_frame->is_masked = rws_true;
	rws_frame_fill_with_send_data(pong_frame, frame->data, frame->data_size, rws_true);
	rws_frame_delete(frame);
	rws_socket_append_send_frames(s, pong_frame);
}

void rws_socket_process_conn_close_frame(rws_socket s, _rws_frame * frame) {
	s->command = COMMAND_INFORM_DISCONNECTED;
	s->error = rws_error_new_code_descr(rws_error_code_connection_closed, "Connection was closed by endpoint");
	//rws_socket_close(s);
	rws_frame_delete(frame);
}

void rws_socket_process_received_frame(rws_socket s, _rws_frame * frame) {
    DBG("%s: frame->opcode:%d", __FUNCTION__, frame->opcode);
	switch (frame->opcode) {
		case rws_opcode_ping:
			DBG("%s: rws_opcode_ping\n", __FUNCTION__);
			rws_socket_process_ping_frame(s, frame);
			break;
		case rws_opcode_pong:
			DBG("%s: rws_opcode_pong\n", __FUNCTION__);
			if (s->on_recvd_pong) s->on_recvd_pong(s);
			rws_frame_delete(frame);
			break;
		case rws_opcode_text_frame:
		case rws_opcode_binary_frame:
		case rws_opcode_continuation:
			rws_socket_process_bin_or_text_frame(s, frame);
			break;
		case rws_opcode_connection_close:
			DBG("%s: rws_opcode_connection_close\n", __FUNCTION__);
			rws_socket_process_conn_close_frame(s, frame);
			break;
		default:
			// unprocessed => delete
			rws_frame_delete(frame);
			break;
	}
}

void rws_socket_idle_recv(rws_socket s) {
	_rws_frame * frame = NULL;

	if (!rws_socket_recv(s)) {
		// sock already closed
		if (s->error) {
			s->command = COMMAND_INFORM_DISCONNECTED;
		}
		return;
	}

    const size_t nframe_size = rws_check_recv_frame_size(s->received, s->received_len);
    if (nframe_size) {
        frame = rws_frame_create_with_recv_data(s->received, nframe_size);
        if (frame)  {
            rws_socket_process_received_frame(s, frame);
        }

        if (nframe_size == s->received_len) {
            s->received_len = 0;
        } else if (s->received_len > nframe_size) {
            const size_t nLeftLen = s->received_len - nframe_size;
            memmove((char*)s->received, (char*)s->received + nframe_size, nLeftLen);
            s->received_len = nLeftLen;
        }
    }
}

void rws_socket_idle_send(rws_socket s) {
	_rws_node * cur = NULL;
	rws_bool sending = rws_true;
	_rws_frame * frame = NULL;

	rws_mutex_lock(s->send_mutex);
	cur = s->send_frames;
	if (cur) {
		while (cur && s->is_connected && sending) {
			frame = (_rws_frame *)cur->value.object;
			cur->value.object = NULL;
			if (frame) {
				sending = rws_socket_send(s, frame->data, frame->data_size);
			}
			rws_frame_delete(frame);
			cur = cur->next;
		}
		rws_list_delete_clean(&s->send_frames);
		if (s->error) {
			s->command = COMMAND_INFORM_DISCONNECTED;
		}
	}
	rws_mutex_unlock(s->send_mutex);
}

void rws_socket_wait_handshake_responce(rws_socket s) {
	if (!rws_socket_recv(s)) {
		// sock already closed
		if (s->error) {
			s->command = COMMAND_INFORM_DISCONNECTED;
		}
		return;
	}

	if (s->received_len == 0) {
		return;
	}

	if (rws_socket_process_handshake_responce(s)) {
        s->received_len = 0;
		s->is_connected = rws_true;
		s->command = COMMAND_INFORM_CONNECTED;
	} else {
		rws_socket_close(s);
		s->command = COMMAND_INFORM_DISCONNECTED;
	}
}

void rws_socket_send_disconnect(rws_socket s) {
	char buff[16];
	size_t len = 0;
	_rws_frame * frame = rws_frame_create();

	len = rws_sprintf(buff, 16, "%u", rws_socket_get_next_message_id(s));

	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_connection_close;
	rws_frame_fill_with_send_data(frame, buff, len, rws_true);
	rws_socket_send(s, frame->data, frame->data_size);
	rws_frame_delete(frame);
	s->command = COMMAND_END;
	rws_thread_sleep(RWS_CONNECT_RETRY_DELAY); // little bit wait after send message
}

void rws_socket_send_handshake(rws_socket s) {
	char buff[512];
	char * ptr = buff;
	size_t writed = 0;
	writed = rws_sprintf(ptr, 512, "GET %s HTTP/%s\r\n", s->path, k_rws_socket_min_http_ver);

	if (s->port == 80) {
		writed += rws_sprintf(ptr + writed, 512 - writed, "Host: %s\r\n", s->host);
	} else {
		writed += rws_sprintf(ptr + writed, 512 - writed, "Host: %s:%i\r\n", s->host, s->port);
	}

	writed += rws_sprintf(ptr + writed, 512 - writed,
						  "Upgrade: websocket\r\n"
						  "Connection: Upgrade\r\n"
						  "Origin: %s://%s\r\n",
						  s->scheme, s->host);

	writed += rws_sprintf(ptr + writed, 512 - writed,
						  "Sec-WebSocket-Key: %s\r\n"
						  "Sec-WebSocket-Protocol: chat, superchat\r\n"
						  "Sec-WebSocket-Version: 13\r\n"
						  "\r\n",
						  "dGhlIHNhbXBsZSBub25jZQ==");

	if (rws_socket_send(s, buff, writed)) {
		s->command = COMMAND_WAIT_HANDSHAKE_RESPONCE;
	} else {
		if (s->error) {
			s->error->code = rws_error_code_send_handshake;
		} else {
			s->error = rws_error_new_code_descr(rws_error_code_send_handshake, "Send handshake");
		}
		rws_socket_close(s);
		s->command = COMMAND_INFORM_DISCONNECTED;
	}
}

struct addrinfo * rws_socket_connect_getaddr_info(rws_socket s) {
	struct addrinfo hints;
	char portstr[16];
	struct addrinfo * result = NULL;
	int ret = 0, retry_number = 0, last_ret = 0;

	rws_error_delete_clean(&s->error);

	rws_sprintf(portstr, 16, "%i", s->port);
	while (++retry_number < RWS_CONNECT_ATTEMPS) {
		result = NULL;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

		ret = getaddrinfo(s->host, portstr, &hints, &result);
		if (ret == 0 && result) {
			return result;
		}

		if (ret != 0) {
			last_ret = ret;
		}

		if (result) {
			freeaddrinfo(result);
		}

		rws_thread_sleep(RWS_CONNECT_RETRY_DELAY);
	}

	s->error = rws_error_new_code_descr(rws_error_code_connect_to_host,
										(last_ret > 0) ? gai_strerror(last_ret) : "Failed connect to host");
	s->command = COMMAND_INFORM_DISCONNECTED;
	return NULL;
}
#define RECV_TIMEOUT 1000 //1 second
void rws_socket_connect_to_host(rws_socket s) {
	struct addrinfo * result = NULL;
	struct addrinfo * p = NULL;
	rws_socket_t sock = RWS_INVALID_SOCKET;
	int retry_number = 0;

	result = rws_socket_connect_getaddr_info(s);
	if (!result) {
		return;
	}

	while ((++retry_number < RWS_CONNECT_ATTEMPS) && (sock == RWS_INVALID_SOCKET)) {
		for (p = result; p != NULL; p = p->ai_next) {
			sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (sock != RWS_INVALID_SOCKET) {
                //s->n.fd = sock;
				rws_socket_set_option(sock, SO_ERROR, 1); // When an error occurs on a socket, set error variable so_error and notify process
				rws_socket_set_option(sock, SO_KEEPALIVE, 1); // Periodically test if connection is alive
				//rws_socket_set_option(sock, SO_RCVTIMEO, RECV_TIMEOUT); // Periodically test if connection is alive
				int nNetTimeout=1000;
				setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));
				if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
                                        s->received_len = 0;
					s->socket = sock;
					fcntl(s->socket, F_SETFL, O_NONBLOCK);
					break;
				}
				RWS_SOCK_CLOSE(sock);
                                sock = RWS_INVALID_SOCKET;
			}
		}
		if (sock == RWS_INVALID_SOCKET) {
			rws_thread_sleep(RWS_CONNECT_RETRY_DELAY);
		}
	}

	freeaddrinfo(result);

	if (s->socket == RWS_INVALID_SOCKET) {
		s->error = rws_error_new_code_descr(rws_error_code_connect_to_host, "Failed connect to host");
		s->command = COMMAND_INFORM_DISCONNECTED;
	} else {
		s->command = COMMAND_SEND_HANDSHAKE;
	}
}

static void rws_socket_work_th_func(void * user_object) {
	rws_socket s = (rws_socket)user_object;
	size_t loop_number = 0;
	while (s->command < COMMAND_END) {
		loop_number++;
		rws_mutex_lock(s->work_mutex);
		printf("[%s %d] =====,s->command : %d\n",__func__,__LINE__,s->command);
		switch (s->command) {
			case COMMAND_CONNECT_TO_HOST:
                {
                #ifdef WEBSOCKET_SSL_ENABLE
                    if(s->scheme && strcmp(s->scheme, "wss") == 0) {
                        rws_ssl_conn(s); break;
                    }
                    else
                #endif /* WEBSOCKET_SSL_ENABLE */
                    {
                        rws_socket_connect_to_host(s); break;
                    }
                }
			case COMMAND_SEND_HANDSHAKE:
                {
                    rws_socket_send_handshake(s); break;
                }
			case COMMAND_WAIT_HANDSHAKE_RESPONCE:
                {
                    rws_socket_wait_handshake_responce(s); break;
                }
			case COMMAND_DISCONNECT:
                {
                    rws_socket_send_disconnect(s); break;
                }
			case COMMAND_IDLE:
				if (loop_number >= 800) {
					loop_number = 0;

					if (s->is_connected) {
						rws_socket_send_ping(s);
					}
				}

				if (s->is_connected) {
					rws_socket_idle_send(s);
				}

				if (s->is_connected) {
					rws_socket_idle_recv(s);
				}
				break;
			default: break;
		}

		rws_mutex_unlock(s->work_mutex);
		switch (s->command) {
			case COMMAND_INFORM_CONNECTED:
				s->command = COMMAND_IDLE;
				if (s->on_connected) {
					s->on_connected(s);
				}
				break;
			case COMMAND_INFORM_DISCONNECTED: {
                    s->command = COMMAND_END;
                    rws_socket_send_disconnect(s);
                    if (s->on_disconnected)  {
                        s->on_disconnected(s);
                    }
                }
				break;
			case COMMAND_IDLE:
				if (s->recvd_frames) {
					rws_socket_inform_recvd_frames(s);
				}
				break;
			default: break;
		}
		rws_thread_sleep(500);
	}

	rws_socket_close(s);
	s->work_thread = NULL;
	rws_socket_delete(s);
}

rws_bool rws_socket_create_start_work_thread(rws_socket s) {
	rws_error_delete_clean(&s->error);
	s->command = COMMAND_NONE;
	s->work_thread = rws_thread_create(&rws_socket_work_th_func, s);
	if (s->work_thread) {
		s->command = COMMAND_CONNECT_TO_HOST;
		return rws_true;
	}
	return rws_false;
}

void rws_socket_resize_received(rws_socket s, const size_t size) {
	void * res = NULL;
	size_t min = 0;
	if (size == s->received_size) {
		return;
	}

	res = rws_malloc(size);
	assert(res && (size > 0));

	min = (s->received_size < size) ? s->received_size : size;
	if (min > 0 && s->received) {
		memcpy(res, s->received, min);
	}
	rws_free_clean(&s->received);
	s->received = res;
	s->received_size = size;
}

void rws_socket_close(rws_socket s) {
    s->received_len = 0;
	if (s->socket != RWS_INVALID_SOCKET) {
#ifdef WEBSOCKET_SSL_ENABLE
        if(s->scheme && strcmp(s->scheme, "wss") == 0)
            rws_ssl_close(s);
        else
#endif /* WEBSOCKET_SSL_ENABLE */
		    RWS_SOCK_CLOSE(s->socket);

		s->socket = RWS_INVALID_SOCKET;
	}
	s->is_connected = rws_false;
}

void rws_socket_append_recvd_frames(rws_socket s, _rws_frame * frame) {
	_rws_node_value frame_list_var;
	frame_list_var.object = frame;
	if (s->recvd_frames) {
		rws_list_append(s->recvd_frames, frame_list_var);
	} else {
		s->recvd_frames = rws_list_create();
		s->recvd_frames->value = frame_list_var;
	}
}

void rws_socket_append_send_frames(rws_socket s, _rws_frame * frame) {
	_rws_node_value frame_list_var;
	frame_list_var.object = frame;
	if (s->send_frames) {
		rws_list_append(s->send_frames, frame_list_var);
	} else {
		s->send_frames = rws_list_create();
		s->send_frames->value = frame_list_var;
	}
}

rws_bool rws_socket_send_text_priv(rws_socket s, const char * text) {
	size_t len = text ? strlen(text) : 0;
	_rws_frame * frame = NULL;

	if (len <= 0) {
		return rws_false;
	}

	frame = rws_frame_create();
	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_text_frame;
	rws_frame_fill_with_send_data(frame, text, len, rws_true);
	rws_socket_append_send_frames(s, frame);

	return rws_true;
}

rws_bool rws_socket_send_bin_priv(rws_socket s, const char * bin, size_t size, rws_binary bin_type)
{
	_rws_frame * frame = NULL;

	frame = rws_frame_create();
	if (frame != NULL)
	{
		frame->is_masked = rws_true;
		switch (bin_type)
		{
			case rws_binary_start:
				//DBG("rws_binary_start\n");
				frame->opcode = rws_opcode_binary_frame;
				break;
			case rws_binary_continue:
				//DBG("rws_binary_continue\n");
				frame->opcode = rws_opcode_continuation;
				break;
			case rws_binary_finish:
				//DBG("rws_binary_finish\n");
				frame->opcode = rws_opcode_continuation;
				break;
		}
		litews_frame_fill_with_send_bin_data(frame, bin, size, bin_type);
		rws_socket_append_send_frames(s, frame);

		return rws_true;
	}
	else
	{
		/*Need to free data????*/
		return rws_false;
	}
}

rws_bool rws_socket_send_bin_start_priv(rws_socket s, const char *bin, size_t len) {
    //CHECK_RET_WITH_RET(bin, rws_false);
	_rws_frame * frame = NULL;

	if (len <= 0) {
		return rws_false;
	}

	frame = rws_frame_create();
	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_binary_frame;
	rws_frame_fill_with_send_data(frame, bin, len, rws_false);
	rws_socket_append_send_frames(s, frame);

	return rws_true;
}

rws_bool rws_socket_send_bin_continue_priv(rws_socket s, const char *bin, size_t len) {
    //CHECK_RET_WITH_RET(bin, rws_false);
	_rws_frame * frame = NULL;

	if (len <= 0) {
		return rws_false;
	}

	frame = rws_frame_create();
	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_continuation;
	rws_frame_fill_with_send_data(frame, bin, len, rws_false);
	rws_socket_append_send_frames(s, frame);

	return rws_true;
}

rws_bool rws_socket_send_bin_finish_priv(rws_socket s, const char *bin, size_t len) {
    //CHECK_RET_WITH_RET(bin, rws_false);
	_rws_frame * frame = NULL;

	if (len <= 0) {
		return rws_false;
	}

	frame = rws_frame_create();
	frame->is_masked = rws_true;
	frame->opcode = rws_opcode_continuation;
	rws_frame_fill_with_send_data(frame, bin, len, rws_true);
	rws_socket_append_send_frames(s, frame);

	return rws_true;
}

void rws_socket_delete_all_frames_in_list(_rws_list * list_with_frames) {
	_rws_frame * frame = NULL;
	_rws_node * cur = list_with_frames;
	while (cur) {
		frame = (_rws_frame *)cur->value.object;
		if (frame) {
			rws_frame_delete(frame);
		}
		cur->value.object = NULL;
        cur = cur->next;
	}
}

void rws_socket_set_option(rws_socket_t s, int option, int value) {
	setsockopt(s, SOL_SOCKET, option, (char *)&value, sizeof(int));
}

void rws_socket_check_write_error(rws_socket s, int error_num) {
	int socket_code = 0, code = 0;
	socklen_t socket_code_size = sizeof(socket_code);

	if (s->socket != RWS_INVALID_SOCKET) {
		if (getsockopt(s->socket, SOL_SOCKET, SO_ERROR, &socket_code, &socket_code_size) != 0) {
			socket_code = 0;
		}
	}

	code = (socket_code > 0) ? socket_code : error_num;
	if (code <= 0) {
		return;
	}

	switch (code) {
		// send errors
		case EACCES: //

//		case EAGAIN: // The socket is marked nonblocking and the requested operation would block
//		case EWOULDBLOCK: // The socket is marked nonblocking and the receive operation would block

		case EBADF: // An invalid descriptor was specified
		case ECONNRESET: // Connection reset by peer
		case EDESTADDRREQ: // The socket is not connection-mode, and no peer address is set
		case EFAULT: // An invalid user space address was specified for an argument
					// The receive buffer pointer(s) point outside the process's address space.
		case EINTR: // A signal occurred before any data was transmitted
					// The receive was interrupted by delivery of a signal before any data were available
		case EINVAL: // Invalid argument passed
		case EISCONN: // The connection-mode socket was connected already but a recipient was specified
		case EMSGSIZE: // The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible
		case ENOBUFS: // The output queue for a network interface was full
		case ENOMEM: // No memory available
		case ENOTCONN: // The socket is not connected, and no target has been given
						// The socket is associated with a connection-oriented protocol and has not been connected
		case ENOTSOCK: // The argument sockfd is not a socket
					// The argument sockfd does not refer to a socket
		case EOPNOTSUPP: // Some bit in the flags argument is inappropriate for the socket type.
		case EPIPE: // The local end has been shut down on a connection oriented socket
		// recv errors
		case ECONNREFUSED: // A remote host refused to allow the network connection (typically because it is not running the requested service).

			s->error = rws_error_new_code_descr(rws_error_code_read_write_socket, rws_strerror(code));
			break;

		default:
			break;
	}
}

#ifdef WEBSOCKET_SSL_ENABLE
#if defined(MBEDTLS_DEBUG_C)
#define DEBUG_LEVEL 1
#endif
static void _rws_tls_net_init( mbedtls_net_context *ctx )
{
    ctx->fd = -1;
}

// static int _rws_tls_net_connect( mbedtls_net_context *ctx, const char *host,
//                          const char *port, int proto )
// {
//     int ret;
//     struct addrinfo hints, *addr_list, *cur;

//     /* Do name resolution with both IPv6 and IPv4 */
//     memset( &hints, 0, sizeof( hints ) );
//     hints.ai_family = AF_UNSPEC;
//     hints.ai_socktype = proto == MBEDTLS_NET_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
//     hints.ai_protocol = proto == MBEDTLS_NET_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

//     if( getaddrinfo( host, port, &hints, &addr_list ) != 0 )
//         return( MBEDTLS_ERR_NET_UNKNOWN_HOST );

//     /* Try the sockaddrs until a connection succeeds */
//     ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
//     for( cur = addr_list; cur != NULL; cur = cur->ai_next )
//     {
//         ctx->fd = (int) socket( cur->ai_family, cur->ai_socktype,
//                             cur->ai_protocol );
//         if( ctx->fd < 0 )
//         {
//             ret = MBEDTLS_ERR_NET_SOCKET_FAILED;
//             continue;
//         }

//         if( connect( ctx->fd, cur->ai_addr, (socklen_t)cur->ai_addrlen ) == 0 )
//         {
//             ret = 0;
//             break;
//         }

//         close( ctx->fd );
//         ret = MBEDTLS_ERR_NET_CONNECT_FAILED;
//     }

//     freeaddrinfo( addr_list );

//     return( ret );
// }

static void _rws_tls_net_free( mbedtls_net_context *ctx )
{
    if( ctx->fd == -1 )
        return;

    shutdown( ctx->fd, 2 );
    close( ctx->fd );

    ctx->fd = -1;
}

static int s_send_timeout_ms = WRITE_TIMEOUT_MS;
static int _rws_tls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;

    int fd = ((mbedtls_net_context *) ctx)->fd;
    struct timeval interval = {s_send_timeout_ms / 1000, (s_send_timeout_ms % 1000) * 1000};

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 100)) {
        interval.tv_sec = 0;
        interval.tv_usec = 10000;
    }

#if 0 // FIXME: not support yet, need to fix it later
    /*set send timeout to avoid send block*/
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&interval,
               sizeof(struct timeval))) {
        return -1;
    }
#endif

    ret = (int) send( fd, buf, len, 0);

    if( ret < 0 )
    {
        if( errno == EPIPE || errno == ECONNRESET )
            return( MBEDTLS_ERR_NET_CONN_RESET );

        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_WRITE );

        return( MBEDTLS_ERR_NET_SEND_FAILED );
    }

    return( ret );
}

static int _rws_tls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    ret = (int) recv( fd, buf, len, 0);
    // DBG("@@ret:%d, errno:%d", ret, errno);
    if( ret < 0 )
    {
        // if( errno == EAGAIN )
        //     return( MBEDTLS_ERR_SSL_WANT_READ );

        if( errno == EPIPE || errno == ECONNRESET )
            return( MBEDTLS_ERR_NET_CONN_RESET );

        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );

        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    return( ret );
}

static int _rws_tls_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((mbedtls_net_context *) ctx)->fd;

    if( fd < 0 )
        return( MBEDTLS_ERR_NET_INVALID_CONTEXT );

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    /* no wait if timeout == 0 */
    ret = select( fd + 1, &read_fds, NULL, NULL, &tv );

    /* Zero fds ready means we timed out */
    if( ret == 0 )
        return( MBEDTLS_ERR_SSL_TIMEOUT );

    if( ret < 0 )
    {
        if( errno == EINTR )
            return( MBEDTLS_ERR_SSL_WANT_READ );

        return( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    /* This call will not block */
    return( _rws_tls_net_recv( ctx, buf, len ) );
}

void transport_utils_random(unsigned char *output, size_t output_len)
{
#if defined(CONFIG_TEE_CA)
    csi_tee_rand_generate(output, output_len);
#else
    int i;
    uint32_t random;
    int mod = output_len % 4;
    int count = 0;
    static uint32_t rnd = 0x12345;
    for (i = 0; i < output_len / 4; i++) {
        random = rnd * 0xFFFF777;
        rnd = random;
        output[count++] = (random >> 24) & 0xFF;
        output[count++] = (random >> 16) & 0xFF;
        output[count++] = (random >> 8) & 0xFF;
        output[count++] = (random) & 0xFF;
    }
    random = rnd * 0xFFFF777;
    rnd = random;
    for (i = 0; i < mod; i++) {
        output[i + count] = (random >> 8 * i) & 0xFF;
    }
#endif
}

static int _rws_random(void *p_rng, unsigned char *output, size_t output_len)
{
    (void)p_rng;
    transport_utils_random(output, output_len);
    return 0;
}

static void _rws_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    DBG("%s", str);
}

int rws_ssl_conn(rws_socket s)
{
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
    // const char *pers = "https";
    int value, ret = 0;
    uint32_t flags;
    // char port[10] = {0};
    _rws_ssl *ssl;

    s->ssl = rws_malloc_zero(sizeof(_rws_ssl));
    if (!s->ssl) {
        DBG("Memory malloc error.");
        ret = -1;
        goto exit;
    }
    ssl = s->ssl;
#if 1
    if (s->server_cert)
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
#endif
    /*
     * Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    _rws_tls_net_init(&ssl->net_ctx);
    // mbedtls_net_init(&ssl->net_ctx);
    mbedtls_ssl_init(&ssl->ssl_ctx);
    mbedtls_ssl_config_init(&ssl->ssl_conf);
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
    mbedtls_pk_init(&ssl->pkey);
    // mbedtls_ctr_drbg_init(&ssl->ctr_drbg);
    // mbedtls_entropy_init(&ssl->entropy);
    // if ((value = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg,
    //                            mbedtls_entropy_func,
    //                            &ssl->entropy,
    //                            (const unsigned char*)pers,
    //                            strlen(pers))) != 0) {
    //     DBG("mbedtls_ctr_drbg_seed() failed, value:-0x%x.", -value);
    //     ret = -1;
    //     goto exit;
    // }

    /*
    * Load the Client certificate
    */
    if (s->client_cert && s->client_pk) {
        ret = mbedtls_x509_crt_parse(&ssl->clicert, (const unsigned char *)s->client_cert, s->client_cert_len);
        if (ret < 0) {
            DBG("Loading cli_cert failed! mbedtls_x509_crt_parse returned -0x%x.", -ret);
            goto exit;
        }

        ret = mbedtls_pk_parse_key(&ssl->pkey, (const unsigned char *)s->client_pk, s->client_pk_len, NULL, 0);
        if (ret != 0) {
            DBG("failed! mbedtls_pk_parse_key returned -0x%x.", -ret);
            goto exit;
        }
    }

    /*
    * Load the trusted CA
    */
    /* cert_len passed in is gotten from sizeof not strlen */
    if (s->server_cert && ((value = mbedtls_x509_crt_parse(&ssl->cacert,
                                        (const unsigned char *)s->server_cert,
                                        s->server_cert_len)) < 0)) {
        DBG("mbedtls_x509_crt_parse() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    /*
     * Start the connection
     */
    /*
    snprintf(port, sizeof(port), "%d", client->port) ;
    if ((ret = mbedtls_net_connect(&ssl->net_ctx, host, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        DBG("failed! mbedtls_net_connect returned %d, port:%s.", ret, port);
        goto exit;
    }*/
    DBG("start to connect to host");
    rws_socket_connect_to_host(s);
    if (s->socket == RWS_INVALID_SOCKET){
        DBG("failed! mbedtls_net_connect returned.");
        goto exit;
    } else {
        DBG("connect to host ok");
        ssl->net_ctx.fd = s->socket;
    }

    /*
     * Setup stuff
     */
    if ((value = mbedtls_ssl_config_defaults(&ssl->ssl_conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        DBG("mbedtls_ssl_config_defaults() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }
    mbedtls_ssl_conf_read_timeout(&ssl->ssl_conf, READ_TIMEOUT_MS);

    // TODO: add customerization encryption algorithm
    memcpy(&ssl->profile, ssl->ssl_conf.cert_profile, sizeof(mbedtls_x509_crt_profile));
    ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_MD5);
    mbedtls_ssl_conf_cert_profile(&ssl->ssl_conf, &ssl->profile);

    mbedtls_ssl_conf_authmode(&ssl->ssl_conf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->ssl_conf, &ssl->cacert, NULL);

    if (s->client_cert && (ret = mbedtls_ssl_conf_own_cert(&ssl->ssl_conf, &ssl->clicert, &ssl->pkey)) != 0) {
        DBG(" failed! mbedtls_ssl_conf_own_cert returned %d.", ret );
        goto exit;
    }

    mbedtls_ssl_conf_rng(&ssl->ssl_conf, _rws_random, NULL);
    mbedtls_ssl_conf_dbg(&ssl->ssl_conf, _rws_debug, NULL);
    // mbedtls_ssl_conf_rng(&ssl->ssl_conf, mbedtls_ctr_drbg_random, &ssl->ctr_drbg);

    if ((value = mbedtls_ssl_setup(&ssl->ssl_ctx, &ssl->ssl_conf)) != 0) {
        DBG("mbedtls_ssl_setup() failed, value:-0x%x.", -value);
        ret = -1;
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, _rws_tls_net_send, _rws_tls_net_recv, _rws_tls_net_recv_timeout);
    // mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);
	//mbedtls_net_set_block(&ssl->net_ctx);

    DBG("start to handshake...");
    /*
    * Handshake
    */
    while ((ret = mbedtls_ssl_handshake(&ssl->ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            DBG("mbedtls_ssl_handshake() failed, ret:-0x%x.", -ret);
            ret = -1;
            goto exit;
        }
    }
    mbedtls_ssl_conf_read_timeout(&ssl->ssl_conf, 5);
    /*
     * Verify the server certificate
     */
    /* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
        * handshake would not succeed if the peer's cert is bad.  Even if we used
        * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */
    if ((flags = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx)) != 0) {
#if defined(MBEDTLS_DEBUG_C)
        char vrfy_buf[512];
        DBG("svr_cert varification failed. authmode:%d", authmode);
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        DBG("%s", vrfy_buf);
#endif
    }
    else
        DBG("svr_cert varification ok. authmode:%d", authmode);

exit:
    if(ret != 0) {
        DBG("ret=%d.", ret);
    	s->error = rws_error_new_code_descr(rws_error_code_connect_to_host, "Failed connect to host");
        DBG("%s: code:%d, error:%s", __FUNCTION__, s->error->code, s->error->description);
    	s->command = COMMAND_INFORM_DISCONNECTED;
    }
    return ret;
}

int rws_ssl_close(rws_socket s)
{
    _rws_ssl *ssl = s->ssl;
    s->client_cert = NULL;
    s->server_cert = NULL;
    s->client_pk = NULL;

    if (!ssl)
        return -1;

    mbedtls_ssl_close_notify(&ssl->ssl_ctx);
    _rws_tls_net_free(&ssl->net_ctx);
    // mbedtls_net_free(&ssl->net_ctx);
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_x509_crt_free(&ssl->clicert);
    mbedtls_pk_free(&ssl->pkey);
    mbedtls_ssl_free(&ssl->ssl_ctx);
    mbedtls_ssl_config_free(&ssl->ssl_conf);
    // mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
    // mbedtls_entropy_free(&ssl->entropy);

    rws_free(ssl);
    return 0;
}
#endif
