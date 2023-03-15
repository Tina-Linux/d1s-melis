/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cmd_util.h"
#include "net_ctrl/net_ctrl.h"
#include "net/wlan/wlan_ext_req.h"

#if PRJCONF_NET_EN
static enum cmd_status cmd_wlan_set_pm_dtim(char *cmd)
{
	int ret, cnt;
	uint32_t period;

	cnt = cmd_sscanf(cmd, "p=%d", &period);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_PM_DTIM, period);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_ps_mode(char *cmd)
{
	int ret, cnt;
	uint32_t ps_mode = 0, ps_ip = 0, ps_cp = 0;
	wlan_ext_ps_cfg_t ps_cfg;

	if (cmd_strncmp(cmd, "enable", 6) == 0) {
		ps_mode = 1;
		cnt = cmd_sscanf((cmd + 6), " ip=%d cp=%d", &ps_ip, &ps_cp);
		if (cnt != 2) {
			ps_ip = 0;
			ps_cp = 0;
			CMD_ERR("cnt %d\n", cnt);
		}
	} else if (cmd_strncmp(cmd, "disable", 7) == 0) {
		ps_mode = 0;
	} else {
		CMD_ERR("invalid argument '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_memset(&ps_cfg, 0, sizeof(wlan_ext_ps_cfg_t));
	ps_cfg.ps_mode = ps_mode;
	ps_cfg.ps_idle_period = ps_ip;
	ps_cfg.ps_change_period = ps_cp;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_PS_CFG, (uint32_t)&ps_cfg);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_ampdu(char *cmd)
{
	int ret, cnt;
	int num;

	cnt = cmd_sscanf(cmd, "l=%d", &num);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_AMPDU_TXNUM, num);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_retry(char *cmd)
{
	int ret;
	int retry_cnt, cnt;

	cnt = cmd_sscanf(cmd, "n=%d", &retry_cnt);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_TX_RETRY_CNT, retry_cnt);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_pm_tx_null_period(char *cmd)
{
	int ret;
	int period, cnt;

	cnt = cmd_sscanf(cmd, "p=%d", &period);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_PM_TX_NULL_PERIOD, period);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_get_bcn_stat(char *cmd)
{
	int ret, i;
	unsigned int sum_cnt = 0;
	int sum_avg = 0;
	wlan_ext_bcn_status_t bcn_status;

	char dly_info[][20] = {
		"(<0      )",
		"(<500us  )",
		"(<1000us )",
		"(<2000us )",
		"(<4000us )",
		"(<8000us )",
		"(<16000us)",
		"(>16000us)",
	};

	cmd_memset(&bcn_status, 0, sizeof(wlan_ext_bcn_status_t));
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_BCN_STATUS, (uint32_t)&bcn_status);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	CMD_LOG(1, "\nAPP AP Beacon Delay Stat Info=================\n");
	for (i = 0; i < 8; i++) {
		CMD_LOG(1, "cnt %d %s: %d\n",
			i, dly_info[i],
			bcn_status.bcn_delay_cnt[i]);
		sum_cnt += bcn_status.bcn_delay_cnt[i];
	}

	if (sum_cnt)
		sum_avg = bcn_status.bcn_delay_sum / sum_cnt;

	CMD_LOG(1, "Dur: %d, Max: 0x%X, Rxed: %d, Missed: 0x%X\n",
		bcn_status.bcn_duration,
		bcn_status.bcn_delay_max,
		bcn_status.bcn_rx_cnt,
		bcn_status.bcn_miss_cnt);
	CMD_LOG(1, "Sum: %d, Cnt: %d, Ava: %d\n",
		bcn_status.bcn_delay_sum,
		sum_cnt,
		sum_avg);
	CMD_LOG(1, "APP AP Beacon Delay Stat Info=================\n");

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_bcn_win_us(char *cmd)
{
	int ret;
	int bcn_win, cnt;

	cnt = cmd_sscanf(cmd, "w=%d", &bcn_win);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_BCN_WIN_US, bcn_win);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_get_cur_tx_rate(char *cmd)
{
	int ret;
	int rate;

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_TX_RATE, (int)(&rate));
	CMD_LOG(1, "current rate:%d\n", rate);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_get_pm_dtim(char *cmd)
{
	int ret;
	wlan_ext_pm_dtim_t dtim;

	cmd_memset(&dtim, 0, sizeof(wlan_ext_pm_dtim_t));
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_PM_DTIM, (int)(&dtim));
	CMD_LOG(1, "AP DTIM set:%d, STA DTIM set:%d\n",
		dtim.pm_join_dtim_period, dtim.pm_dtim_period_extend);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_get_cur_signal(char *cmd)
{
	int ret;
	wlan_ext_signal_t signal;

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_SIGNAL, (int)(&signal));

	CMD_LOG(1, "current rssi:%d, noise:%d\n", signal.rssi, signal.noise);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_scan_param(char *cmd)
{
	int ret, cnt;
	int num_probes;
	int probe_delay;
	int min_dwell;
	int max_dwell;
	wlan_ext_scan_param_t param;

	cnt = cmd_sscanf(cmd, "n=%d d=%d min=%d max=%d",
		&num_probes, &probe_delay, &min_dwell, &max_dwell);
	if (cnt != 4) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	memset(&param, 0, sizeof(wlan_ext_scan_param_t));
	param.num_probes = num_probes;
	param.probe_delay = probe_delay;
	param.min_dwell = min_dwell;
	param.max_dwell = max_dwell;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SCAN_PARAM, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_listen_interval(char *cmd)
{
	int ret, cnt;
	uint32_t listen_interval;

	cnt = cmd_sscanf(cmd, "l=%d", &listen_interval);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_LISTEN_INTERVAL, listen_interval);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}


#define IPC_FUNC	0

#if IPC_FUNC
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include "netif/etharp.h"
static enum cmd_status cmd_wlan_set_auto_scan(char *cmd)
{

	int ret, cnt;
	int enable;
	int interval;
	wlan_ext_auto_scan_param_t param;

	if (cmd_strncmp(cmd, "enable", 6) == 0) {
		enable = 1;
		cnt = cmd_sscanf((cmd + 6), " i=%d", &interval);
		if (cnt != 1) {
			interval = 0;
			CMD_ERR("cnt %d\n", cnt);
		}
	} else if (cmd_strncmp(cmd, "disable", 7) == 0) {
		enable = 0;
		interval = 0;
	} else {
		CMD_ERR("invalid argument '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	memset(&param, 0, sizeof(wlan_ext_auto_scan_param_t));
	param.auto_scan_enable = enable;
	param.auto_scan_interval = interval;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_AUTO_SCAN, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_ACKED;
}

#define P2P_WAKEUP_PACKET_DATA	"for wakeup packet!"
#define P2P_KEEP_ALIVE_DATA		"for keeping alive!"
#define EXIT_DATA				"exit!"
#define MAX_DATA_SIZE			128

int ser_udp_flag = 0;
int sock_ser;
int sock_remote;
struct sockaddr_in remote_addr;
socklen_t addr_len = sizeof(remote_addr);
static enum cmd_status cmd_wlan_server_start(char *cmd)
{
	struct sockaddr_in local_addr;
	int port, cnt, ret;

	cnt = cmd_sscanf(cmd, "port=%d udp=%d", &port, &ser_udp_flag);
	if (cnt != 2) {
		CMD_ERR("cnt %d\n", cnt);
		//return CMD_STATUS_INVALID_ARG;
	}

	if (ser_udp_flag) {
		sock_ser = socket(AF_INET, SOCK_DGRAM, 0);
	} else {
		sock_ser = socket(AF_INET, SOCK_STREAM, 0);
	}

	int tmp = 1;
	ret = setsockopt(sock_ser, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(int));
	if (ret != 0) {
		CMD_ERR("setsockopt(SO_REUSEADDR) failed, err %d\n", (int)OS_GetErrno);
		closesocket(sock_ser);
		return -1;
	}

	/* bind socket to port */
	memset(&local_addr, 0, sizeof(struct sockaddr_in));
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	local_addr.sin_port = htons(port);
	local_addr.sin_family = AF_INET;
	ret = bind(sock_ser, (struct sockaddr *)&local_addr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		CMD_ERR("Failed to bind socket %d, err %d\n", sock_ser, (int)OS_GetErrno);
		closesocket(sock_ser);
		return CMD_STATUS_ACKED;
	}

	if (ser_udp_flag) {
		printf("UDP bind with port %d\n", port);
	} else {
		printf("TCP listen at port %d\n", port);
		ret = listen(sock_ser, 1);
		if (ret < 0) {
			CMD_ERR("Failed to listen socket %d, err %d\n", sock_ser, (int)OS_GetErrno);
			closesocket(sock_ser);
			return CMD_STATUS_ACKED;
		}

		ret = sizeof(struct sockaddr_in);
		while (1) {
			sock_remote = accept(sock_ser, (struct sockaddr *)&remote_addr,
								(socklen_t *)&ret);
			if (sock_remote >= 0) break;
		}
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_server_send(char *cmd)
{
	int cnt, ret, i;
	int send_cnt, period, len;
	uint8_t *data_buf = NULL;
	cnt = cmd_sscanf(cmd, "cnt=%d p=%d l=%d", &send_cnt, &period, &len);
	if (cnt != 3) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (len > MAX_DATA_SIZE) {
		CMD_ERR("invalid length %d, should be small than %d\n", len, MAX_DATA_SIZE);
		return CMD_STATUS_INVALID_ARG;
	}
	data_buf = cmd_malloc(len);
	for (i = 0;i < len;i++) {
		data_buf[i] = i;
	}

	//UDP remote_addr was set when receive before
	printf("%s send to %s:%d\n", ser_udp_flag?"UDP":"TCP", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
	for (i = 0;i < send_cnt;i++) {
		printf("cnt:%d send data %d bytes\n", i, len);
		if (ser_udp_flag) {
				ret = sendto(sock_ser, data_buf, len, 0, (struct sockaddr *)&remote_addr, addr_len);
		} else {
				ret = send(sock_remote, data_buf, len, 0);
		}
		if (ret <= 0) {
			printf("send return %d, err %d\n", ret, (int)OS_GetErrno);
			break;
		}
		XR_OS_MSleep(period);
	}
	cmd_free(data_buf);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_server_recv(char *cmd)
{
	int cnt, recv_cnt;
	uint32_t data_len;
	uint8_t *data_buf = NULL;

	cnt = cmd_sscanf(cmd, "cnt=%d", &recv_cnt);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	data_buf = cmd_malloc(MAX_DATA_SIZE);
	if (data_buf == NULL) {
		CMD_ERR("malloc() failed!\n");
		return CMD_STATUS_ACKED;
	}
	if (ser_udp_flag) {
		int i = 0;
		printf("UDP recv from %s:%d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
		while (1) {
			cmd_memset(data_buf, 0, MAX_DATA_SIZE);
			data_len = recvfrom(sock_ser, data_buf, MAX_DATA_SIZE, 0,
								(struct sockaddr *)&remote_addr, &addr_len);
			if (data_len > 0) {
				printf("cnt:%d recv data %d bytes\n", i, data_len);
				i++;
				if (i >= recv_cnt)
					break;
			} else {
				data_len = 0;
			}
		}
	} else {
		printf("TCP recv from %s:%d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
		for (int i = 0;i < recv_cnt;i++)
		{
			cmd_memset(data_buf, 0, MAX_DATA_SIZE);
			data_len = recv(sock_remote, data_buf, MAX_DATA_SIZE, 0);
			if (data_len) {
				printf("cnt:%d recv data %d bytes\n", i, data_len);
			}
		}
	}
	cmd_free(data_buf);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_server_stop(char *cmd)
{
	printf("close server socket!\n");
	closesocket(sock_ser);
	return CMD_STATUS_OK;
}

int cli_udp_flag = 0;
int sock_cli;
int ser_port;
uint32_t ser_ip[4];
struct sockaddr_in ser_addr;
socklen_t ser_addr_len = sizeof(ser_addr);
static enum cmd_status cmd_wlan_client_start(char *cmd)
{
	int cnt, ret;
	uint8_t str_ip_addr[100];

	cnt = cmd_sscanf(cmd, "ser=%d.%d.%d.%d port=%d udp=%d",
		&ser_ip[0], &ser_ip[1], &ser_ip[2], &ser_ip[3], &ser_port, &cli_udp_flag);
	if (cnt != 6) {
		CMD_ERR("cnt %d\n", cnt);
		//return CMD_STATUS_INVALID_ARG;
	}

	memset(str_ip_addr, 0, 100);
	sprintf((char *)str_ip_addr, "%d.%d.%d.%d",	ser_ip[0], ser_ip[1], ser_ip[2], ser_ip[3]);

	if (cli_udp_flag) {
		sock_cli = socket(AF_INET, SOCK_DGRAM, 0);
	} else {
		sock_cli = socket(AF_INET, SOCK_STREAM, 0);
	}

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr((char *)str_ip_addr);
	ser_addr.sin_port = htons(ser_port);
	if (!cli_udp_flag) {
		printf("try to connect %s:%d...\n", str_ip_addr, ser_port);
		ret = connect(sock_cli, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr));
		if(ret < 0)
			printf("Connect failed! ret = %d\n", ret);
		else {
			printf("Connect OK!\n");
		}
	}
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_client_send(char *cmd)
{
	int cnt, ret, i;
	int send_cnt, period, len;
	uint8_t *data_buf = NULL;
	cnt = cmd_sscanf(cmd, "cnt=%d p=%d l=%d", &send_cnt, &period, &len);
	if (cnt != 3) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (len > MAX_DATA_SIZE) {
		CMD_ERR("invalid length %d, should be small than %d\n", len, MAX_DATA_SIZE);
		return CMD_STATUS_INVALID_ARG;
	}
	data_buf = cmd_malloc(len);
	for (i = 0;i < len;i++) {
		data_buf[i] = i;
	}

	//ser_addr was set when client start
	printf("%s send to %s:%d\n", cli_udp_flag?"UDP":"TCP", inet_ntoa(ser_addr.sin_addr), ntohs(ser_addr.sin_port));
	for (i = 0;i < send_cnt;i++) {
		printf("cnt:%d send data %d bytes\n", i, len);
		if (cli_udp_flag) {
			ret = sendto(sock_cli, data_buf, len, 0, (struct sockaddr *)&ser_addr, ser_addr_len);
		} else {
			ret = send(sock_cli, data_buf, len, 0);
		}
		if (ret <= 0) {
			printf("send return %d, err %d\n", ret, (int)OS_GetErrno);
			break;
		}
		XR_OS_MSleep(period);
	}
	cmd_free(data_buf);

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_client_recv(char *cmd)
{
	int cnt, recv_cnt;
	uint32_t data_len;
	uint8_t *data_buf = NULL;

	cnt = cmd_sscanf(cmd, "cnt=%d", &recv_cnt);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	data_buf = malloc(MAX_DATA_SIZE);
	if (data_buf == NULL) {
		CMD_ERR("malloc() failed!\n");
		return CMD_STATUS_ACKED;
	}
	if (cli_udp_flag) {
		int i = 0;
		printf("UDP recv from %s:%d\n", inet_ntoa(ser_addr.sin_addr), ntohs(ser_addr.sin_port));
		while (1) {
			cmd_memset(data_buf, 0, MAX_DATA_SIZE);
			data_len = recvfrom(sock_cli, data_buf, MAX_DATA_SIZE, 0,
								(struct sockaddr *)&ser_addr, &ser_addr_len);
			if (data_len > 0) {
				printf("cnt:%d recv data %d bytes\n", i, data_len);
				i++;
				if (i >= recv_cnt)
					break;
			} else {
				data_len = 0;
			}
		}
	} else {
		printf("TCP recv from %s:%d\n", inet_ntoa(ser_addr.sin_addr), ntohs(ser_addr.sin_port));
		for (int i = 0;i < recv_cnt;i++)
		{
			cmd_memset(data_buf, 0, MAX_DATA_SIZE);
			data_len = recv(sock_cli, data_buf, MAX_DATA_SIZE, 0);
			if (data_len) {
				printf("cnt:%d recv data %d bytes\n", i, data_len);
			}
		}
	}
	cmd_free(data_buf);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_client_stop(char *cmd)
{
	//send(sock_cli_tcp, EXIT_DATA, strlen(EXIT_DATA), 0);
	printf("close client socket!\n");
	closesocket(sock_cli);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_p2p_server(char *cmd)
{
	int ret, cnt, i;
	int mac[6];
	int ser_num, enable;
	int udp_flag = 0;
	wlan_ext_p2p_svr_set_t svr;
	struct sockaddr_in local_addr;
	socklen_t len = sizeof(struct sockaddr);

	cnt = cmd_sscanf(cmd, "s=%d e=%d udp=%d mac=%x:%x:%x:%x:%x:%x",
		&ser_num, &enable, &udp_flag,
		&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
	if (cnt != 9) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	//set server param
	if (ser_num > 2) {
		CMD_ERR("invalid server number %d, should be 0 to 2\n", ser_num);
		return CMD_STATUS_INVALID_ARG;
	}
	cmd_memset(&svr, 0, sizeof(wlan_ext_p2p_svr_set_t));
	cmd_memcpy(svr.SrcIPv4Addr, &(g_wlan_netif->ip_addr.addr), 4);
	getsockname(sock_cli, (struct sockaddr *)&local_addr, &len);
	svr.P2PServerCfgs[ser_num].DstPort = ser_port;
	svr.P2PServerCfgs[ser_num].SrcPort = ntohs(local_addr.sin_port);
	for (i = 0;i < 4;i++) {
		svr.P2PServerCfgs[ser_num].DstIPv4Addr[i] = ser_ip[i];
	}
	for (i = 0;i < 6;i++) {
		svr.P2PServerCfgs[ser_num].DstMacAddr[i] = mac[i];
	}
	svr.P2PServerCfgs[ser_num].TcpSeqInit = getsockack(sock_cli);
	svr.P2PServerCfgs[ser_num].TcpAckInit = getsockseq(sock_cli);
	svr.P2PServerCfgs[ser_num].IPIdInit = 1;
	svr.P2PServerCfgs[ser_num].Enable = enable;
	svr.P2PServerCfgs[ser_num].TcpOrUdp = udp_flag?0x02:0x01;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_P2P_SVR, (int)(&svr));


	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_p2p_wkpacket(char *cmd)
{
	int ret, cnt, i;
	int ser_num, enable, payload_len;
	wlan_ext_p2p_wkp_param_set_t wkp_param;

	cnt = cmd_sscanf(cmd, "s=%d e=%d pl=%d", &ser_num, &enable, &payload_len);
	if (cnt != 3) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	//set wakeup packet param
	if (ser_num > 2) {
		CMD_ERR("invalid server number %d, should be 0 to 2\n", ser_num);
		return CMD_STATUS_INVALID_ARG;
	}
	cmd_memset(&wkp_param, 0, sizeof(wlan_ext_p2p_wkp_param_set_t));
	wkp_param.P2PWkpParamCfgs[ser_num].Enable = enable;
	wkp_param.P2PWkpParamCfgs[ser_num].PayloadLen = payload_len;
	for (i = 0; i < payload_len; i++) {
		wkp_param.P2PWkpParamCfgs[ser_num].Payload[i] = i;
	}
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_P2P_WKP_CFG, (int)(&wkp_param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_p2p_wakeupip(char *cmd)
{
	int ret, cnt, i;
	int ser_num, enable, ip[4];
	wlan_ext_p2p_wkp_param_set_t wkp_param;

	cnt = cmd_sscanf(cmd, "s=%d e=%d ser=%d.%d.%d.%d", &ser_num, &enable, &ip[0], &ip[1], &ip[2], &ip[3]);
	if (cnt != 6) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	//set wakeup packet param
	if (ser_num > 7) {
		CMD_ERR("invalid server number %d, should be 0 to 7\n", ser_num);
		return CMD_STATUS_INVALID_ARG;
	}
	cmd_memset(&wkp_param, 0, sizeof(wlan_ext_p2p_wkp_param_set_t));
	wkp_param.Enable = enable;
	for (i = 0; i < 4; i++) {
		wkp_param.P2PIpv4FilterCfgs[ser_num].Ipv4Filter[i] = ip[i];
	}
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_P2P_WKP_CFG, (int)(&wkp_param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_p2p_kpalive(char *cmd)
{
	int ret, cnt, i;
	int ser_num, enable;
	int period, timeout, retry, payload_len;
	wlan_ext_p2p_kpalive_param_set_t param;

	cnt = cmd_sscanf(cmd, "s=%d e=%d p=%d t=%d r=%d pl=%d",
		&ser_num, &enable, &period, &timeout, &retry, &payload_len);
	if (cnt != 6) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	//set keep alive param
	if (ser_num > 2) {
		CMD_ERR("invalid server number %d, should be 0 to 2\n", ser_num);
		return CMD_STATUS_INVALID_ARG;
	}
	cmd_memset(&param, 0, sizeof(wlan_ext_p2p_kpalive_param_set_t));
	param.KeepAlivePeriod_s = period;
	param.TxTimeOut_s = timeout;
	param.TxRetryLimit = retry;
	param.P2PKeepAliveParamCfgs[ser_num].Enable = enable;
	param.P2PKeepAliveParamCfgs[ser_num].PayloadLen = payload_len;
	for (i = 0;i < payload_len;i++) {
		param.P2PKeepAliveParamCfgs[ser_num].Payload[i] = i;
	}
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_P2P_KPALIVE_CFG, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_p2p_hostsleep(char *cmd)
{
	int ret, cnt;
	int hostsleep;

	cnt = cmd_sscanf(cmd, "%d", &hostsleep);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

    ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_P2P_HOST_SLEEP, hostsleep);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}
#endif

#define DNA_WLAN	1
#if DNA_WLAN
#include "frm_construct.h"
static enum cmd_status cmd_wlan_set_rcv_special_frm(char *cmd)
{
	int ret;
	uint32_t enable, type;
	wlan_ext_rcv_spec_frm_param_set_t param;

	int cnt;
	cnt = cmd_sscanf(cmd, "e=%d t=0x%x", &enable, &type);
	if (cnt != 2) {
		CMD_ERR("cnt %d\n", cnt);
	}
	memset(&param, 0, sizeof(wlan_ext_rcv_spec_frm_param_set_t));
	param.Enable = enable;
	param.u32RecvSpecFrameCfg = type;

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_RCV_SPECIAL_FRM, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_send_raw_frm_cfg(char *cmd)
{
	int ret, cnt;
	uint32_t cfg, enable;
	wlan_ext_send_raw_frm_param_set_t param;

	cnt = cmd_sscanf(cmd, "e=%d c=0x%x", &enable, &cfg);
	if (cnt != 2) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	memset(&param, 0, sizeof(wlan_ext_send_raw_frm_param_set_t));
	param.Enable = enable;
	param.u16SendRawFrameCfg = cfg;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SEND_RAW_FRM_CFG, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_switch_channel_cfg(char *cmd)
{
	int ret, cnt;
	uint32_t band, flag, channel, sc_time;
	wlan_ext_switch_channel_param_set_t param;

	cnt = cmd_sscanf(cmd, "b=%d f=0x%x c=%d t=%d",
		&band, &flag, &channel, &sc_time);
	if (cnt != 4) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	memset(&param, 0, sizeof(wlan_ext_switch_channel_param_set_t));
	param.Enable = 1;
	param.Band = band;
	param.Flag = flag;
	param.ChannelNum = channel;
	param.SwitchChannelTime = sc_time;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SWITCH_CHN_CFG, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_get_cur_channel(char *cmd)
{
	int ret;
	uint32_t channel;

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_CURRENT_CHN, (int)(&channel));
	printf("current channel is %d\n", channel);
	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_sniff_kp_active(char *cmd)
{
	int ret, cnt;
	uint32_t enable, cfg;
	wlan_ext_sniff_kp_active_set_t param;

	cnt = cmd_sscanf(cmd, "e=%d c=0x%x", &enable, &cfg);
	if (cnt != 2) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	memset(&param, 0, sizeof(wlan_ext_sniff_kp_active_set_t));
	param.Enable = enable;
	param.u32Config = cfg;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SNIFF_KP_ACTIVE, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_sniff_auto_wakeup_host(char *cmd)
{
	int ret, cnt;
	int cfg, type, enable, wkp_time, kp_time, flag, start_time, channel;
	wlan_ext_sniff_sync_param_set_t param;

	cnt = cmd_sscanf(cmd, "cfg=0x%x t=0x%x e=%d c=%d wt=%d kt=%d f=0x%x st=%d",
		&cfg, &type, &enable, &channel, &wkp_time, &kp_time, &flag, &start_time);
	if (cnt != 8) {
		printf("cnt %d\n", cnt);
		//return CMD_STATUS_INVALID_ARG;
	}
	param.Enable = enable;
	param.ChannelNum = channel;
	param.SyncFrameType = type;//data
	param.u32SniffSyncCfg = cfg;// | SNIFF_SYNC_DISABLE_TIMER;
	param.time_sync_at_host.WakeupPeriod_ms = wkp_time;//ms
	param.time_sync_at_host.KeepActivePeriod_ms = kp_time;//ms
	param.time_sync_at_host.Flag = flag;//SNIFF_AUTO_WAKEUP_FRAME_SEND_TO_HOST;
	param.time_sync_at_host.StartTime = start_time;//us
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SNIFF_SYNC_CFG, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_sniff_auto_wakeup_wifi(char *cmd)
{
	int ret, cnt;
	int cfg, type, channel, flag, SyncDTIM, MaxLostSyncPacket;
	int TSFOffset, AdaptiveExpansion, KeepActiveNumAfterLostSync, ActiveTime, MaxAdaptiveExpansionLimit;
	int MaxKeepAliveTime;
	wlan_ext_sniff_sync_param_set_t param;

	cnt = cmd_sscanf(cmd, "cfg=0x%x t=0x%x c=%d f=0x%x d=%d ml=%d tsf=%d ae=%d kaa=%d at=%d ma=%d mk=%d",
		&cfg, &type, &channel, &flag, &SyncDTIM, &MaxLostSyncPacket,	&TSFOffset,
		&AdaptiveExpansion, &KeepActiveNumAfterLostSync, &ActiveTime, &MaxAdaptiveExpansionLimit,
		&MaxKeepAliveTime);
	if (cnt != 12) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}
	param.Enable = 1;
	param.ChannelNum = channel;
	param.SyncFrameType = type;
	param.u32SniffSyncCfg = cfg;//s

	param.time_sync_at_wifi.Flag = flag;
	param.time_sync_at_wifi.SyncDTIM = SyncDTIM;
	param.time_sync_at_wifi.MaxLostSyncPacket = MaxLostSyncPacket;
	param.time_sync_at_wifi.TSFOffset = TSFOffset;
	param.time_sync_at_wifi.AdaptiveExpansion = AdaptiveExpansion;
	param.time_sync_at_wifi.KeepActiveNumAfterLostSync = KeepActiveNumAfterLostSync;
	param.time_sync_at_wifi.ActiveTime = ActiveTime;
	param.time_sync_at_wifi.MaxAdaptiveExpansionLimit = MaxAdaptiveExpansionLimit;
	param.time_sync_at_wifi.MaxKeepAliveTime = MaxKeepAliveTime;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SNIFF_SYNC_CFG, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static wlan_ext_frm_filter_set_t g_frm_filter;
static enum cmd_status cmd_wlan_rcv_frm_filter_mac(char *cmd)
{
	int ret = 0, cnt, i, filter;

	uint32_t A1[6];
	uint32_t A2[6];
	uint32_t A3[6];

	cnt = cmd_sscanf(cmd, "f=%d a1=%x:%x:%x:%x:%x:%x a2=%x:%x:%x:%x:%x:%x a3=%x:%x:%x:%x:%x:%x",
		&filter,
		&A1[0], &A1[1], &A1[2], &A1[3], &A1[4], &A1[5],
		&A2[0], &A2[1], &A2[2], &A2[3], &A2[4], &A2[5],
		&A3[0], &A3[1], &A3[2], &A3[3], &A3[4], &A3[5]);
	if (cnt != 19) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (filter == 1) {
		for (i = 0; i < 6; i++) {
			g_frm_filter.Filter1.MacAddrA1[i] = (uint8_t)A1[i];
			g_frm_filter.Filter1.MacAddrA2[i] = (uint8_t)A2[i];
			g_frm_filter.Filter1.MacAddrA3[i] = (uint8_t)A3[i];
		}
	} else if (filter == 2) {
		for (i = 0; i < 6; i++) {
			g_frm_filter.Filter2.MacAddrA1[i] = (uint8_t)A1[i];
			g_frm_filter.Filter2.MacAddrA2[i] = (uint8_t)A2[i];
			g_frm_filter.Filter2.MacAddrA3[i] = (uint8_t)A3[i];
		}
	} else {
		CMD_ERR("%s: invalid filter type:%d\n", __func__, filter);
		return CMD_STATUS_ACKED;
	}

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_frm_filter(char *cmd)
{
	int ret;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_FRM_FILTER, (int)(&g_frm_filter));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_rcv_frm_filter1_ie_cfg(char *cmd)
{
	int ret = 0, cnt, i;
	rcv_frm_filter_t * param;
	param = &(g_frm_filter.Filter1);

	int enable, filter, and_mask, or_mask, frm_type, mac_mask, ie_id, ie_len;
	uint32_t oui[10];

	cnt = cmd_sscanf(cmd, "e=%d f=0x%x am=0x%x om=0x%x t=0x%x mm=0x%x ii=%d il=%d oui=%x %x %x %x %x %x %x %x %x %x ",
		&enable, &filter, &and_mask, &or_mask, &frm_type, &mac_mask, &ie_id, &ie_len,
		&oui[0], &oui[1], &oui[2], &oui[3], &oui[4],
		&oui[5], &oui[6], &oui[7], &oui[8], &oui[9]);
	if (cnt != 18) {
		printf("cnt %d\n", cnt);
		if ((cnt == 6) || (cnt == 1)) {
			ie_len = 0;
		} else {
			return CMD_STATUS_INVALID_ARG;
		}
	}

	if (enable == 0) {
		g_frm_filter.Filter1Cfg = 0;
		cmd_memset(param, 0, sizeof(rcv_frm_filter_t));
		return CMD_STATUS_OK;
	}

	g_frm_filter.Filter1Cfg = 1;
	param->FilterEnable = filter;
	param->AndOperationMask = and_mask;
	param->OrOperationMask = or_mask;
	param->FrameType = frm_type;
	param->MacAddrMask = mac_mask;
	param->IeCfg.ElementId = ie_id;
	param->IeCfg.Length = ie_len;
	if (cnt >= 8) {
	//CMD_WRN("IE Id:%d, Len:%d\n", ie_id, ie_len);
		for (i = 0; i < ie_len; i++) {
			param->IeCfg.OUI[i] = (uint8_t)oui[i];
			//printf("OUI: num:%d origin::%x, set:%x\n", i, oui[i], param->IeCfg.OUI[i]);
		}
	}
	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_rcv_frm_filter2_ie_cfg(char *cmd)
{
	int ret = 0, cnt, i;
	rcv_frm_filter_t * param;
	param = &(g_frm_filter.Filter2);

	int enable, filter, and_mask, or_mask, frm_type, mac_mask, ie_id, ie_len;
	uint32_t oui[10];

	cnt = cmd_sscanf(cmd, "e=%d f=0x%x am=0x%x om=0x%x t=0x%x mm=0x%x ii=%d il=%d oui=%x %x %x %x %x %x %x %x %x %x ",
		&enable, &filter, &and_mask, &or_mask, &frm_type, &mac_mask, &ie_id, &ie_len,
		&oui[0], &oui[1], &oui[2], &oui[3], &oui[4],
		&oui[5], &oui[6], &oui[7], &oui[8], &oui[9]);
	if (cnt != 18) {
		printf("cnt %d\n", cnt);
		//return CMD_STATUS_INVALID_ARG;
	}

	if (enable == 0) {
		g_frm_filter.Filter2Cfg = 0;
		cmd_memset(param, 0, sizeof(rcv_frm_filter_t));
		return CMD_STATUS_OK;
	}

	g_frm_filter.Filter2Cfg = 1;
	param->FilterEnable = filter;
	param->AndOperationMask = and_mask;
	param->OrOperationMask = or_mask;
	param->FrameType = frm_type;
	param->MacAddrMask = mac_mask;
	param->IeCfg.ElementId = ie_id;
	param->IeCfg.Length = ie_len;
	for (i = 0; i < ie_len; i++)
		param->IeCfg.OUI[i] = (uint8_t)oui[i];

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_rcv_frm_filter1_pl_cfg(char *cmd)
{
	int ret=0, cnt, i;
	rcv_frm_filter_t * param;
	param = &(g_frm_filter.Filter1);

	int enable, filter, and_mask, or_mask, frm_type, mac_mask, pl_offset, pl_len;
	uint32_t payload[10];

	cnt = cmd_sscanf(cmd, "e=%d f=0x%x am=0x%x om=0x%x t=0x%x mm=0x%x po=%d pl=%d p=%x %x %x %x %x %x %x %x %x %x ",
		&enable, &filter, &and_mask, &or_mask, &frm_type, &mac_mask, &pl_offset, &pl_len,
		&payload[0], &payload[1], &payload[2], &payload[3], &payload[4],
		&payload[5], &payload[6], &payload[7], &payload[8], &payload[9]);
	if (cnt != 18) {
		printf("cnt %d\n", cnt);
		//return CMD_STATUS_INVALID_ARG;
	}

	if (enable == 0) {
		g_frm_filter.Filter1Cfg = 0;
		cmd_memset(param, 0, sizeof(rcv_frm_filter_t));
		return CMD_STATUS_ACKED;
	}

	g_frm_filter.Filter1Cfg = 1;
	param->FilterEnable = filter;
	param->AndOperationMask = and_mask;
	param->OrOperationMask = or_mask;
	param->FrameType = frm_type;
	param->MacAddrMask = mac_mask;
	param->PayloadCfg.PayloadOffset = pl_offset;
	param->PayloadCfg.PayloadLength = pl_len;
	for (i = 0; i < pl_len; i++)
		param->PayloadCfg.Payload[i] = (uint8_t)payload[i];

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_rcv_frm_filter2_pl_cfg(char *cmd)
{
	int ret=0, cnt, i;
	rcv_frm_filter_t * param;
	param = &(g_frm_filter.Filter2);

	int enable, filter, and_mask, or_mask, frm_type, mac_mask, pl_offset, pl_len;
	uint32_t payload[10];

	cnt = cmd_sscanf(cmd, "e=%d f=0x%x am=0x%x om=0x%x t=0x%x mm=0x%x po=%d pl=%d p=%x %x %x %x %x %x %x %x %x %x ",
		&enable, &filter, &and_mask, &or_mask, &frm_type, &mac_mask, &pl_offset, &pl_len,
		&payload[0], &payload[1], &payload[2], &payload[3], &payload[4],
		&payload[5], &payload[6], &payload[7], &payload[8], &payload[9]);
	if (cnt != 18) {
		printf("cnt %d\n", cnt);
		//return CMD_STATUS_INVALID_ARG;
	}

	if (enable == 0) {
		g_frm_filter.Filter2Cfg = 0;
		cmd_memset(param, 0, sizeof(rcv_frm_filter_t));
		return CMD_STATUS_ACKED;
	}

	g_frm_filter.Filter2Cfg = 1;
	param->FilterEnable = filter;
	param->AndOperationMask = and_mask;
	param->OrOperationMask = or_mask;
	param->FrameType = frm_type;
	param->MacAddrMask = mac_mask;
	param->PayloadCfg.PayloadOffset = pl_offset;
	param->PayloadCfg.PayloadLength = pl_len;
	for (i = 0; i < pl_len; i++)
		param->PayloadCfg.Payload[i] = (uint8_t)payload[i];

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}


#define FRM_TYPE_ASRQ       (0)
#define FRM_TYPE_ASRSP      (1)
#define FRM_TYPE_RSRQ       (2)
#define FRM_TYPE_RSRSP      (3)
#define FRM_TYPE_PBRQ       (4)
#define FRM_TYPE_PBRSP      (5)
#define FRM_TYPE_BCN        (6)
#define FRM_TYPE_ATIM       (7)
#define FRM_TYPE_DAS        (8)
#define FRM_TYPE_AUTH       (9)
#define FRM_TYPE_DAUTH      (10)
#define FRM_TYPE_ACTION     (11)
#define FRM_TYPE_NULLDATA   (12)
#define FRM_TYPE_DATA       (13)
#define FRM_TYPE_QOSDATA    (14)
#define FRM_TYPE_MANAGER    (15)
#define FRM_TYPE_ALL_DATA   (16)
#define FRM_TYPE_ARPREQ     (17)
#define FRM_TYPE_MAX        (18)
wlan_ext_temp_frm_set_t frame;
extern uint8_t dest_mac[6];
extern uint8_t src_mac[6];
void construct_frame(int frm_type)
{
	switch (frm_type)
	{
	case FRM_TYPE_ASRQ:
		frame.FrmLength = assocreq_construct((uint8_t *)frame.Frame);
		printf("send raw frame associate req (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_ASRSP:
		frame.FrmLength = assocrsp_construct((uint8_t *)frame.Frame);
		printf("send raw frame associate rsp (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_RSRQ:
		frame.FrmLength = reassocreq_construct((uint8_t *)frame.Frame);
		printf("send raw frame reassociate req (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_RSRSP:
		frame.FrmLength = reassocrsp_construct((uint8_t *)frame.Frame);
		printf("send raw frame reassociate rsp (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_PBRQ:
		frame.FrmLength = probereq_construct((uint8_t *)frame.Frame);
		printf("send raw frame probe req (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_PBRSP:
		frame.FrmLength = probersp_construct((uint8_t *)frame.Frame);
		printf("send raw frame probe rsp (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_BCN:
		frame.FrmLength = beacon_construct((uint8_t *)frame.Frame);
		printf("send raw frame beacon (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_ATIM:
		frame.FrmLength = atim_construct((uint8_t *)frame.Frame);
		printf("send raw frame atim (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_DAS:
		frame.FrmLength = disassocreq_construct((uint8_t *)frame.Frame);
		printf("send raw frame disassociate req (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_AUTH:
		frame.FrmLength = auth_construct((uint8_t *)frame.Frame);
		printf("send raw frame auth (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_DAUTH:
		frame.FrmLength = deauth_construct((uint8_t *)frame.Frame);
		printf("send raw frame deauth (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_ACTION:
		frame.FrmLength = action_construct((uint8_t *)frame.Frame);
		printf("send raw frame action (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_NULLDATA:
		frame.FrmLength = nulldata_construct((uint8_t *)frame.Frame);
		printf("send raw frame nulldata (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_DATA:
		frame.FrmLength = data_construct((uint8_t *)frame.Frame);
		printf("send raw frame data (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_QOSDATA:
		frame.FrmLength = qosdata_construct((uint8_t *)frame.Frame);
		printf("send raw frame qosdata (%d)\n", frame.FrmLength);
		break;
	case FRM_TYPE_ARPREQ:
		frame.FrmLength = arpreq_construct((uint8_t *)frame.Frame);
		printf("send raw frame arp req (%d)\n", frame.FrmLength);
		break;
	}
}

static enum cmd_status cmd_wlan_set_temp_frm(char *cmd)
{
	int ret, cnt, frm_type;
	cnt = cmd_sscanf(cmd, "t=%d", &frm_type);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}
	construct_frame(frm_type);
	if (frm_type != FRM_TYPE_BCN && frm_type != FRM_TYPE_PBRSP) {
		printf("force add TSF\n");
		for (int i = 0;i < frame.FrmLength - 24;i++) {
			frame.Frame[frame.FrmLength - 1 - i + 10] = frame.Frame[frame.FrmLength - 1 - i];
		}
		frame.Frame[32] = 0x64;
		frame.Frame[33] = 0;
		frame.FrmLength += 10;
	}
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_TEMP_FRM, (int)(&frame));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_send_sync_frm(char *cmd)
{
	int ret, cnt;
	uint32_t bcn_intvl, enable;
	wlan_ext_send_sync_frm_set_t param;

	cnt = cmd_sscanf(cmd, "e=%d b=%d", &enable,
					&bcn_intvl);
	if (cnt != 2) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	memset(&param, 0, sizeof(wlan_ext_send_sync_frm_set_t));
	param.Enable = enable;
	param.BcnInterval = bcn_intvl;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SYNC_FRM_SEND, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_send_raw_frm(char *cmd)
{
	int ret = 0, i, j, cnt, ext_ch, cur_ch, tmp_ch;
	int frm_cnt, frm_period, frm_type;
	uint32_t mac[6];
	cnt = cmd_sscanf(cmd, "c=%d p=%d t=%d ec=%d m=%x:%x:%x:%x:%x:%x",
					&frm_cnt, &frm_period, &frm_type, &ext_ch,
					&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
	if (cnt != 10) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}
	for (i = 0; i < 6; i++)
		dest_mac[i] = (uint8_t)mac[i];

	//wlan_get_mac_addr(g_wlan_netif, src_mac, 6);

	switch (frm_type)
	{
	case FRM_TYPE_MANAGER:
		printf("send all manager frame\n");
		cnt = 0;
		for (i = 0;i < frm_cnt;i++) {
			for (j = 0; j < 6; j++)
				dest_mac[j] = (uint8_t)mac[j];
			for (j = 0; j < 12; j++) {
				cmd_memset((uint8_t *)frame.Frame, 0, MAX_FRM_LEN);
				construct_frame(j);
				wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
				XR_OS_MSleep(frm_period);
				printf("******************************** cnt:%d ********************************\n", cnt++);
			}
		}
		return CMD_STATUS_OK;
	case FRM_TYPE_ALL_DATA:
		printf("send all data frame\n");
		cnt = 0;
		for (i = 0;i < frm_cnt;i++) {
			for (j = 0; j < 6; j++)
				dest_mac[j] = (uint8_t)mac[j];
			for (j = 12; j < 15; j++) {
				cmd_memset((uint8_t *)frame.Frame, 0, MAX_FRM_LEN);
				construct_frame(j);
				wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
				XR_OS_MSleep(frm_period);
				printf("******************************** cnt:%d ********************************\n", cnt++);
			}
			//send broadcast data frame
			cmd_memset(dest_mac, 0xff, 6);
			cmd_memset((uint8_t *)frame.Frame, 0, MAX_FRM_LEN);
			construct_frame(FRM_TYPE_DATA);
			wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
			XR_OS_MSleep(frm_period);
			printf("******************************** cnt:%d ********************************\n", cnt++);
		}
		return CMD_STATUS_OK;
	case FRM_TYPE_MAX:
		{
			printf("send all frame\n");
			wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_CURRENT_CHN, (int)(&cur_ch));
			wlan_ext_switch_channel_param_set_t param1;
			memset(&param1, 0, sizeof(wlan_ext_switch_channel_param_set_t));
			param1.Enable = 1;
			param1.SwitchChannelTime = 5000;//SEND_RAW_FRAME_MAX_SWITCH_CHANNEL_TIME;
			cnt = 0;
			for (i = 0;i < frm_cnt;i++) {
				for (j = 0; j < 6; j++)
					dest_mac[j] = (uint8_t)mac[j];
				for (j = 0; j < 15; j++) {
					cmd_memset((uint8_t *)frame.Frame, 0, MAX_FRM_LEN);
					construct_frame(j);
					wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
					XR_OS_MSleep(frm_period);
					if (ext_ch) {
						param1.ChannelNum = ext_ch;
						wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SWITCH_CHN_CFG, (int)(&param1));
						do {
							XR_OS_MSleep(10);
							wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_CURRENT_CHN, (int)(&tmp_ch));
						} while (tmp_ch != ext_ch);
						printf("switch to channel %d\n", ext_ch);
						wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
						XR_OS_MSleep(100);
						param1.ChannelNum = cur_ch;
						wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SWITCH_CHN_CFG, (int)(&param1));
						do {
							XR_OS_MSleep(5);
							wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_CURRENT_CHN, (int)(&tmp_ch));
						} while (tmp_ch != cur_ch);
						printf("switch to channel %d\n", cur_ch);
						XR_OS_MSleep(frm_period);
					}
					printf("******************************** cnt:%d ********************************\n", cnt++);
				}
				cmd_memset(dest_mac, 0xff, 6);
				cmd_memset((uint8_t *)frame.Frame, 0, MAX_FRM_LEN);
				construct_frame(FRM_TYPE_DATA);
				wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
				XR_OS_MSleep(frm_period);
				if (ext_ch) {
					param1.ChannelNum = ext_ch;
					wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SWITCH_CHN_CFG, (int)(&param1));
					do {
						XR_OS_MSleep(10);
						wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_CURRENT_CHN, (int)(&tmp_ch));
					} while (tmp_ch != ext_ch);
					printf("switch to channel %d\n", ext_ch);
					wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
					XR_OS_MSleep(100);
					param1.ChannelNum = cur_ch;
					wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SWITCH_CHN_CFG, (int)(&param1));
					do {
						XR_OS_MSleep(5);
						wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_CURRENT_CHN, (int)(&tmp_ch));
					} while (tmp_ch != cur_ch);
					printf("switch to channel %d\n", cur_ch);
					XR_OS_MSleep(frm_period);
				}
				printf("******************************** cnt:%d ********************************\n", cnt++);
			}
		}
		return CMD_STATUS_OK;
	default:
		construct_frame(frm_type);
		break;
	}
	buf_dump((uint8_t *)frame.Frame, frame.FrmLength);

	for (i = 0;i < frm_cnt;i++) {
		ret = wlan_send_raw_frame(g_wlan_netif, IEEE80211_FC_STYPE_AUTH, (uint8_t *)frame.Frame, frame.FrmLength);
		printf("cnt:%d\n", i+1);
		if (ret == -1) {
			CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
			return CMD_STATUS_ACKED;
		}
		XR_OS_MSleep(frm_period);
	}

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

#if 0
static enum cmd_status cmd_wlan_set_sniff_ext_param(char *cmd)
{
	int ret, cnt;
	int enable, retry, expenRight, dtim;
	wlan_ext_sniff_extern_param_set_t param;

	cnt = cmd_sscanf(cmd, "e=0x%x r=%d er=%d d=%d",
		&enable, &retry, &expenRight, &dtim);
	if (cnt != 4) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	param.SniffExtFuncEnable = enable;
	param.SniffRetryAfterLostSync = retry;
	param.SniffAdaptiveExpenRight = expenRight;
	param.SniffRetryDtim = dtim;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SNIFF_EXTERN_CFG, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_get_rcv_stat(char *cmd)
{
	int ret, i;
	unsigned int sum_cnt = 0;
	int sum_avg = 0;
	wlan_ext_bcn_status_t bcn_status;

	char dly_info[][20] = {
		"(<0      )",
		"(<500us  )",
		"(<1000us )",
		"(<2000us )",
		"(<4000us )",
		"(<8000us )",
		"(<16000us)",
		"(>16000us)",
	};

	cmd_memset(&bcn_status, 0, sizeof(wlan_ext_bcn_status_t));
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_SNIFF_STAT, (uint32_t)&bcn_status);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	printf("\nAPP AP Beacon Delay Stat Info=================\n");
	for (i = 0; i < 8; i++) {
		printf("cnt %d %s: %d\n",
			i, dly_info[i],
			bcn_status.bcn_delay_cnt[i]);
		sum_cnt += bcn_status.bcn_delay_cnt[i];
	}

	if (sum_cnt)
		sum_avg = bcn_status.bcn_delay_sum / sum_cnt;

	printf("Dur: %d, Max: 0x%X, Rxed: %d, Missed: 0x%X\n",
		bcn_status.bcn_duration,
		bcn_status.bcn_delay_max,
		bcn_status.bcn_rx_cnt,
		bcn_status.bcn_miss_cnt);
	printf("Sum: %d, Cnt: %d, Ava: %d\n",
		bcn_status.bcn_delay_sum,
		sum_cnt,
		sum_avg);
	printf("APP AP Beacon Delay Stat Info=================\n");

	return CMD_STATUS_OK;
}
#endif

uint8_t g_flag_frm_rcv = 0;
uint32_t g_frm_rcv_cnt = 0;
uint32_t g_frm_rcv_mac_cnt = 0;
uint32_t g_frm_not_rcv_cnt = 0;
uint64_t new_timestap=0;
uint64_t last_timestap=0;
extern uint64_t HAL_RTC_FreeRunCntToTime(uint64_t cnt);
extern uint64_t HAL_RTC_GetFreeRunCnt(void);

void sta_rx_cb(uint8_t *data, uint32_t len, void *info)
{
	if (!info) {
		printf("%s(), info NULL\n", __func__);
		return;
	}
	g_flag_frm_rcv = 1;

	struct ieee80211_frame *wh;
	uint8_t filter_mac[6] = {0x14,0x72,0x58,0x36,0x90,0xaa};
	wh = (struct ieee80211_frame *)data;
	g_frm_rcv_cnt++;
	if (memcmp(wh->i_addr3, filter_mac, 6) == 0) {
		g_frm_rcv_mac_cnt++;
	}
	char * str_frm_type;
	switch (wh->i_fc[0] & IEEE80211_FC0_TYPE_MASK)
	{
	case IEEE80211_FC0_TYPE_MGT:
		switch (wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK)
		{
		case IEEE80211_FC_STYPE_ASSOC_REQ:
			str_frm_type = "association req";
			break;
		case IEEE80211_FC_STYPE_ASSOC_RESP:
			str_frm_type = "association resp";
			break;
		case IEEE80211_FC_STYPE_REASSOC_REQ:
			str_frm_type = "reassociation req";
			break;
		case IEEE80211_FC_STYPE_REASSOC_RESP:
			str_frm_type = "reassociation resp";
			break;
		case IEEE80211_FC_STYPE_PROBE_REQ:
			str_frm_type = "probe req";
			break;
		case IEEE80211_FC_STYPE_PROBE_RESP:
			str_frm_type = "probe resp";
			break;
		case IEEE80211_FC_STYPE_BEACON:
			str_frm_type = "beacon";
			break;
		case IEEE80211_FC_STYPE_ATIM:
			str_frm_type = "atim";
			break;
		case IEEE80211_FC_STYPE_DISASSOC:
			str_frm_type = "disassociation";
			break;
		case IEEE80211_FC_STYPE_AUTH:
			str_frm_type = "authentication";
			break;
		case IEEE80211_FC_STYPE_DEAUTH:
			str_frm_type = "deauthentication";
			break;
		case IEEE80211_FC_STYPE_ACTION:
			str_frm_type = "action";
			break;
		default:
			str_frm_type = "unknown mgmt";
			break;
		}
		break;
	case IEEE80211_FC0_TYPE_CTL:
		str_frm_type = "control";
		break;
	case IEEE80211_FC0_TYPE_DATA:
		switch (wh->i_fc[0] & IEEE80211_FC0_SUBTYPE_MASK)
		{
		case IEEE80211_FC0_SUBTYPE_DATA:
			str_frm_type = "data";
			break;
		case IEEE80211_FC0_SUBTYPE_QOS:
			str_frm_type = "qos data";
			break;
		case IEEE80211_FC0_SUBTYPE_NODATA:
			str_frm_type = "null data";
			break;
		default:
			str_frm_type = "unknown data";
			break;
		}
		break;
	default:
		str_frm_type = "unknown";
		break;
	}
	printf("recv pack type:%s cnt:%d, mac cnt:%d, len:%d\n",
		str_frm_type, g_frm_rcv_cnt, g_frm_rcv_mac_cnt, len);
}

static enum cmd_status cmd_wlan_set_rcv_cb(char *cmd)
{
	int cnt, enable;

	cnt = cmd_sscanf(cmd, "e=%d", &enable);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}
	if (enable)
		wlan_monitor_set_rx_cb(g_wlan_netif, sta_rx_cb);
	else
		wlan_monitor_set_rx_cb(g_wlan_netif, NULL);
	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_set_even_ie(char *cmd)
{
	int cnt, enable;
	uint8_t ie[7] = {0xDD, 0x05, 0x00, 0x50, 0xF2, 0x05, 0x01};

	cnt = cmd_sscanf(cmd, "e=%d", &enable);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}
	if (enable) {
		ie[6] ++;
		printf("Set event beacon!\n");
		wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_UPDATE_TEMP_IE, (int)(ie));
	} else {
		printf("Set normal beacon!\n");
		wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_UPDATE_TEMP_IE, (int)(ie));
	}
	return CMD_STATUS_OK;
}

#endif

static enum cmd_status cmd_wlan_set_bcn_win_cfg(char *cmd)
{
	int ret, cnt;
	int start_num, stop_num, amp_us, max_num;
	wlan_ext_bcn_win_param_set_t param;

	cnt = cmd_sscanf(cmd, "start=%d stop=%d amp=%d max=%d",
		&start_num, &stop_num, &amp_us, &max_num);
	if (cnt != 4) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	param.BeaconWindowAdjStartNum = start_num;
	param.BeaconWindowAdjStopNum = stop_num;
	param.BeaconWindowAdjAmpUs = amp_us;
	param.BeaconWindowMaxStartNum = max_num;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_BCN_WIN_CFG, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_scan_freq(char *cmd)
{
	int ret, cnt;
	int num;
	int chn[14];
	int freq[14];
	wlan_ext_scan_freq_t param;

	cnt = cmd_sscanf(cmd, "n=%d c=%d %d %d %d %d %d %d %d %d %d %d %d %d %d", &num,
					&chn[0], &chn[1], &chn[2], &chn[3], &chn[4], &chn[5], &chn[6],
					&chn[7], &chn[8], &chn[9], &chn[10], &chn[11], &chn[12], &chn[13]);
	if (cnt != 15) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}

	if (num > 14) {
		CMD_ERR("%s: invalid num:%d\n", __func__, num);
		return CMD_STATUS_ACKED;
	}
	for (int i = 0;i < num;i++) {
		freq[i] = 2407 + 5 * chn[i];
	}
	param.freq_num = num;
	param.freq_list = freq;
	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_SET_SCAN_FREQ, (int)(&param));

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_pm(char *cmd)
{
	int ret, cnt;
	int enable;

	cnt = cmd_sscanf(cmd, "e=%d", &enable);
	if (cnt != 1) {
		CMD_ERR("cnt %d\n", cnt);
		return CMD_STATUS_INVALID_ARG;
	}
	ret = wlan_set_ps_mode(g_wlan_netif, enable);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

static enum cmd_status cmd_wlan_get_temp_volt(char *cmd)
{
	int ret;
	wlan_ext_temp_volt_get_t param;

	ret = wlan_ext_request(g_wlan_netif, WLAN_EXT_CMD_GET_TEMP_VOLT, (int)(&param));
	CMD_LOG(1, "Temperature:%.02f°C\n", (float)param.Temperature / 16);
	CMD_LOG(1, "Voltage:%.02fV\n", (float)param.Voltage / 16);

	if (ret == -2) {
		CMD_ERR("%s: command '%s' invalid arg\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	} else if (ret == -1) {
		CMD_ERR("%s: command '%s' exec failed\n", __func__, cmd);
		return CMD_STATUS_ACKED;
	}

	return CMD_STATUS_OK;
}

/*
 * wlan commands
 */

static enum cmd_status cmd_wlan_help_exec(char *cmd);

static const struct cmd_data g_wlan_cmds[] = {
	{ "pm_dtim",			cmd_wlan_set_pm_dtim },
	{ "get_pm_dtim",		cmd_wlan_get_pm_dtim },
	{ "set_lsn_intv",		cmd_wlan_set_listen_interval },
	{ "ps_mode",			cmd_wlan_set_ps_mode },
	{ "pm",					cmd_wlan_pm },
	{ "ampdu",				cmd_wlan_set_ampdu },
	{ "retry",				cmd_wlan_set_retry },
	{ "null_prd",			cmd_wlan_set_pm_tx_null_period },
	{ "bcn_win_cfg",		cmd_wlan_set_bcn_win_cfg },
	{ "bcn_win",			cmd_wlan_set_bcn_win_us },
	{ "get_bcn_stat",		cmd_wlan_get_bcn_stat },
	{ "get_cur_rate",		cmd_wlan_get_cur_tx_rate },
	{ "get_cur_signal",		cmd_wlan_get_cur_signal },
	{ "scan_freq",			cmd_wlan_scan_freq },
	{ "set_scan_param",		cmd_wlan_set_scan_param },
	{ "get_temp_volt",		cmd_wlan_get_temp_volt },
#if IPC_FUNC
	{ "set_auto_scan",		cmd_wlan_set_auto_scan },
	{ "ser_start",			cmd_wlan_server_start },
	{ "ser_send",			cmd_wlan_server_send },
	{ "ser_recv",			cmd_wlan_server_recv },
	{ "ser_stop",			cmd_wlan_server_stop },
	{ "cli_start",			cmd_wlan_client_start },
	{ "cli_send",			cmd_wlan_client_send },
	{ "cli_recv",			cmd_wlan_client_recv },
	{ "cli_stop",			cmd_wlan_client_stop },
	{ "p2p_server",			cmd_wlan_set_p2p_server },
	{ "p2p_wkpacket",		cmd_wlan_set_p2p_wkpacket },
	{ "p2p_wakeupip",		cmd_wlan_set_p2p_wakeupip },
	{ "p2p_kpalive",		cmd_wlan_set_p2p_kpalive },
	{ "p2p_hostsleep",		cmd_wlan_set_p2p_hostsleep },
#endif

#if DNA_WLAN
	{ "rcv_spec_frm",		cmd_wlan_set_rcv_special_frm },
	{ "raw_frm_cfg",		cmd_wlan_send_raw_frm_cfg },
	{ "sniff_kp_active",	cmd_wlan_sniff_kp_active },
	{ "sniff_auto_wkp_host",cmd_wlan_set_sniff_auto_wakeup_host },
	{ "sniff_auto_wkp_wifi",cmd_wlan_set_sniff_auto_wakeup_wifi },
	{ "send_raw_frm",		cmd_wlan_send_raw_frm },
	{ "switch_chn",			cmd_wlan_switch_channel_cfg },
	{ "get_chn",			cmd_wlan_get_cur_channel },
	{ "rcv_filter_mac",		cmd_wlan_rcv_frm_filter_mac },
	{ "set_frm_filter",		cmd_wlan_set_frm_filter },
	{ "rcv_filter1_ie",		cmd_wlan_rcv_frm_filter1_ie_cfg },
	{ "rcv_filter2_ie",		cmd_wlan_rcv_frm_filter2_ie_cfg },
	{ "rcv_filter1_pl",		cmd_wlan_rcv_frm_filter1_pl_cfg },
	{ "rcv_filter2_pl",		cmd_wlan_rcv_frm_filter2_pl_cfg },
	{ "set_temp_frm",		cmd_wlan_set_temp_frm },
	{ "send_sync_frm",		cmd_wlan_send_sync_frm },
	//{ "set_ext_param",		cmd_wlan_set_sniff_ext_param },
	//{ "get_rcv_stat",		cmd_wlan_get_rcv_stat },
	{ "set_rcv_cb",			cmd_wlan_set_rcv_cb },
	{ "set_even_ie",		cmd_wlan_set_even_ie },
#endif
	{ "help",				cmd_wlan_help_exec },
};

static enum cmd_status cmd_wlan_help_exec(char *cmd)
{
	return cmd_help_exec(g_wlan_cmds, cmd_nitems(g_wlan_cmds), 16);
}

enum cmd_status cmd_wlan_exec(char *cmd)
{
	if (g_wlan_netif == NULL) {
		return CMD_STATUS_FAIL;
	}
	return cmd_exec(cmd, g_wlan_cmds, cmd_nitems(g_wlan_cmds));
}
#endif
