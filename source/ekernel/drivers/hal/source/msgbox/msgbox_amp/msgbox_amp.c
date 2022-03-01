/*
 * this is only for , adopt to other must change the m and n.
 */
#include "../platform-msgbox.h"
#include "hal_msgbox.h"
#include "hal_interrupt.h"
#include "aw_io.h"
#include "hal_clk.h"
#include "hal_reset.h"
#include "hal_mutex.h"

static struct msg_endpoint *it_edp = NULL;
static hal_mutex_t it_edp_mutex;

#define MSGBOX_MAX_QUEUE 8

static inline int calculte_n(int local, int remote)
{
	if (remote < local)
		return remote;
	else
		return remote - 1;
}

static void irq_msgbox_channel_handler(struct msg_endpoint *medp)
{
	void *msg_sts, *msg_reg, *msg_irq_s;
	u32 data;

	msg_sts = (void *)MSGBOX_MSG_STA_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp),
		medp->read_ch);
	msg_reg = (void *)MSGBOX_MSG_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp),
		medp->read_ch);
	msg_irq_s = (void *)MSGBOX_RD_IRQ_STA_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp));

	while (readl(msg_sts)) {
		data = readl(msg_reg);
		if (medp->rec)
			medp->rec(data, medp->private);
	}

	writel(1 << (medp->read_ch * 2), msg_irq_s);
}

static int irq_msgbox_handler(int i, void *p)
{
	struct msg_endpoint *t;

	hal_mutex_lock(it_edp_mutex);
	for (t = it_edp; t != NULL; t = t->next) {
		irq_msgbox_channel_handler(t);
	}
	hal_mutex_unlock(it_edp_mutex);

	return 0;
}

uint32_t hal_msgbox_init(void)
{
	struct reset_control *rst;
	hal_clk_t clk;

	it_edp_mutex = hal_mutex_create();
	// TODO: maybe not always NULL when failed in different OS?
	if (it_edp_mutex == NULL) {
		hal_log_err("%s(%d): Failed to create it_edp_mutex\n", __func__, __LINE__);
		return -1;
	}

	rst = hal_reset_control_get(HAL_SUNXI_RESET, RST_MSGBOX);
	hal_reset_control_deassert(rst);
	hal_reset_control_put(rst);

	clk = hal_clock_get(HAL_SUNXI_CCU, CLK_MSGBOX);
	hal_clock_enable(clk);
	hal_clock_put(clk);

	irq_request(IRQ_MSGBOX, irq_msgbox_handler, NULL);
	irq_enable(IRQ_MSGBOX);

	return 0;
}

static void msgbox_enable_rec_int(struct msg_endpoint *medp)
{
	void *msg_irq_e;
	u32 value;

	msg_irq_e = (void *)MSGBOX_RD_IRQ_EN_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp));

	value = readl(msg_irq_e);
	value |= (1 << (medp->read_ch * 2));
	writel(value, msg_irq_e);

}

static void msgbox_disable_rec_int(struct msg_endpoint *medp)
{
	void *msg_irq_e;
	u32 value;

	msg_irq_e = (void *)MSGBOX_RD_IRQ_EN_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp));

	value = readl(msg_irq_e);
	value &= ~(1 << (medp->read_ch * 2));
	writel(value, msg_irq_e);
}

uint32_t hal_msgbox_alloc_channel(struct msg_endpoint *edp, uint32_t remote,
			      uint32_t read, uint32_t write)
{
	edp->local_amp = THIS_MSGBOX_USE;
	edp->remote_amp = remote;
	edp->read_ch = read;
	edp->write_ch = write;

	hal_mutex_lock(it_edp_mutex);
	/* add to global list */
	if (it_edp == NULL) {
		it_edp = edp;
	} else {
		struct msg_endpoint *t = it_edp;
		while (t) {
			if (t->next == NULL) {
				t->next = edp;
				break;
			}
			t = t->next;
		}
	}
	edp->next = NULL;
	hal_mutex_unlock(it_edp_mutex);

	msgbox_enable_rec_int(edp);

	return 0;
}

void hal_msgbox_free_channel(struct msg_endpoint *edp)
{
	msgbox_disable_rec_int(edp);

	hal_mutex_lock(it_edp_mutex);
	if (it_edp == edp) {
		it_edp = it_edp->next;
	} else {
		struct msg_endpoint *t = it_edp;
		while (t->next) {
			if (t->next == edp) {
				t->next = edp->next;
				break;
			}
			t = t->next;
		}
	}
	hal_mutex_unlock(it_edp_mutex);
}

static void msgbox_channel_send_data(struct msg_endpoint *medp, u32 data)
{
	void *msg_sts, *msg_reg;

	msg_sts = (void *)MSGBOX_MSG_STA_REG(
		medp->remote_amp, calculte_n(medp->remote_amp, medp->local_amp),
		medp->write_ch);
	msg_reg = (void *)MSGBOX_MSG_REG(
		medp->remote_amp, calculte_n(medp->remote_amp, medp->local_amp),
		medp->write_ch);

	while (readl(msg_sts) == MSGBOX_MAX_QUEUE);
	writel(data, msg_reg);
}

u32 hal_msgbox_channel_send(struct msg_endpoint *medp, uint8_t *bf,
			    uint32_t len)
{
	u32 data, i;

	for (i = 0; i < len; i++) {

		if (!(i % 4))
			data = 0;

		data |= *bf++ << ((i % 4) << 3);

		if ((i % 4) == 3 || i == len - 1) {
			msgbox_channel_send_data(medp, data);
		}
	}

	return 0;
}


