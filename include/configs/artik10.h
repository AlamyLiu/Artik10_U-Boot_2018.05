/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2018 Cypress Semiconductor
 *
 * Configuration settings for the SAMSUNG ARTIK10 board.
 */

#ifndef __CONFIG_ARTIK10_H
#define __CONFIG_ARTIK10_H

#include <configs/exynos5420-common.h>
#include <configs/exynos5-dt-common.h>
#include <configs/exynos5-common.h>

#undef CONFIG_LCD
#undef CONFIG_EXYNOS_FB
#undef CONFIG_EXYNOS_DP

#undef CONFIG_KEYBOARD

#define CONFIG_BOARD_COMMON

#define CONFIG_ARTIK10			/* which is in a ARTIK10 */

#define CONFIG_SYS_SDRAM_BASE	0x40000000
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_LOAD_ADDR - 0x1000000)

/* select serial console configuration */
#define CONFIG_SERIAL3		/* use SERIAL 3 */
#define CONFIG_DEFAULT_CONSOLE	"console=ttySAC3,115200n8\0"

#define TZPC_BASE_OFFSET		0x10000

/* DRAM Memory Banks */
#define CONFIG_NR_DRAM_BANKS	8
#define SDRAM_BANK_SIZE		(256UL << 20UL)	/* 256 MB */

/* FLASH and environment organization */
#define CONFIG_SYS_NO_FLASH
#undef CONFIG_CMD_IMLS

#define CONFIG_ENV_IS_IN_MMC
#define CONFIG_SYS_MMC_ENV_DEV	0


/* ----- ARTIK Common ----- */

/* USB gadget */
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_VBUS_DRAW	2


#endif	/* __CONFIG_ARTIK10_H */
