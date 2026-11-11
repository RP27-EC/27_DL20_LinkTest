/**
 ******************************************************************************
 * @file    community_task.c
 * @brief   DL20 无线串口收发测试（独立测试工程，不属于 RP27_Dart）
 ******************************************************************************
 * @attention
 *  硬件：USART1（C 板丝印 UART2，PA9 TX / PB7 RX），115200 8N1，接 DL20。
 *  DL20 波特率必须按键配置为 115200，两只模块同信道，一只 A 端一只 B 端。
 *
 *  协议：ASCII 文本行，便于串口助手直接查看。
 *    本板每 200 ms 发送：$DL20,<本板ID>,<序号>,<最近收到的对端序号>*<校验>\r\n
 *    校验为 '$' 与 '*' 之间全部字符的异或，两位大写十六进制。
 *
 *  用法一（单板 + 电脑）：另一只 DL20 经 USB 转 TTL 接电脑。
 *    串口助手能看到上述帧；电脑发送任意一行文本（以回车换行结尾），
 *    本板回 ECHO,<本板ID>:<原文>\r\n。
 *  用法二（双板）：两块板烧同一份固件，互相解析对方帧并统计丢包。
 *
 *  观测：dl20_stat 可在 Keil 调试 Watch 中查看；USB 虚拟串口每秒打印一行统计。
 *  LED：蓝灯每次发送翻转；绿灯每收到一条有效数据翻转；1 s 内没有有效数据则红灯常亮。
 *
 *  NOTE: DL20 点对点模式最高约 3300 字节/秒，本测试约 150 字节/秒，远低于上限。
 ******************************************************************************
 */
#include "community_task.h"
#include "drv_uart.h"
#include "led.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Private macro -------------------------------------------------------------*/
#define DL20_TX_PERIOD_MS     (200u)   /* 发帧周期 ms */
#define DL20_LINK_TIMEOUT_MS  (1000u)  /* 判定断链 ms */
#define DL20_STAT_PERIOD_MS   (1000u)  /* USB 打印周期 ms */
#define DL20_RING_SIZE        (256u)   /* 接收环形缓冲 B，须为 2 的幂 */
#define DL20_LINE_MAX         (64u)    /* 单行最大长度 B */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
	uint16_t self_id;        /* 本板 ID，取自芯片 UID */
	uint16_t peer_id;        /* 最近对端 ID，0 表示未收到 */
	uint32_t tx_frames;      /* 已发帧数 */
	uint32_t peer_frames;    /* 收到的有效对端帧数 */
	uint32_t peer_lost;      /* 按序号推算的丢帧数 */
	uint32_t peer_dup;       /* 重复序号帧数 */
	uint16_t peer_last_seq;  /* 最近对端序号 */
	uint16_t peer_ack;       /* 对端回报收到的本板序号 */
	uint32_t pc_lines;       /* 电脑发来并已回显的行数 */
	uint32_t bad_frames;     /* 校验或格式错误的 $ 帧数 */
	uint32_t self_frames;    /* 收到本板 ID 的帧数，应为 0 */
	uint32_t line_overflow;  /* 超长被丢弃的行数 */
	uint32_t ring_overflow;  /* 环形缓冲溢出字节数 */
	uint32_t rx_bytes;       /* 串口收到的总字节数 */
	uint32_t last_valid_ms;  /* 最近有效数据时刻 ms */
	uint8_t  link_ok;        /* 1：1 s 内有有效数据 */
} dl20_stat_t;

/* Exported variables --------------------------------------------------------*/
volatile dl20_stat_t dl20_stat;

/* Private variables ---------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern USBD_HandleTypeDef hUsbDeviceFS;   /* 定义于 usb_device.c */

static uint8_t           ring_buf[DL20_RING_SIZE];
static volatile uint16_t ring_head;   /* 中断写入位置 */
static volatile uint16_t ring_tail;   /* 任务读取位置 */

static char     line_buf[DL20_LINE_MAX + 1];
static uint16_t line_len;
static uint8_t  line_drop;            /* 1：当前行超长，丢到换行为止 */
static uint16_t tx_seq;
static uint8_t  peer_seq_valid;

/* Private functions ---------------------------------------------------------*/
static uint16_t dl20_make_id(void)
{
	const uint32_t *uid = (const uint32_t *)UID_BASE;
	uint32_t mix = uid[0] ^ uid[1] ^ uid[2];

	mix ^= mix >> 16;
	if ((mix & 0xFFFFu) == 0u)
	{
		mix = 1u;   /* 0 保留为“无对端” */
	}
	return (uint16_t)mix;
}

static uint8_t dl20_xor(const char *start, const char *end)
{
	uint8_t cs = 0;

	while (start < end)
	{
		cs ^= (uint8_t)*start++;
	}
	return cs;
}

static void dl20_send(const char *buf, uint16_t len)
{
	/* NOTE: 阻塞发送，30 B 约 2.6 ms，测试工程可接受 */
	HAL_UART_Transmit(&huart1, (uint8_t *)buf, len, 10);
}

static void dl20_send_frame(void)
{
	char body[40];
	char frame[48];
	int  n;

	n = snprintf(body, sizeof(body), "DL20,%04X,%u,%u",
	             dl20_stat.self_id, tx_seq, dl20_stat.peer_last_seq);
	if (n <= 0)
	{
		return;
	}
	n = snprintf(frame, sizeof(frame), "$%s*%02X\r\n",
	             body, dl20_xor(body, body + n));
	if (n > 0)
	{
		dl20_send(frame, (uint16_t)n);
		dl20_stat.tx_frames++;
		tx_seq++;
		LED_BLUE_Toggle;
	}
}

static void dl20_mark_valid(uint32_t now)
{
	dl20_stat.last_valid_ms = now;
	LED_GREEN_Toggle;
}

/**
 * @brief  解析一行 $DL20 帧，成功返回 1。
 */
static uint8_t dl20_parse_frame(const char *line, uint32_t now)
{
	const char   *star = strchr(line, '*');
	unsigned int  id, seq, ack;
	char         *end;
	unsigned long cs;
	uint16_t      gap;

	if ((star == NULL) || (strlen(star) != 3u))
	{
		return 0;
	}
	cs = strtoul(star + 1, &end, 16);
	if ((end != star + 3) || ((uint8_t)cs != dl20_xor(line + 1, star)))
	{
		return 0;
	}
	if (sscanf(line, "$DL20,%4x,%u,%u*", &id, &seq, &ack) != 3)
	{
		return 0;
	}

	if ((uint16_t)id == dl20_stat.self_id)
	{
		dl20_stat.self_frames++;   /* 自收：TX/RX 短接或回环 */
		return 1;
	}

	if (peer_seq_valid && ((uint16_t)id == dl20_stat.peer_id))
	{
		gap = (uint16_t)((uint16_t)seq - dl20_stat.peer_last_seq);
		if (gap == 0u)
		{
			dl20_stat.peer_dup++;
		}
		else if (gap < 0x8000u)
		{
			dl20_stat.peer_lost += (uint32_t)(gap - 1u);
		}
		/* gap 过大视为对端复位，不计丢帧 */
	}
	dl20_stat.peer_id       = (uint16_t)id;
	dl20_stat.peer_last_seq = (uint16_t)seq;
	dl20_stat.peer_ack      = (uint16_t)ack;
	dl20_stat.peer_frames++;
	peer_seq_valid = 1;
	dl20_mark_valid(now);
	return 1;
}

static void dl20_handle_line(const char *line, uint32_t now)
{
	char reply[DL20_LINE_MAX + 24];
	int  n;

	if (line[0] == '\0')
	{
		return;
	}
	if (strncmp(line, "$DL20,", 6) == 0)
	{
		if (!dl20_parse_frame(line, now))
		{
			dl20_stat.bad_frames++;
		}
		return;
	}
	/* NOTE: 不回显 ECHO 行，防止双板互相回显死循环 */
	if (strncmp(line, "ECHO", 4) == 0)
	{
		return;
	}
	n = snprintf(reply, sizeof(reply), "ECHO,%04X:%s\r\n",
	             dl20_stat.self_id, line);
	if (n > 0)
	{
		dl20_send(reply, (uint16_t)((n < (int)sizeof(reply)) ? n : (int)sizeof(reply) - 1));
		dl20_stat.pc_lines++;
		dl20_mark_valid(now);
	}
}

static void dl20_poll_rx(uint32_t now)
{
	uint8_t ch;

	while (ring_tail != ring_head)
	{
		ch = ring_buf[ring_tail];
		ring_tail = (uint16_t)((ring_tail + 1u) & (DL20_RING_SIZE - 1u));

		if (ch == '\n')
		{
			if (!line_drop)
			{
				if ((line_len > 0u) && (line_buf[line_len - 1u] == '\r'))
				{
					line_len--;
				}
				line_buf[line_len] = '\0';
				dl20_handle_line(line_buf, now);
			}
			line_len  = 0;
			line_drop = 0;
		}
		else if (line_drop)
		{
			/* 丢弃到换行为止 */
		}
		else if (line_len < DL20_LINE_MAX)
		{
			line_buf[line_len++] = (char)ch;
		}
		else
		{
			line_drop = 1;
			dl20_stat.line_overflow++;
		}
	}
}

static void dl20_print_stat(void)
{
	/* NOTE: CDC 异步发送，缓冲须为静态 */
	static char buf[200];
	int  n;
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;

	/* WARNING: 未枚举时 pClassData 为空，直接发送会访问空指针。 */
	if ((hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED) || (hcdc == NULL) || (hcdc->TxState != 0u))
	{
		return;
	}

	n = snprintf(buf, sizeof(buf),
	             "id=%04X peer=%04X link=%u tx=%lu peer_rx=%lu lost=%lu dup=%lu "
	             "ack=%u pc=%lu bad=%lu self=%lu ovf=%lu/%lu bytes=%lu\r\n",
	             dl20_stat.self_id, dl20_stat.peer_id, dl20_stat.link_ok,
	             (unsigned long)dl20_stat.tx_frames, (unsigned long)dl20_stat.peer_frames,
	             (unsigned long)dl20_stat.peer_lost, (unsigned long)dl20_stat.peer_dup,
	             dl20_stat.peer_ack, (unsigned long)dl20_stat.pc_lines,
	             (unsigned long)dl20_stat.bad_frames, (unsigned long)dl20_stat.self_frames,
	             (unsigned long)dl20_stat.line_overflow, (unsigned long)dl20_stat.ring_overflow,
	             (unsigned long)dl20_stat.rx_bytes);
	if (n > 0)
	{
		CDC_Transmit_FS((uint8_t *)buf, (uint16_t)((n < (int)sizeof(buf)) ? n : (int)sizeof(buf) - 1));
	}
}

/* Exported functions --------------------------------------------------------*/
/**
 * @brief  USART1 空闲中断回调（覆盖 drv_uart.c 中的弱定义），在中断中运行。
 * @note   drv_uart.c 在调用本函数前已关闭 DMA、调用后才重置计数，
 *         所以此处 NDTR 仍是本次剩余量。只做拷贝，不解析。
 */
void USART1_rxDataHandler(uint8_t *rxBuf)
{
	uint16_t len = (uint16_t)(USART1_RX_BUF_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx));
	uint16_t i;
	uint16_t next;

	if (len > USART1_RX_BUF_LEN)
	{
		return;
	}
	for (i = 0; i < len; i++)
	{
		next = (uint16_t)((ring_head + 1u) & (DL20_RING_SIZE - 1u));
		if (next == ring_tail)
		{
			dl20_stat.ring_overflow++;
			continue;
		}
		ring_buf[ring_head] = rxBuf[i];
		ring_head = next;
	}
	dl20_stat.rx_bytes += len;
}

void StartCommunityTask(void const *argument)
{
	uint32_t now;
	uint32_t last_tx_ms;
	uint32_t last_stat_ms;

	dl20_stat.self_id = dl20_make_id();
	LED_RED_ON;
	LED_GREEN_OFF;
	LED_BLUE_OFF;

	now          = HAL_GetTick();
	last_tx_ms   = now;
	last_stat_ms = now;
	dl20_stat.last_valid_ms = now - DL20_LINK_TIMEOUT_MS;

	for (;;)
	{
		now = HAL_GetTick();

		dl20_poll_rx(now);

		if (now - last_tx_ms >= DL20_TX_PERIOD_MS)
		{
			last_tx_ms += DL20_TX_PERIOD_MS;
			dl20_send_frame();
		}

		dl20_stat.link_ok = (uint8_t)((now - dl20_stat.last_valid_ms) < DL20_LINK_TIMEOUT_MS);
		if (dl20_stat.link_ok)
		{
			LED_RED_OFF;
		}
		else
		{
			LED_RED_ON;
		}

		if (now - last_stat_ms >= DL20_STAT_PERIOD_MS)
		{
			last_stat_ms += DL20_STAT_PERIOD_MS;
			dl20_print_stat();
		}

		osDelay(1);
	}
}
