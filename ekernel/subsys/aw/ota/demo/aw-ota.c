#include <stdio.h>
#include <string.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <lwip/netdb.h>
#include <lwip/opt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include "aw_upgrade.h"
#include <env.h>

#define OTA_CONFIG_FILE "/data/ota.conf"
#define MAX_OTA_PARA_SIZE (1024)

#define min(a, b) (((a) < (b)) ? (a) : (b))
#undef write
#undef read
struct resp_header
{
	int status_code;//HTTP/1.1 '200' OK
	char content_type[128];//Content-Type: application/gzip
	long content_length;//Content-Length: 11683079
	char file_name[256];
};

static struct resp_header resp;

static void parse_url(const char *url, const char **newurl_p, char *domain, int *port, char *file_name)
{
	int i, j = 0;
	int start = 0;
	*port = 80;
	char *patterns[] = {"http://", "https://", NULL};

	for (i = 0; patterns[i]; i++)
		if (strncmp(url, patterns[i], strlen(patterns[i])) == 0) {
			start = strlen(patterns[i]);
		}

	for (i = start; url[i] != '/' && url[i] != '\0'; i++, j++)
		domain[j] = url[i];
	domain[j] = '\0';

	*newurl_p = url + i + 1;
	printf("start:%d\n", start);
	printf("url:%s\n newurl:%s\n", url, *newurl_p);

	char *pos = strstr(domain, ":");
	if (pos)
		sscanf(pos, ":%d", port);

	for (i = 0; i < (int)strlen(domain); i++)
	{
		if (domain[i] == ':')
		{
			domain[i] = '\0';
			break;
		}
	}

	j = 0;
	for (i = start; url[i] != '\0'; i++)
	{
		if (url[i] == '/')
		{
			if (i !=  strlen(url) - 1)
				j = 0;
			continue;
		}
		else
			file_name[j++] = url[i];
	}
	file_name[j] = '\0';
}

static struct resp_header get_resp_header(const char *response)
{
	struct resp_header resp;

	char *pos = strstr(response, "HTTP/");
	if (pos)
		sscanf(pos, "%*s %d", &resp.status_code);

	pos = strstr(response, "Content-Type:");
	if (pos)
		sscanf(pos, "%*s %10s", resp.content_type);
		//sscanf(pos, "%*s %10s", &resp.content_type);

	pos = strstr(response, "Content-Length:");
	if (pos)
		sscanf(pos, "%*s %ld", &resp.content_length);

	return resp;
}

static void get_ip_addr(char *domain, char *ip_addr)
{
	printf("domain:%s\n",domain);
	struct hostent *host = gethostbyname(domain);
	if (!host)
	{
		ip_addr = NULL;
		return;
	}

	for (int i = 0; host->h_addr_list[i]; i++)
	{
		strcpy(ip_addr, inet_ntoa( * (struct in_addr*) host->h_addr_list[i]));
		break;
	}
}

static void progressBar(long cur_size, long total_size)
{
	float percent = (float) cur_size / total_size;
	const int numTotal = 50;
	int numShow = (int)(numTotal * percent);

	if (numShow == 0)
		numShow = 1;

	if (numShow > numTotal)
		numShow = numTotal;

	char sign[51] = {0};
	memset(sign, '=', numTotal);

	printf("\r%.2f%%\t[%-*.*s] %.2f/%.2fMB",
			percent * 100,
			numTotal,
			numShow,
			sign,
			cur_size / 1024.0 / 1024.0,
			total_size / 1024.0 / 1024.0);
	fflush(stdout);

	if (numShow == numTotal)
		printf("\nDownload 100.00%%\n");
}

#if 0
void * download(void * socket_d)
{
	int client_socket = *(int *) socket_d;
	int length = 0;
	int mem_size = 4096;//mem_size might be enlarge, so reset it
	int buf_len = mem_size;//read 4k each time
	int len;
	char rtos_file_name[256+6];
	strcpy(rtos_file_name, "/data/");
	strcpy(rtos_file_name+6, resp.file_name);
	printf("rtos_file_name: %s ^_^\n\n",rtos_file_name);

	int fd = open(rtos_file_name, O_CREAT | O_WRONLY, S_IRWXG | S_IRWXO | S_IRWXU);
	if (fd < 0)
	{
		printf("Create file failed\n");
		exit(-1);
	}

	char *buf = (char *) malloc(mem_size * sizeof(char));

	while (length < resp.content_length && (len = lwip_read(client_socket, buf, buf_len)) != 0 )
	{
		write(fd, buf, len);
		length += len;
		progressBar(length, resp.content_length);
	}

	if (length == resp.content_length)
		printf("\nDownload successful ^_^\n\n");
	else
		printf("\nLength %d resp.content_length:%d\n\n", length, resp.content_length);

	if (buf)
		free(buf);
	if (fd >= 0)
		close(fd);
}
#endif

void * download_rtos(void * socket_d)
{
	int client_socket = *(int *) socket_d;
	int length = 0;
	int mem_size = 4096;//mem_size might be enlarge, so reset it
	int buf_len = mem_size;//read 4k each time
	int len, ret, is_last_slice = 0;


	char *buf = (char *) malloc(mem_size * sizeof(char));
	if (buf == NULL)
		goto download_rtos_out;


	while (length < resp.content_length && (len = lwip_read(client_socket, buf, buf_len)) != 0 )
	{
		ret = aw_upgrade_slice("rtos", buf, length, len, is_last_slice);
		if (ret)
			goto download_rtos_out;
		length += len;
		progressBar(length, resp.content_length);
	}

	if (length == resp.content_length) {
		printf("\nDownload successful ^_^\n\n");
		aw_upgrade_end(0);
	} else {
		printf("\nLength %d resp.content_length:%ld\n\n", length, resp.content_length);
	}

download_rtos_out:
	if (buf)
		free(buf);
	return NULL;
}

#if 0
int cmd_wgets(int argc, char ** argv)
{
	/*
	   test url:
	   1. https://nodejs.org/dist/v4.2.3/node-v4.2.3-linux-x64.tar.gz
	   2. http://img.ivsky.com/img/tupian/pre/201312/04/nelumbo_nucifera-009.jpg
	   */
	char url[2048] = "127.0.0.1";
	char *newurl;
	char domain[64] = {0};
	char ip_addr[16] = {0};
	int port = 80;
	char file_name[256] = {0};

	if (argc == 1)
	{
		printf("Input a valid URL please\n");
		return -1;
	}
	else
		strcpy(url, argv[1]);

	puts("1: Parsing url...");
	parse_url(url, &newurl, domain, &port, file_name);
	printf("url:%s\n newurl:%s\n", url, newurl);

	if (argc == 3)
		strcpy(file_name, argv[2]);

	puts("2: Get ip address...");
	get_ip_addr(domain, ip_addr);
	if (strlen(ip_addr) == 0)
	{
		printf("can not get ip address\n");
		return 0;
	}

	puts("\n>>>>Detail<<<<");
	printf("URL: %s\n", newurl);
	printf("DOMAIN: %s\n", domain);
	printf("IP: %s\n", ip_addr);
	printf("PORT: %d\n", port);
	printf("FILENAME: %s\n\n", file_name);

	char header[2048] = {0};
	sprintf(header, \
			"GET %s HTTP/1.1\r\n"\
			"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
			"User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
			"Host:%s\r\n"\
			"Connection:close\r\n"\
			"\r\n"\
			,newurl, domain);

	printf("%s\n%d\n", header, (int) strlen(header));

	int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket < 0)
	{
		printf("invalid socket descriptor: %d\n", client_socket);
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip_addr);
	addr.sin_port = htons(port);

	puts("3: Connect server...");
	int res = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
	if (res == -1)
	{
		printf("connect failed, return: %d\n", res);
		return -1;
	}

	puts("4: Send request...");
	lwip_write(client_socket, header, strlen(header));

	int mem_size = 4096;
	int length = 0;
	int len;
	char *buf = (char *) malloc(mem_size * sizeof(char));
	char *response = (char *) malloc(mem_size * sizeof(char));

	while ((len = lwip_read(client_socket, buf, 1)) != 0)
	{
		if (length + len > mem_size)
		{
			mem_size *= 2;
			char * temp = (char *) realloc(response, sizeof(char) * mem_size);
			if (temp == NULL)
			{
				printf("realloc failed\n");
				goto fail;
			}
			response = temp;
		}

		buf[len] = '\0';
		strcat(response, buf);

		int flag = 0;
		for (int i = strlen(response) - 1; response[i] == '\n' || response[i] == '\r'; i--, flag++);
		if (flag == 4)
			break;

		length += len;
	}

	printf("\n>>>>Response header:<<<<\n%s\n", response);

	resp = get_resp_header(response);
	strcpy(resp.file_name, file_name);

	printf("5: Start thread to download...\n");
	pthread_t download_thread;
	pthread_create(&download_thread, NULL, download, (void *) &client_socket);
	pthread_join(download_thread, NULL);

	if (buf)
		free(buf);
	return 0;
fail:
	if (buf)
		free(buf);
	return -1;

}
FINSH_FUNCTION_EXPORT_CMD(cmd_wgets, wgets, network wget single test);
#endif

int update_from_network(char* url)
{
	const char *newurl;
	char domain[64] = {0};
	char ip_addr[16] = {0};
	int port = 80;
	char file_name[256] = {0};

	puts("1: Parsing url...");
	parse_url(url, &newurl, domain, &port, file_name);

	puts("2: Get ip address...");
	get_ip_addr(domain, ip_addr);
	if (strlen(ip_addr) == 0)
	{
		printf("can not get ip address\n");
		return 0;
	}

	puts("\n>>>>Detail<<<<");
	printf("URL: %s\n", url);
	printf("DOMAIN: %s\n", domain);
	printf("IP: %s\n", ip_addr);
	printf("PORT: %d\n", port);
	printf("FILENAME: %s\n\n", file_name);

	char header[2048] = {0};
	sprintf(header, \
			"GET %s HTTP/1.1\r\n"\
			"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
			"User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537(KHTML, like Gecko) Chrome/47.0.2526Safari/537.36\r\n"\
			"Host:%s\r\n"\
			"Connection:close\r\n"\
			"\r\n"\
			,newurl, domain);

	printf("%s\n%d\n", header, (int) strlen(header));

	int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket < 0)
	{
		printf("invalid socket descriptor: %d\n", client_socket);
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip_addr);
	addr.sin_port = htons(port);

	puts("3: Connect server...");
	int res = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr));
	if (res == -1)
	{
		printf("connect failed, return: %d\n", res);
		return -1;
	}

	puts("4: Send request...");
	lwip_write(client_socket, header, strlen(header));

	int mem_size = 4096;
	int length = 0;
	int len;
	char *buf = (char *) malloc(mem_size * sizeof(char));
	char *response = (char *) malloc(mem_size * sizeof(char));

	while ((len = lwip_read(client_socket, buf, 1)) != 0)
	{
		if (length + len > mem_size)
		{
			mem_size *= 2;
			char * temp = (char *) realloc(response, sizeof(char) * mem_size);
			if (temp == NULL)
			{
				printf("realloc failed\n");
				goto fail;
			}
			response = temp;
		}

		buf[len] = '\0';
		strcat(response, buf);

		int flag = 0;
		for (int i = strlen(response) - 1; response[i] == '\n' || response[i] == '\r'; i--, flag++);
		if (flag == 4)
			break;

		length += len;
	}

	printf("\n>>>>Response header:<<<<\n%s\n", response);

	resp = get_resp_header(response);
	strcpy(resp.file_name, file_name);

	printf("5: Start thread to download...\n");
	pthread_t download_thread;
	pthread_create(&download_thread, NULL, download_rtos, (void *) &client_socket);
	pthread_join(download_thread, NULL);

	if (buf)
		free(buf);
	return 0;
fail:
	if (buf)
		free(buf);
	return -1;

}

int update_from_flash(char* path)
{
	int source;
	struct stat statbuff;
	char* source_file = path;
	char* source_buffer = NULL;
	int slice_size = 4*1024;
	int source_file_size, update_size, offset = 0, is_last_slice = 0, ret = 0;

	source = open(source_file, O_RDONLY);
	if (source < 0) {
		printf("open %s fail\n", source_file);
		ret = -1;
		goto update_from_flash_out;
	}

	if (stat(source_file, &statbuff) < 0) {
		printf("stat %s wrong\n", source_file);
		ret = -1;
		goto update_from_flash_out;
	} else {
		source_buffer = malloc(slice_size);
		if (source_buffer == NULL) {
			printf("malloc %d for source buffer fail\n", slice_size);
			ret = -1;
			goto update_from_flash_out;
		}
		source_file_size = statbuff.st_size;
		printf("source file size:%d slice_size:%d\n", source_file_size, slice_size);

		while (offset < source_file_size) {
		memset(source_buffer, 0, slice_size);
			update_size = min(source_file_size - offset, slice_size);
			ret = read(source, source_buffer, update_size);
			if (ret != update_size) {
				printf("read source file fail, now offset:%d ret:%d update_size:%d source_file_size:%d\n",
						offset, ret, update_size, source_file_size);
				ret = -1;
				goto update_from_flash_out;
			}

			if (offset + update_size >= source_file_size)
				is_last_slice = 1;
			else
				is_last_slice = 0;
			ret = aw_upgrade_slice("rtos", source_buffer, offset, update_size, is_last_slice);

			if (ret)
				goto update_from_flash_out;
			offset += update_size;
			progressBar(offset, source_file_size);
		}

		printf("last slice done, source_file_size:%d offset:%d ret:%d\n", source_file_size, offset, ret);
		aw_upgrade_end(0);
	}

update_from_flash_out:
	if (source >= 0) {
		close(source);
	}

	if (source_buffer) {
		free(source_buffer);
	}

	return ret;

}

int save_para(char* para)
{
	int fd, len;
	int ret = 0;
	if (access(OTA_CONFIG_FILE, F_OK) == 0) {
		remove(OTA_CONFIG_FILE);
	}
	fd = open(OTA_CONFIG_FILE, O_RDWR | O_CREAT);
	if (fd < 0) {
		printf("open %s fail\n", OTA_CONFIG_FILE);
		ret = -1;
		goto save_para_out;
	}
	len = strlen(para);
	if (len != write(fd, para, len)) {
		ret = -1;
		goto save_para_out;
	}

save_para_out:
	if (fd >= 0)
		close(fd);

	return ret;
}

int load_para(char* para)
{
	FILE *fp = NULL;
	int ret = 0;
	if (access(OTA_CONFIG_FILE, F_OK) == 0) {
		printf("detected %s\n", OTA_CONFIG_FILE);
		if((fp = fopen(OTA_CONFIG_FILE, "r")) == NULL) {
			printf("fopen error!\n");
			ret = -1;
			goto load_para_out;
		}
		fgets(para, MAX_OTA_PARA_SIZE ,fp);
		printf("para:%s\n", para);
	} else {
		printf("%s not exist\n", OTA_CONFIG_FILE);
		ret = -1;
		goto load_para_out;
	}

load_para_out:
	if (fp)
		fclose(fp);

	return ret;
}

int clear_para_after_end(void)
{
	int ret = 0;
	char *ota_result = NULL;

	if (fw_env_open())
		return -1;

	ota_result = fw_getenv("ota_result");
	if (ota_result) {
		printf("ota_result = %s\n", ota_result);
		if (access(OTA_CONFIG_FILE, F_OK) == 0) {
			printf("remove %s\n", OTA_CONFIG_FILE);
			remove(OTA_CONFIG_FILE);
		}
		ret = fw_env_write("ota_result", NULL);
		if (ret)
			goto clear_para_after_end_out;
		/* flush to flash */
		fw_env_flush();
	}
clear_para_after_end_out:
	fw_env_close();
	return ret;

}
int ota_task(char *para_in)
{
	int ret = 0;
	char *para = NULL, *para_malloc = NULL;
	if (para_in == NULL) {

		clear_para_after_end();

		para_malloc = malloc(MAX_OTA_PARA_SIZE);
		if (para_malloc == NULL) {
			printf("ota task malloc fail\n");
			ret = -1;
			goto ota_task_out;
		}
		ret = load_para(para_malloc);
		if (ret)
			goto ota_task_out;
		para = para_malloc;
		printf("load para success:%s\n", para);
	} else {
		para = para_in;
		save_para(para);
	}

	if (strncmp(para, "http", 4) == 0) {
		ret = update_from_network(para);
	} else if (strncmp(para, "/data", 5) == 0) {
		ret = update_from_flash(para);
	} else {
		printf("para not start with http or /data: %s\n", para);
		ret = -1;
	}

ota_task_out:
	if (para_malloc)
		free(para_malloc);

	return ret;
}

int cmd_awota(int argc, char **argv)
{
	int ret;
	if (argc == 1) {
		printf("Input a valid URL or path please\n");
		ret = -1;
	} else {
		ret = ota_task(argv[1]);
	}

	return ret ;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_awota, __cmd_awota, awota url/path);

int cmd_ota_task(int argc, char **argv)
{
	return ota_task(NULL);
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_ota_task, __cmd_ota_task, debug awota task);

