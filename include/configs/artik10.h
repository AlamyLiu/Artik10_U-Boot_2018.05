/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2018 Cypress Semiconductor
 *
 * Configuration settings for the SAMSUNG ARTIK10 board.
 */

#ifndef __CONFIG_ARTIK10_H
#define __CONFIG_ARTIK10_H

#include <configs/exynos5420-common.h>
/*
#include <configs/exynos5-dt-common.h>
*/
#include <configs/exynos5-common.h>

#undef CONFIG_LCD
#undef CONFIG_EXYNOS_FB
#undef CONFIG_EXYNOS_DP

#undef CONFIG_KEYBOARD

#define CONFIG_BOARD_COMMON

#define CONFIG_SYS_SDRAM_BASE	0x40000000
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x1000000)

#define TZPC_BASE_OFFSET		0x10000

/* DRAM Memory Banks */
#define CONFIG_NR_DRAM_BANKS	8
#define SDRAM_BANK_SIZE		(256UL << 20UL)	/* 256 MB */
/* Reserve the last 22 MiB for the secure firmware */
#define CONFIG_SYS_MEM_TOP_HIDE		(22UL << 20UL)
#define CONFIG_TZSW_RESERVED_DRAM_SIZE	CONFIG_SYS_MEM_TOP_HIDE

/* FIXME: MUST BE DISABLED AFTER TMU IS TURNED ON (exynos5-common.h)*/
#undef CONFIG_EXYNOS_TMU

/* FLASH and environment organization */
#undef CONFIG_CMD_IMLS

/* Environment variables */
#undef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_SIZE
#define CONFIG_ENV_OFFSET		(0x99E00)
#define CONFIG_ENV_SIZE			(SZ_1K * 16)


/* ----- ARTIK Common ----- */

/* USB gadget */
#define CONFIG_USB_GADGET_VBUS_DRAW	2

/* THOR */
#define CONFIG_G_DNL_THOR_VENDOR_NUM	CONFIG_USB_GADGET_VENDOR_NUM
#define CONFIG_G_DNL_THOR_PRODUCT_NUM	(0x685D)

/* UMS */
#define CONFIG_G_DNL_UMS_VENDOR_NUM	0x0525
#define CONFIG_G_DNL_UMS_PRODUCT_NUM	0xA4A5


#endif	/* __CONFIG_ARTIK10_H */
