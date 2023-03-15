/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the people's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hal_cmd.h>
#include <sound/snd_core.h>

#include "snd_sunxi_alg_cfg.h"

/* @ alg_cfg file format
 * invalid line: #xxxxxxxxxx........xxxxxxxxxx
 * invalid line: {blank line}
 * valid line  : [0xnnnn] = 0xnnnnnnnn
 *
 * return:
 * -1	: invalid line
 * 0	: valid line
 */
static int sunxi_alg_cfg_get_line(FILE *fp, char *line_buffer)
{
	char buffer[ALG_CFG_PARM_LEN_MAX] = {0};

	if (fgets(buffer, sizeof(buffer), fp)) {
		if (buffer[0] == '[' && strstr(buffer, "[0x") && strstr(buffer, "] = 0x")) {
			/* printf("%s\n", buffer); */
			memcpy(line_buffer, buffer, ALG_CFG_PARM_LEN_MAX);
		} else {
			return -1;
		}
	}

	return 0;
}

/* @ alg_cfg file format
 * valid line  : [reg] = reg_val
 *
 * return:
 * -1	: parse faild
 * 0	: parse success
 */
static int sunxi_alg_cfg_parse_line(const char *line, unsigned int *reg, unsigned int *reg_val)
{
	unsigned long ret;
	const char *line_tmp;

	if (!line)
		return -1;

	/* get reg */
	line_tmp = line + 1;
	ret = strtoul(line_tmp, NULL, 16);
	if (ret) {
		/* printf("0x%04lx\n", ret); */
		*reg = (unsigned int)ret;
	} else {
		return -1;
	}

	/* get reg val */
	line_tmp = line + 11;
	ret = strtoul(line_tmp, NULL, 16);
	if (ret) {
		/* printf("0x%08lx\n", ret); */
		*reg_val = (unsigned int)ret;
	} else {
		return -1;
	}

	return 0;
}

static void sunxi_alg_cfg_set(unsigned int reg, unsigned int reg_val,
			      struct alg_cfg_reg_domain *domain)
{
	if (!domain) {
		snd_err("domain is invailed\n");
		return;
	}

	if (reg < domain->reg_min || reg > domain->reg_max) {
		snd_err("reg is invailed 0x%x\n", reg);
		return;
	}

	snd_writel(reg_val, domain->reg_base + reg);

	/*
	printf("reg_base -> 0x%x, reg -> 0x%04x, reg_val -> 0x%08x\n",
		domain->reg_base, reg, reg_val);
	*/
}

int sunxi_alg_cfg(char *alg_cfg_name, struct alg_cfg_reg_domain *domain)
{
	FILE *alg_cfg_fp;
	char alg_cfg_line_buffer[ALG_CFG_PARM_LEN_MAX] = {0};
	unsigned int reg, reg_val;
	int ret;

	alg_cfg_fp = fopen(alg_cfg_name, "r");
	if (!alg_cfg_fp) {
		snd_err("alg_cfg file is null: %s\n", alg_cfg_name);
		return -1;
	}

	fseek(alg_cfg_fp, 0, SEEK_SET);
	while (!feof(alg_cfg_fp)) {
		ret = sunxi_alg_cfg_get_line(alg_cfg_fp, alg_cfg_line_buffer);
		if (ret)
			continue;
		ret = sunxi_alg_cfg_parse_line(alg_cfg_line_buffer, &reg, &reg_val);
		if (ret)
			continue;
		sunxi_alg_cfg_set(reg, reg_val, domain);
	}

	if (alg_cfg_fp) {
		fclose(alg_cfg_fp);
	}

	return 0;
}

static struct alg_cfg_reg_domain sunxi_domain_meq;
static struct alg_cfg_reg_domain sunxi_domain_peq;
static struct alg_cfg_reg_domain sunxi_domain_1bdrc;
static struct alg_cfg_reg_domain sunxi_domain_3bdrc;

static int sunxi_alg_cfg_domain_get(struct alg_cfg_reg_domain **domain,
				    enum SUNXI_ALG_CFG_DOMAIN domain_sel)
{
	switch (domain_sel) {
	case SUNXI_ALG_CFG_DOMAIN_MEQ:
		*domain = &sunxi_domain_meq;
		break;
	case SUNXI_ALG_CFG_DOMAIN_PEQ:
		*domain = &sunxi_domain_peq;
		break;
	case SUNXI_ALG_CFG_DOMAIN_1BDRC:
		*domain = &sunxi_domain_1bdrc;
		break;
	case SUNXI_ALG_CFG_DOMAIN_3BDRC:
		*domain = &sunxi_domain_3bdrc;
		break;
	default:
		snd_err("unsupport alg cfg domain sel %d\n", domain_sel);
		return -EINVAL;
	}

	return 0;
}

int sunxi_alg_cfg_domain_set(struct alg_cfg_reg_domain *domain,
			     enum SUNXI_ALG_CFG_DOMAIN domain_sel)
{
	switch (domain_sel) {
	case SUNXI_ALG_CFG_DOMAIN_MEQ:
		memcpy(&sunxi_domain_meq, domain, sizeof(struct alg_cfg_reg_domain));
		break;
	case SUNXI_ALG_CFG_DOMAIN_PEQ:
		memcpy(&sunxi_domain_peq, domain, sizeof(struct alg_cfg_reg_domain));
		break;
	case SUNXI_ALG_CFG_DOMAIN_1BDRC:
		memcpy(&sunxi_domain_1bdrc, domain, sizeof(struct alg_cfg_reg_domain));
		break;
	case SUNXI_ALG_CFG_DOMAIN_3BDRC:
		memcpy(&sunxi_domain_3bdrc, domain, sizeof(struct alg_cfg_reg_domain));
		break;
	default:
		snd_err("unsupport alg cfg domain sel %d\n", domain_sel);
		return -EINVAL;
	}

	return 0;
}

static void usage(void)
{
	printf("Usage: audio_alg_cfg {option} alg_cfg_file\n");
	printf("-MEQ   main EQ\n");
	printf("-PEQ   post EQ\n");
	printf("-1BDRC 1-Band DRC\n");
	printf("-3BDRC 3-Band DRC\n");
}

int cmd_audio_alg_cfg(int argc, char **argv)
{
	int ret;
	char *alg_cfg_name;
	struct alg_cfg_reg_domain *domain;
	enum SUNXI_ALG_CFG_DOMAIN domain_sel;

	if (argc < 3) {
		usage();
		return 0;
	}

	if (strcmp(argv[1], "-MEQ") == 0) {
		domain_sel = SUNXI_ALG_CFG_DOMAIN_MEQ;
	} else if (strcmp(argv[1], "-PEQ") == 0) {
		domain_sel = SUNXI_ALG_CFG_DOMAIN_PEQ;
	} else if (strcmp(argv[1], "-1BDRC") == 0) {
		domain_sel = SUNXI_ALG_CFG_DOMAIN_1BDRC;
	} else if (strcmp(argv[1], "-3BDRC") == 0) {
		domain_sel = SUNXI_ALG_CFG_DOMAIN_3BDRC;
	} else {
		usage();
		return 0;
	}

	alg_cfg_name = argv[2];

	ret = sunxi_alg_cfg_domain_get(&domain, domain_sel);
	if (ret) {
		snd_err("alg_cfg domain get failed\n");
		return -1;
	}

	ret = sunxi_alg_cfg(alg_cfg_name, domain);
	if (ret) {
		snd_err("alg_cfg failed\n");
	}

	return 0;
}
FINSH_FUNCTION_EXPORT_CMD(cmd_audio_alg_cfg, audio_alg_cfg, audio hardware algorithm config);
