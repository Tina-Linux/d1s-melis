/*#####################################################################
# File Describe:telnet.c
# Author: flyranchaoflyranchao
# Created Time:flyranchao@allwinnertech.com
# Created Time:2020年05月19日 星期二 13时36分23秒
#====================================================================*/
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <errno.h>
#include <aw_dirent.h>
#include <unistd.h>
#include <finsh.h>
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
static CURL *curl;
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    DIR *dp;
    struct dirent *dirp;
    int fd, bytes, cnt;
    char dirs[128], dircmp[NAME_MAX], lbuf[2];
    static long socketdno;
    static char buflast;
    static int madewrite = 0;
    static int sockfdokay = 0;
    if (size * nmemb == 0)
	return (0);
    printf("%c", *(char *) buffer);
    if (*(char *) buffer == ' ' && buflast == ':') {
	if (!madewrite) {
	    madewrite = 1;
	    sprintf(dirs, "/proc/%d/fd", getpid());
	    dp = opendir(dirs);
	    dp = opendir(dirs);
	    if (dp) {
		dirp = (struct dirent *) 1;
		for (cnt = 0; dirp;) {
		    dirp = readdir(dp);
		    if (dirp) {
			cnt++;
			printf("dir is: %s cnt: %d\n", dirp->d_name, cnt);
			sprintf(dirs, "/proc/%d/fd/%s", getpid(),
				dirp->d_name);
			memset(dircmp, 0, NAME_MAX);
			//if (readlink(dirs, dircmp, NAME_MAX) != -1) {
			    printf("link points to: %s\n", dircmp);
			    if (!strncmp
				(dircmp, "socket:", strlen("socket:"))
				&& cnt == 6) {
				printf
				    ("found socket at expected position: %d link: %s\n",
				     cnt, dirp->d_name);
				sockfdokay = 1;
			    }
			//}
		    }
		}
		printf("fd dir count %d\n", cnt);
		closedir(dp);
	    }
	    if (cnt == 8 && sockfdokay == 1) {	//
		bytes = write(3, "CogcoG\r\r", strlen("CogcoG\r\r"));
		printf("wrote: %d bytes sock: %d\n", bytes, 3);
		bytes = 1;
		bytes = write(3, "who\r", strlen("who\r"));
		printf("wrote: %d bytes sock: %d\n", bytes, 3);
#if 0
		while (bytes > 0) {
		    bytes = read(3, lbuf, 1);
		    printf("lub:: %c ::\n", lbuf);
		}
#endif
		bytes = write(3, "quit\r", strlen("quit\r"));
		printf("wrote: %d bytes sock: %d\n", bytes, 3);
		bytes = write(3, "exit\r", strlen("exit\r"));
		printf("wrote: %d bytes sock: %d\n", bytes, 3);
	    }
	}
    }
    buflast = *(char *) buffer;
    /* printf("bytes in: %d\n",size*nmemb); */
    return (size * nmemb);
}

int cmd_telnet_test(int argc, char * const argv[])
{
    FILE *hd_src;
    int res;
    curl = curl_easy_init();
/* Get curl 7.7 from sunet.se's FTP site: */
    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
    curl_easy_setopt(curl, CURLOPT_USERPWD, "user:password");
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    res = curl_easy_perform(curl);
    printf("res is: %d\n", res);
/* always cleanup */
    curl_easy_cleanup(curl);
/* fclose(ftpfile); close the local file */
    return 0;
}

FINSH_FUNCTION_EXPORT(cmd_telnet_test, libcurl test for telnet);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_telnet_test, __cmd_telnet, libcurl test for telnet);
