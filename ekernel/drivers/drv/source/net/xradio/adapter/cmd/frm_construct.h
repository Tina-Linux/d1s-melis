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

/*
 * Management information element payloads.
 */
#define	IEEE80211_FC0_VERSION_MASK		0x03
#define	IEEE80211_FC0_VERSION_SHIFT		0
#define	IEEE80211_FC0_VERSION_0			0x00
#define	IEEE80211_FC0_TYPE_MASK			0x0c
#define	IEEE80211_FC0_TYPE_SHIFT		2
#define	IEEE80211_FC0_TYPE_MGT			0x00
#define	IEEE80211_FC0_TYPE_CTL			0x04
#define	IEEE80211_FC0_TYPE_DATA			0x08

#define	IEEE80211_FC0_SUBTYPE_MASK		0xf0
#define	IEEE80211_FC0_SUBTYPE_SHIFT		4
/* for TYPE_MGT */
#define	IEEE80211_FC0_SUBTYPE_ASSOC_REQ		0x00
#define	IEEE80211_FC0_SUBTYPE_ASSOC_RESP	0x10
#define	IEEE80211_FC0_SUBTYPE_REASSOC_REQ	0x20
#define	IEEE80211_FC0_SUBTYPE_REASSOC_RESP	0x30
#define	IEEE80211_FC0_SUBTYPE_PROBE_REQ		0x40
#define	IEEE80211_FC0_SUBTYPE_PROBE_RESP	0x50
#define	IEEE80211_FC0_SUBTYPE_BEACON		0x80
#define	IEEE80211_FC0_SUBTYPE_ATIM			0x90
#define	IEEE80211_FC0_SUBTYPE_DISASSOC		0xa0
#define	IEEE80211_FC0_SUBTYPE_AUTH			0xb0
#define	IEEE80211_FC0_SUBTYPE_DEAUTH		0xc0
#define	IEEE80211_FC0_SUBTYPE_ACTION		0xd0
#define	IEEE80211_FC0_SUBTYPE_ACTION_NOACK	0xe0
/* for TYPE_DATA (bit combination) */
#define	IEEE80211_FC0_SUBTYPE_DATA			0x00
#define	IEEE80211_FC0_SUBTYPE_CF_ACK		0x10
#define	IEEE80211_FC0_SUBTYPE_CF_POLL		0x20
#define	IEEE80211_FC0_SUBTYPE_CF_ACPL		0x30
#define	IEEE80211_FC0_SUBTYPE_NODATA		0x40
#define	IEEE80211_FC0_SUBTYPE_CFACK			0x50
#define	IEEE80211_FC0_SUBTYPE_CFPOLL		0x60
#define	IEEE80211_FC0_SUBTYPE_CF_ACK_CF_ACK	0x70
#define	IEEE80211_FC0_SUBTYPE_QOS			0x80
#define	IEEE80211_FC0_SUBTYPE_QOS_NULL		0xc0

#define	IEEE80211_FC1_DIR_MASK			0x03
#define	IEEE80211_FC1_DIR_NODS			0x00	/* STA->STA */
#define	IEEE80211_FC1_DIR_TODS			0x01	/* STA->AP  */
#define	IEEE80211_FC1_DIR_FROMDS		0x02	/* AP ->STA */
#define	IEEE80211_FC1_DIR_DSTODS		0x03	/* AP ->AP  */

#define IEEE80211_ADDR_LEN 6

struct ieee80211_frame_ {
	uint8_t		i_fc[2];
	uint16_t	i_dur;
	uint8_t		i_addr1[IEEE80211_ADDR_LEN];
	uint8_t		i_addr2[IEEE80211_ADDR_LEN];
	uint8_t		i_addr3[IEEE80211_ADDR_LEN];
	uint16_t	i_seq;
	/* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
	/* see below */
} __packed;

enum {
	IEEE80211_ELEMID_SSID		= 0,
	IEEE80211_ELEMID_RATES		= 1,
	IEEE80211_ELEMID_FHPARMS	= 2,
	IEEE80211_ELEMID_DSPARMS	= 3,
	IEEE80211_ELEMID_CFPARMS	= 4,
	IEEE80211_ELEMID_TIM		= 5,
	IEEE80211_ELEMID_IBSSPARMS	= 6,
	IEEE80211_ELEMID_COUNTRY	= 7,
	IEEE80211_ELEMID_CHALLENGE	= 16,
	/* 17-31 reserved for challenge text extension */
	IEEE80211_ELEMID_PWRCNSTR	= 32,
	IEEE80211_ELEMID_PWRCAP		= 33,
	IEEE80211_ELEMID_TPCREQ		= 34,
	IEEE80211_ELEMID_TPCREP		= 35,
	IEEE80211_ELEMID_SUPPCHAN	= 36,
	IEEE80211_ELEMID_CSA		= 37,
	IEEE80211_ELEMID_MEASREQ	= 38,
	IEEE80211_ELEMID_MEASREP	= 39,
	IEEE80211_ELEMID_QUIET		= 40,
	IEEE80211_ELEMID_IBSSDFS	= 41,
	IEEE80211_ELEMID_ERP		= 42,
	IEEE80211_ELEMID_HTCAP		= 45,	// HT Capabilities element of IEEE 802.11-2012
	IEEE80211_ELEMID_QOS		= 46,
	IEEE80211_ELEMID_RSN		= 48,
	IEEE80211_ELEMID_XRATES		= 50,
	IEEE80211_ELEMID_HTINFO		= 61,	// HT Operation element of IEEE 802.11-2012
	IEEE80211_ELEMID_TPC		= 150,
	IEEE80211_ELEMID_CCKM		= 156,
	IEEE80211_ELEMID_VENDOR		= 221,	/* vendor private */

	/*
	 * 802.11s IEs
	 * NB: On vanilla Linux still IEEE80211_ELEMID_MESHPEER = 55,
	 * but they defined a new with id 117 called PEER_MGMT.
	 * NB: complies with open80211
	 */
	IEEE80211_ELEMID_MESHCONF	= 113,
	IEEE80211_ELEMID_MESHID		= 114,
	IEEE80211_ELEMID_MESHLINK	= 115,
	IEEE80211_ELEMID_MESHCNGST	= 116,
	IEEE80211_ELEMID_MESHPEER	= 117,
	IEEE80211_ELEMID_MESHCSA	= 118,
	IEEE80211_ELEMID_MESHTIM	= 39, /* XXX: remove */
	IEEE80211_ELEMID_MESHAWAKEW	= 119,
	IEEE80211_ELEMID_MESHBEACONT	= 120,
	/* 121-124 MMCAOP not implemented yet */
	IEEE80211_ELEMID_MESHGANN	= 125,
	IEEE80211_ELEMID_MESHRANN	= 126,
	/* 127 Extended Capabilities */
	/* 128-129 reserved */
	IEEE80211_ELEMID_MESHPREQ	= 130,
	IEEE80211_ELEMID_MESHPREP	= 131,
	IEEE80211_ELEMID_MESHPERR	= 132,
	/* 133-136 reserved */
	IEEE80211_ELEMID_MESHPXU	= 137,
	IEEE80211_ELEMID_MESHPXUC	= 138,
	IEEE80211_ELEMID_MESHAH		= 60, /* XXX: remove */
};


#define	IEEE80211_CAPINFO_ESS			0x0001
#define	IEEE80211_CAPINFO_IBSS			0x0002
#define	IEEE80211_CAPINFO_CF_POLLABLE		0x0004
#define	IEEE80211_CAPINFO_CF_POLLREQ		0x0008
#define	IEEE80211_CAPINFO_PRIVACY		0x0010
#define	IEEE80211_CAPINFO_SHORT_PREAMBLE	0x0020
#define	IEEE80211_CAPINFO_PBCC			0x0040
#define	IEEE80211_CAPINFO_CHNL_AGILITY		0x0080
#define	IEEE80211_CAPINFO_SPECTRUM_MGMT		0x0100
/* bit 9 is reserved */
#define	IEEE80211_CAPINFO_SHORT_SLOTTIME	0x0400
#define	IEEE80211_CAPINFO_RSN			0x0800
/* bit 12 is reserved */
#define	IEEE80211_CAPINFO_DSSSOFDM		0x2000
/* bits 14-15 are reserved */

uint8_t * add_ssid(uint8_t *frm, const char *ssid, u_int len);
uint8_t * add_channel(uint8_t *frm, uint8_t channel);
uint16_t getcapinfo(void);

/*
 * 802.11 rate set.
 */
#define	IEEE80211_RATE_SIZE	8		/* 802.11 standard */
#define	IEEE80211_RATE_MAXSIZE	15		/* max rates we'll handle */
uint8_t * add_rates(uint8_t *frm);
uint8_t * add_xrates(uint8_t *frm);

/**
 * struct ieee80211_tim_ie
 *
 * This structure refers to "Traffic Indication Map information element"
 */
struct ieee80211_tim_ie {
	uint8_t		tim_ie;			/* IEEE80211_ELEMID_TIM */
	uint8_t		tim_len;
	uint8_t		tim_count;		/* DTIM count */
	uint8_t		tim_period;		/* DTIM period */
	uint8_t		tim_bitctl;		/* bitmap control */
	uint8_t		tim_bitmap[1];		/* variable-length bitmap */
} __packed;

/* ERP information element flags */
#define	IEEE80211_ERP_NON_ERP_PRESENT	0x01
#define	IEEE80211_ERP_USE_PROTECTION	0x02
#define	IEEE80211_ERP_LONG_PREAMBLE	0x04
uint8_t * add_erp(uint8_t *frm);

#define	WME_OUI			0xf25000
#define	WME_OUI_TYPE		0x02
#define	WME_INFO_OUI_SUBTYPE	0x00
#define	WME_PARAM_OUI_SUBTYPE	0x01
#define	WME_VERSION		1
#define	WME_OUI_BYTES		0x00, 0x50, 0xf2

/* WME stream classes */
#define	WME_AC_BE	0		/* best effort */
#define	WME_AC_BK	1		/* background */
#define	WME_AC_VI	2		/* video */
#define	WME_AC_VO	3		/* voice */
/*
 * WME/802.11e information element.
 */
struct ieee80211_wme_info {
	uint8_t		wme_id;		/* IEEE80211_ELEMID_VENDOR */
	uint8_t		wme_len;	/* length in bytes */
	uint8_t		wme_oui[3];	/* 0x00, 0x50, 0xf2 */
	uint8_t		wme_type;	/* OUI type */
	uint8_t		wme_subtype;	/* OUI subtype */
	uint8_t		wme_version;	/* spec revision */
	uint8_t		wme_info;	/* QoS info */
} __packed;
/*
 * WME AC parameter field
 */
struct ieee80211_wme_acparams {
	uint8_t		acp_aci_aifsn;
	uint8_t		acp_logcwminmax;
	uint16_t	acp_txop;
} __packed;

#define WME_NUM_AC		4	/* 4 AC categories */
#define	WME_NUM_TID		16	/* 16 tids */

#define WME_PARAM_ACI		0x60	/* Mask for ACI field */
#define WME_PARAM_ACI_S		5	/* Shift for ACI field */
#define WME_PARAM_ACM		0x10	/* Mask for ACM bit */
#define WME_PARAM_ACM_S		4	/* Shift for ACM bit */
#define WME_PARAM_AIFSN		0x0f	/* Mask for aifsn field */
#define WME_PARAM_AIFSN_S	0	/* Shift for aifsn field */
#define WME_PARAM_LOGCWMIN	0x0f	/* Mask for CwMin field (in log) */
#define WME_PARAM_LOGCWMIN_S	0	/* Shift for CwMin field */
#define WME_PARAM_LOGCWMAX	0xf0	/* Mask for CwMax field (in log) */
#define WME_PARAM_LOGCWMAX_S	4	/* Shift for CwMax field */

#define WME_AC_TO_TID(_ac) (       \
	((_ac) == WME_AC_VO) ? 6 : \
	((_ac) == WME_AC_VI) ? 5 : \
	((_ac) == WME_AC_BK) ? 1 : \
	0)

#define TID_TO_WME_AC(_tid) (      \
	((_tid) == 0 || (_tid) == 3) ? WME_AC_BE : \
	((_tid) < 3) ? WME_AC_BK : \
	((_tid) < 6) ? WME_AC_VI : \
	WME_AC_VO)
/*
 * WME Parameter Element
 */
struct ieee80211_wme_param {
	uint8_t		param_id;
	uint8_t		param_len;
	uint8_t		param_oui[3];
	uint8_t		param_oui_type;
	uint8_t		param_oui_subtype;
	uint8_t		param_version;
	uint8_t		param_qosInfo;
#define	WME_QOSINFO_COUNT	0x0f	/* Mask for param count field */
#define WME_QOSINFO_UAPSD	0x80	/* Mask for U-APSD field */
#define WME_QOSINFO_UAPSD_S	7	/* Shift for U-APSD field */
	uint8_t		param_reserved;
	struct ieee80211_wme_acparams	params_acParams[WME_NUM_AC];
} __packed;
/*
 * WME/WMM support.
 */
struct wmeParams {
	uint8_t		wmep_acm;
	uint8_t		wmep_aifsn;
	uint8_t		wmep_logcwmin;		/* log2(cwmin) */
	uint8_t		wmep_logcwmax;		/* log2(cwmax) */
	uint8_t		wmep_txopLimit;
	uint8_t		wmep_noackPolicy;	/* 0 (ack), 1 (no ack) */
};
// WMM Parameter Element, used by AP mode, don't support U-APSD
uint8_t * add_wme_param(uint8_t *frm);

void buf_dump(unsigned char* buf, int len);
int beacon_construct(uint8_t *frm);
int probereq_construct(uint8_t *frm);
int probersp_construct(uint8_t *frm);
int assocreq_construct(uint8_t *frm);
int assocrsp_construct(uint8_t *frm);
int reassocreq_construct(uint8_t *frm);
int reassocrsp_construct(uint8_t *frm);
int disassocreq_construct(uint8_t *frm);
int atim_construct(uint8_t *frm);
int auth_construct(uint8_t *frm);
int deauth_construct(uint8_t *frm);
int action_construct(uint8_t *frm);
int nulldata_construct(uint8_t *frm);
int data_construct(uint8_t *frm);
int qosdata_construct(uint8_t *frm);
int arpreq_construct(uint8_t *frm);

