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

#include "prj_config.h"
#include <string.h>
#include <stdlib.h>

#include "kernel/os/os.h"
#if PRJCONF_NET_EN
#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#endif /* PRJCONF_NET_EN */
#include "sysinfo.h"
#include "sysinfo_debug.h"

static struct sysinfo g_sysinfo;

#if PRJCONF_NET_EN

static uint8_t m_sysinfo_mac_addr[] = { 0x00, 0x80, 0xE1, 0x29, 0xE8, 0xD1 };

static void sysinfo_gen_mac_random(uint8_t mac_addr[6])
{
	int i;
	for (i = 0; i < 6; ++i) {
		mac_addr[i] = (uint8_t)OS_Rand32();
	}
	mac_addr[0] &= 0xFC;
}

static void sysinfo_init_mac_addr(void)
{
	SYSINFO_DBG("mac addr source: %d\n", PRJCONF_MAC_ADDR_SOURCE);

	switch (PRJCONF_MAC_ADDR_SOURCE) {
	case SYSINFO_MAC_ADDR_CODE:
		memcpy(g_sysinfo.mac_addr, m_sysinfo_mac_addr, SYSINFO_MAC_ADDR_LEN);
		return;
	default:
//		SYSINFO_ERR("invalid mac addr source\n");
		goto random_mac_addr;
	}

random_mac_addr:
	SYSINFO_DBG("random mac addr\n");
	sysinfo_gen_mac_random(g_sysinfo.mac_addr);
}

#endif /* PRJCONF_NET_EN */

static void sysinfo_init_value(void)
{
	sysinfo_default();
}

/**
 * @brief Initialize the sysinfo module
 * @return 0 on success, -1 on failure
 */
int sysinfo_init(void)
{
	sysinfo_init_value();
	return 0;
}

/**
 * @brief DeInitialize the sysinfo module
 * @return None
 */
void sysinfo_deinit(void)
{
}

/**
 * @brief Set default value to sysinfo
 * @return 0 on success, -1 on failure
 */
int sysinfo_default(void)
{
	memset(&g_sysinfo, 0, SYSINFO_SIZE);

#if PRJCONF_NET_EN
	/* MAC address */
	sysinfo_init_mac_addr();

	/* wlan mode */
	g_sysinfo.wlan_mode = WLAN_MODE_STA;

	/* netif STA */
	g_sysinfo.sta_use_dhcp = 1;

	/* netif AP */
	IP4_ADDR(&g_sysinfo.netif_ap_param.ip_addr, 192, 168, 51, 1);
	IP4_ADDR(&g_sysinfo.netif_ap_param.net_mask, 255, 255, 255, 0);
	IP4_ADDR(&g_sysinfo.netif_ap_param.gateway, 192, 168, 51, 1);
#endif

	SYSINFO_DBG("set default value\n");
	return 0;
}

/**
 * @brief Get the pointer of the sysinfo
 * @return Pointer to the sysinfo, NULL on failure
 */
struct sysinfo *sysinfo_get(void)
{
	return &g_sysinfo;
}
