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

#include <stdio.h>
#include <string.h>

//#include "common/framework/platform_init.h"
#include "kernel/os/os.h"
#include "sys/endian.h"
#include "frm_construct.h"

const char * ssid = "AP_test";
uint8_t bssid[6] = {0x14,0x72,0x58,0x36,0x90,0xaa};
uint8_t src_mac[6] = {0x14,0x72,0x58,0x36,0x90,0xaa};
uint8_t dest_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t src_ip[4] = {192,168,51,123};
uint8_t ap_channel = 1;
/*
 * Add an ssid element to a frame.
 */
uint8_t * add_ssid(uint8_t *frm, const char *ssid, u_int len)
{
	*frm++ = IEEE80211_ELEMID_SSID;
	*frm++ = len;
	memcpy(frm, ssid, len);
	return frm + len;
}

uint8_t * add_channel(uint8_t *frm, uint8_t channel)
{
	*frm++ = IEEE80211_ELEMID_DSPARMS;
	*frm++ = 1;
	*frm++ = channel;
	return frm;
}

uint16_t getcapinfo(void)
{
	uint16_t capinfo;
	capinfo = IEEE80211_CAPINFO_ESS;
	//capinfo |= IEEE80211_CAPINFO_PRIVACY;
	capinfo |= IEEE80211_CAPINFO_SHORT_PREAMBLE;
	capinfo |= IEEE80211_CAPINFO_SHORT_SLOTTIME;
	return capinfo;
}

/*
 * 802.11 rate set.
 */
uint8_t rates_tab[IEEE80211_RATE_SIZE] = {
0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24
};
uint8_t xrates_tab[4] = {
0x30, 0x48, 0x60, 0x6C
};

/*
 * Add a supported rates element id to a frame.
 */
uint8_t * add_rates(uint8_t *frm) // @esp
{
	int nrates;
	*frm++ = IEEE80211_ELEMID_RATES;
	nrates = IEEE80211_RATE_SIZE;
	*frm++ = nrates;
	memcpy(frm, rates_tab, nrates);
	return frm + nrates;
}
uint8_t * add_xrates(uint8_t *frm) // @esp
{
	int nrates = 4;
	*frm++ = IEEE80211_ELEMID_XRATES;
	*frm++ = nrates;
	memcpy(frm, xrates_tab, nrates);
	frm += nrates;
	return frm;
}


uint8_t ie_oui[5] = {
0x00, 0x50, 0xF2, 0x05, 0x01
};

uint8_t * add_ie(uint8_t *frm) // @esp
{
	int ie_len = 5;
	*frm++ = IEEE80211_ELEMID_VENDOR;
	*frm++ = ie_len;
	memcpy(frm, ie_oui, ie_len);
	frm += ie_len;
	return frm;
}

uint8_t llc_hdr[8] = {
0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x08, 0x06
};
uint8_t * add_llc_hdr(uint8_t *frm) // @esp
{
	int llc_len = 8;
	memcpy(frm, llc_hdr, llc_len);
	frm += llc_len;
	return frm;
}

uint8_t arp_hdr[8] = {
0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01
};
uint8_t * add_arp_hdr(uint8_t *frm) // @esp
{
	int arp_len = 8;
	memcpy(frm, arp_hdr, arp_len);
	frm += arp_len;
	return frm;
}

void buf_dump(unsigned char* buf, int len)
{
	int i;
	for (i = 1;i < len + 1;i++) {
		printf("%02X ", buf[i-1]);
		if (i % 16 == 0)
			printf("\n");
	}
}

/*
 * create a template frame for beacon
 */
int beacon_construct(uint8_t *frm)
{
	int frm_len;
	uint16_t capinfo;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 | IEEE80211_FC0_TYPE_MGT |
	    IEEE80211_FC0_SUBTYPE_BEACON;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;

	memset(wh->i_addr1, 0xFF, 6);
	memcpy(wh->i_addr2, bssid, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	/*
	 * beacon frame format
	 *	[8] time stamp
	 *	[2] beacon interval
	 *	[2] cabability information
	 *	[tlv] ssid
	 *	[tlv] supported rates
	 *	[3] parameter set (DS)
	 *	[8] CF parameter set (optional)
	 *	[tlv] parameter set (IBSS/TIM)
	 *	[tlv] country (optional)
	 *	[3] power control (optional)
	 *	[5] channel switch announcement (CSA) (optional)
	 *	[tlv] extended rate phy (ERP)
	 *	[tlv] extended supported rates
	 *	[tlv] RSN parameters
	 *	[tlv] HT capabilities
	 *	[tlv] HT information
	 * XXX Vendor-specific OIDs (e.g. Atheros)
	 *	[tlv] WPA parameters
	 *	[tlv] WME parameters
	 *	[tlv] Vendor OUI HT capabilities (optional)
	 *	[tlv] Vendor OUI HT information (optional)
	 *	[tlv] Atheros capabilities (optional)
	 *	[tlv] TDMA parameters (optional)
	 *	[tlv] Mesh ID (MBSS)
	 *	[tlv] Mesh Conf (MBSS)
	 *	[tlv] application data (optional)
	 */

	memset(frm, 0, 8);	/* XXX timestamp is set by hardware/driver */
	frm += 8;
	*(uint16_t *)frm = htole16(100);
	frm += 2;
	capinfo = getcapinfo();
	*(uint16_t *)frm = htole16(capinfo);
	frm += 2;
	frm = add_ssid(frm, ssid, strlen(ssid));
	frm = add_rates(frm);
	frm = add_channel(frm, ap_channel);
	/* TIM IE is the same for Mesh and Hostap */
	struct ieee80211_tim_ie *tie = (struct ieee80211_tim_ie *) frm;

	tie->tim_ie = IEEE80211_ELEMID_TIM;
	tie->tim_len = 4;	/* length */
	tie->tim_count = 0;	/* DTIM count */
	tie->tim_period = 1;	/* DTIM period */
	tie->tim_bitctl = 0;	/* bitmap control */
	tie->tim_bitmap[0] = 0;	/* Partial Virtual Bitmap */
	frm += sizeof(struct ieee80211_tim_ie);
//	frm = add_erp(frm);
//	frm = add_xrates(frm);
//	frm = add_wme_param(frm);
	frm = add_ie(frm);
	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for probe req
 */
int probereq_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_PROBE_REQ;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, dest_mac, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	/*
	 * prreq frame format
	 *	[tlv] ssid
	 *	[tlv] supported rates
	 *	[tlv] RSN (optional)
	 *	[tlv] extended supported rates
	 *	[tlv] WPA (optional)
	 *	[tlv] user-specified ie's
	 */

	frm = add_ssid(frm, ssid, strlen(ssid));
	frm = add_rates(frm);
	//frm = add_rsn(frm);
	frm = add_xrates(frm);
	//frm = ieee80211_add_wpa(frm, vap);
	//frm = add_appie(frm, vap->iv_appie_probereq);
	frm = add_ie(frm);
	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}


/*
 * create a template frame for probe resp
 */
int probersp_construct(uint8_t *frm)
{
	int frm_len;
	uint16_t capinfo;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_PROBE_RESP;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	/*
	 * probe response frame format
	 *	[8] time stamp
	 *	[2] beacon interval
	 *	[2] cabability information
	 *	[tlv] ssid
	 *	[tlv] supported rates
	 *	[tlv] parameter set (FH/DS)
	 *	[tlv] parameter set (IBSS)
	 *	[tlv] country (optional)
	 *	[3] power control (optional)
	 *	[5] channel switch announcement (CSA) (optional)
	 *	[tlv] extended rate phy (ERP)
	 *	[tlv] extended supported rates
	 *	[tlv] RSN (optional)
	 *	[tlv] HT capabilities
	 *	[tlv] HT information
	 *	[tlv] WPA (optional)
	 *	[tlv] WME (optional)
	 *	[tlv] Vendor OUI HT capabilities (optional)
	 *	[tlv] Vendor OUI HT information (optional)
	 *	[tlv] Atheros capabilities
	 *	[tlv] AppIE's (optional)
	 *	[tlv] Mesh ID (MBSS)
	 *	[tlv] Mesh Conf (MBSS)
	 */

	memset(frm, 0, 8);	/* timestamp should be filled later */
	frm += 8;
	*(uint16_t *)frm = htole16(100);
	frm += 2;
	capinfo = getcapinfo();
	*(uint16_t *)frm = htole16(capinfo);
	frm += 2;
	frm = add_ssid(frm, ssid, strlen(ssid));
	frm = add_rates(frm);
	frm = add_channel(frm, ap_channel);
	frm = add_xrates(frm);
	//frm = add_rsn(frm);
	//frm = add_htcap_vendor(frm);
	//frm = add_htinfo_vendor(frm);
	//frm = add_appie(frm);
	frm = add_ie(frm);
	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for associate req
 */
int assocreq_construct(uint8_t *frm)
{
	int frm_len;
	uint16_t capinfo;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_ASSOC_REQ;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	/*
	 * asreq frame format
	 *	[2] capability information
	 *	[2] listen interval
	 *	[6*] current AP address (reassoc only)
	 *	[tlv] ssid
	 *	[tlv] supported rates
	 *	[tlv] extended supported rates
	 *	[4] power capability (optional)
	 *	[28] supported channels (optional)
	 *	[tlv] HT capabilities
	 *	[tlv] WME (optional)
	 *	[tlv] Vendor OUI HT capabilities (optional)
	 *	[tlv] Atheros capabilities (if negotiated)
	 *	[tlv] AppIE's (optional)
	 */

	capinfo = getcapinfo();
	*(uint16_t *)frm = htole16(capinfo);
	frm += 2;
	*(uint16_t *)frm = 1;	/* listen interval */
	frm += 2;
	frm = add_ssid(frm, ssid, strlen(ssid));
	frm = add_rates(frm);
//	frm = add_rsn(frm, vap);
	frm = add_xrates(frm);
	frm = add_ie(frm);
	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for associate resp
 */
int assocrsp_construct(uint8_t *frm)
{
	int frm_len;
	uint16_t capinfo;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_ASSOC_RESP;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	/*
	 * asresp frame format
	 *	[2] capability information
	 *	[2] status
	 *	[2] association ID
	 *	[tlv] supported rates
	 *	[tlv] extended supported rates
	 *	[tlv] HT capabilities (standard, if STA enabled)
	 *	[tlv] HT information (standard, if STA enabled)
	 *	[tlv] WME (if configured and STA enabled)
	 *	[tlv] HT capabilities (vendor OUI, if STA enabled)
	 *	[tlv] HT information (vendor OUI, if STA enabled)
	 *	[tlv] Atheros capabilities (if STA enabled)
	 *	[tlv] AppIE's (optional)
	 */
	capinfo = getcapinfo();
	*(uint16_t *)frm = htole16(capinfo);
	frm += 2;
	*(uint16_t *)frm = 0;	/* status */
	frm += 2;
	*(uint16_t *)frm = 1;	/* association ID */
	frm += 2;
	frm = add_rates(frm);
	frm = add_xrates(frm);
	frm = add_ie(frm);

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for reassociate req
 */
int reassocreq_construct(uint8_t *frm)
{
	int frm_len;
	uint16_t capinfo;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_REASSOC_REQ;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	/*
	 * asreq frame format
	 *	[2] capability information
	 *	[2] listen interval
	 *	[6*] current AP address (reassoc only)
	 *	[tlv] ssid
	 *	[tlv] supported rates
	 *	[tlv] extended supported rates
	 *	[4] power capability (optional)
	 *	[28] supported channels (optional)
	 *	[tlv] HT capabilities
	 *	[tlv] WME (optional)
	 *	[tlv] Vendor OUI HT capabilities (optional)
	 *	[tlv] Atheros capabilities (if negotiated)
	 *	[tlv] AppIE's (optional)
	 */

	capinfo = getcapinfo();
	*(uint16_t *)frm = htole16(capinfo);
	frm += 2;
	*(uint16_t *)frm = 1;	/* listen interval */
	frm += 2;
	memcpy(frm, bssid, 6);
	frm += IEEE80211_ADDR_LEN;
	frm = add_ssid(frm, ssid, strlen(ssid));
	frm = add_rates(frm);
//	frm = add_rsn(frm, vap);
	frm = add_xrates(frm);
	frm = add_ie(frm);

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for reassociate resp
 */
int reassocrsp_construct(uint8_t *frm)
{
	int frm_len;
	uint16_t capinfo;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_REASSOC_RESP;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	/*
	 * asresp frame format
	 *	[2] capability information
	 *	[2] status
	 *	[2] association ID
	 *	[tlv] supported rates
	 *	[tlv] extended supported rates
	 *	[tlv] HT capabilities (standard, if STA enabled)
	 *	[tlv] HT information (standard, if STA enabled)
	 *	[tlv] WME (if configured and STA enabled)
	 *	[tlv] HT capabilities (vendor OUI, if STA enabled)
	 *	[tlv] HT information (vendor OUI, if STA enabled)
	 *	[tlv] Atheros capabilities (if STA enabled)
	 *	[tlv] AppIE's (optional)
	 */
	capinfo = getcapinfo();
	*(uint16_t *)frm = htole16(capinfo);
	frm += 2;
	*(uint16_t *)frm = 0;	/* status */
	frm += 2;
	*(uint16_t *)frm = 1;	/* association ID */
	frm += 2;
	frm = add_rates(frm);
	frm = add_xrates(frm);
	frm = add_ie(frm);

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for disassociate req
 */
int disassocreq_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_DISASSOC;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);
	*(uint16_t *)frm = 8;	/* reason code 8:Disassociated because sending station is leaving */
	frm += 2;

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for atim
 */
int atim_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_ATIM;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);


	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for auth
 */
int auth_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_AUTH;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);

	*(uint16_t *)frm = 0;	/* IEEE80211_AUTH_ALG_OPEN */
	frm += 2;
	*(uint16_t *)frm = 1;	/* sequence number */
	frm += 2;
	*(uint16_t *)frm = 0;	/* status */
	frm += 2;

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for deauth
 */
int deauth_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_DEAUTH;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);
	*(uint16_t *)frm = 15;	/* reason code 15:4-way handshake timeout */
	frm += 2;

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for action
 */
int action_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_ACTION;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);
	*frm++ = 3;	/* category 3:Block Ack */
	*frm++ = 0; /* action 0:ADDBA req */
	*frm++ = 1;	/* dialog token */
	*(uint16_t *)frm = 0x0402;	/* ba param set */
	frm += 2;
	*(uint16_t *)frm = 0;		/* ba timeout */
	frm += 2;
	*(uint16_t *)frm = 0x0030;	/* ba seq ctl */
	frm += 2;

	frm = add_ie(frm);

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for SA Query
 */
int sa_query_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_MGT | IEEE80211_FC0_SUBTYPE_ACTION;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);
	*frm++ = 8;	/* category 8:WLAN_ACTION_SA_QUERY */
	*frm++ = 0; /* action 0:WLAN_SA_QUERY_REQUEST */
	*(uint16_t *)frm = 0x0402;	/* trans_id set */
	frm += 2;

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for nulldata
 */
int nulldata_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_NODATA;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);


	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for data
 */
int data_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_DATA;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);
	for (int i=0;i<16;i++) {
		*frm = i;
		frm++;
	}
	frm = add_ie(frm);

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for QOS data
 */
int qosdata_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_QOS;
	wh->i_fc[1] = IEEE80211_FC1_DIR_NODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, dest_mac, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);
	for (int i=0;i<16;i++) {
		*frm = i;
		frm++;
	}
	frm = add_ie(frm);

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}

/*
 * create a template frame for ARP req
 */
int arpreq_construct(uint8_t *frm)
{
	int frm_len;
	struct ieee80211_frame_ *wh;

	wh = (struct ieee80211_frame_ *)frm;
	wh->i_fc[0] = IEEE80211_FC0_VERSION_0 |
		IEEE80211_FC0_TYPE_DATA | IEEE80211_FC0_SUBTYPE_QOS;
	wh->i_fc[1] = IEEE80211_FC1_DIR_TODS;
	wh->i_dur = 0;
	memcpy(wh->i_addr1, bssid, 6);
	memcpy(wh->i_addr2, src_mac, 6);
	memcpy(wh->i_addr3, bssid, 6);
	//memcpy(wh->i_addr3, dest_mac, 6);
	wh->i_seq = 0;
	frm += sizeof(struct ieee80211_frame_);
	*frm = 0;//QOS control field
	frm++;
	*frm = 0;
	frm++;
	frm = add_llc_hdr(frm);
	frm = add_arp_hdr(frm);
	memcpy(frm, src_mac, 6);//sender hardware addr
	frm += 6;
	memcpy(frm, src_ip, 4);//sender hardware
	frm += 4;
	memset(frm, 255, 4);//sender internet address
	frm += 4;
	memset(frm, 0xff, 6);//target hardware addr
	frm += 6;

	frm_len = frm - (uint8_t *)wh;
	return frm_len;
}


