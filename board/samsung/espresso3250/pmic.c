/*
 * (C) Copyright 2011 Samsung Electronics Co. Ltd
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <common.h>
#include "pmic.h"
#include "smdk3250_val.h"

void Delay(void)
{
	unsigned long i;
	for(i=0;i<DELAY;i++);
}

void IIC0_SCLH_SDAH(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLH_SDAL(void)
{
	IIC0_ESCL_Hi;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_SCLL_SDAH(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	Delay();
}

void IIC0_SCLL_SDAL(void)
{
	IIC0_ESCL_Lo;
	IIC0_ESDA_Lo;
	Delay();
}

void IIC0_ELow(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLH_SDAL();
	IIC0_SCLL_SDAL();
}

void IIC0_EHigh(void)
{
	IIC0_SCLL_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAH();
	IIC0_SCLL_SDAH();
}

void IIC0_EStart(void)
{
	IIC0_SCLH_SDAH();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLL_SDAL();
}

void IIC0_EEnd(void)
{
	IIC0_SCLL_SDAL();
	IIC0_SCLH_SDAL();
	Delay();
	IIC0_SCLH_SDAH();
}

void IIC0_EAck_write(void)
{
	unsigned long ack;

	IIC0_ESDA_INP;			// Function <- Input

	IIC0_ESCL_Lo;
	Delay();
	IIC0_ESCL_Hi;
	Delay();
	ack = GPD1DAT;
	IIC0_ESCL_Hi;
	Delay();
	IIC0_ESCL_Hi;
	Delay();

	IIC0_ESDA_OUTP;			// Function <- Output (SDA)

	ack = (ack>>0)&0x1;
//	while(ack!=0);

	IIC0_SCLL_SDAL();
}

void IIC0_EAck_read(void)
{
	IIC0_ESDA_OUTP;			// Function <- Output

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	IIC0_ESDA_Hi;
	IIC0_ESCL_Hi;
	IIC0_ESCL_Hi;

	IIC0_ESDA_INP;			// Function <- Input (SDA)

	IIC0_SCLL_SDAL();
}

void IIC0_ESetport(void)
{
	GPD1PUD &= ~(0xf<<0);	// Pull Up/Down Disable	SCL, SDA

	IIC0_ESCL_Hi;
	IIC0_ESDA_Hi;

	IIC0_ESCL_OUTP;		// Function <- Output (SCL)
	IIC0_ESDA_OUTP;		// Function <- Output (SDA)

	Delay();
}

void IIC0_EWrite (unsigned char ChipId, unsigned char IicAddr, unsigned char IicData)
{
	unsigned long i;

	IIC0_EStart();

////////////////// write chip id //////////////////
	for(i = 7; i>0; i--)
	{
		if((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_ELow();	// write

	IIC0_EAck_write();	// ACK

////////////////// write reg. addr. //////////////////
	for(i = 8; i>0; i--)
	{
		if((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EAck_write();	// ACK

////////////////// write reg. data. //////////////////
	for(i = 8; i>0; i--)
	{
		if((IicData >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EAck_write();	// ACK

	IIC0_EEnd();
}

void IIC0_ERead (unsigned char ChipId, unsigned char IicAddr, unsigned char *IicData)
{
	unsigned long i, reg;
	unsigned char data = 0;

	IIC0_EStart();

////////////////// write chip id //////////////////
	for(i = 7; i>0; i--)
	{
		if((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_ELow();	// write

	IIC0_EAck_write();	// ACK

////////////////// write reg. addr. //////////////////
	for(i = 8; i>0; i--)
	{
		if((IicAddr >> (i-1)) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EAck_write();	// ACK

	IIC0_EStart();

////////////////// write chip id //////////////////
	for(i = 7; i>0; i--)
	{
		if((ChipId >> i) & 0x0001)
			IIC0_EHigh();
		else
			IIC0_ELow();
	}

	IIC0_EHigh();	// read

	IIC0_EAck_write();	// ACK

////////////////// read reg. data. //////////////////
	IIC0_ESDA_INP;

	IIC0_ESCL_Lo;
	IIC0_ESCL_Lo;
	Delay();

	for(i = 8; i>0; i--)
	{
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		reg = GPD1DAT;
		IIC0_ESCL_Hi;
		IIC0_ESCL_Hi;
		Delay();
		IIC0_ESCL_Lo;
		IIC0_ESCL_Lo;
		Delay();
		reg = (reg >> 0) & 0x1;

		data |= reg << (i-1);
	}

	IIC0_EAck_read();	// ACK
	IIC0_ESDA_OUTP;

	IIC0_EEnd();

	*IicData = data;
}

void pmic_init(void)
{
	unsigned char rtc_ctrl;
	unsigned char wrstbi_ctrl;

	IIC0_ESetport();

	/* enable low_jitter, CP, AP at RTC_BUF */
	IIC0_EWrite(S2MPS14_WR_ADDR, WRSTBI, 0x20);
	IIC0_EWrite(S2MPS14_WR_ADDR, RTC_BUF, 0x17);
	IIC0_EWrite(S2MPS14_WR_ADDR, BUCK2_OUT,
			WR_BUCK_VOLT(CONFIG_ARM_VOLT) + VDD_BASE_OFFSET);

}

void pmic_enable_wtsr()
{
	unsigned char buf = 0;
	IIC0_ESetport();

	IIC0_ERead(S2MPS14_RTC_RD_ADDR, RTC_WTSR_SMPL, &buf);
	buf |= WTSREN;
	buf |= WTSRT;
	IIC0_EWrite(S2MPS14_RTC_WR_ADDR, RTC_WTSR_SMPL, buf);
}

void pmic_enable_peric_dev(void)
{
	unsigned char ldo_ctrl;

	IIC0_ESetport();

	/* enable LDO18 : VCC_2.8V_PERI */
	IIC0_ERead(S2MPS14_WR_ADDR, 0x34, &ldo_ctrl);
	ldo_ctrl |= (1 << 6);
	IIC0_EWrite(S2MPS14_WR_ADDR, 0x34, ldo_ctrl);

	/* enable LDO23 : VCC_1.8V_PERI */
	IIC0_ERead(S2MPS14_WR_ADDR, 0x39, &ldo_ctrl);
	ldo_ctrl |= (1 << 6);
	IIC0_EWrite(S2MPS14_WR_ADDR, 0x39, ldo_ctrl);
}

#ifdef CONFIG_USE_LCD
void pmic_turnon_vdd_lcd(void)
{
	unsigned char ldo_ctrl;

	IIC0_ESetport();

	/* enable LDO16 : VCC_LCD_3.3V */
	IIC0_ERead(S2MPS14_WR_ADDR, 0x32, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS14_WR_ADDR, 0x32, ldo_ctrl);

	/* enable LDO19 : TSP_AVDD_1.8V */
	IIC0_ERead(S2MPS14_WR_ADDR, 0x35, &ldo_ctrl);
	ldo_ctrl |= OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS14_WR_ADDR, 0x35, ldo_ctrl);
}

void pmic_turnoff_vdd_lcd(void)
{
	unsigned char ldo_ctrl;

	IIC0_ESetport();

	/* disable LDO16 : VCC_LCD_3.3V */
	IIC0_ERead(S2MPS14_WR_ADDR, 0x32, &ldo_ctrl);
	ldo_ctrl &= ~OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS14_WR_ADDR, 0x32, ldo_ctrl);

	/* disable LDO19 : TSP_AVDD_1.8V */
	IIC0_ERead(S2MPS14_WR_ADDR, 0x35, &ldo_ctrl);
	ldo_ctrl &= ~OUTPUT_PWREN_ON;
	IIC0_EWrite(S2MPS14_WR_ADDR, 0x35, ldo_ctrl);
}
#endif
