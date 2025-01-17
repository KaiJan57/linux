/*
 * linux/arch/arm/mach-tegra/pinmux-t2-tables.c
 *
 * Common pinmux configurations for Tegra 2 SoCs
 *
 * Copyright (C) 2010 NVIDIA Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/of.h>
#include <linux/spinlock.h>
#include <linux/syscore_ops.h>
#include <linux/io.h>

#undef  CONFIG_ARCH_TEGRA_2x_SOC
#define CONFIG_ARCH_TEGRA_2x_SOC

#include "gpio-names.h"
#include "iomap.h"

#include "pinmux-t2.h"
#include "pinmux.h"

#define SET_DRIVE_PINGROUP(pg_name, r, drv_down_offset, drv_down_mask, drv_up_offset, drv_up_mask,	\
	slew_rise_offset, slew_rise_mask, slew_fall_offset, slew_fall_mask)	\
	[TEGRA_DRIVE_PINGROUP_ ## pg_name] = {		\
		.name = #pg_name,			\
		.reg = r,				\
		.drvup_offset = drv_up_offset,		\
		.drvup_mask = drv_up_mask,		\
		.drvdown_offset = drv_down_offset,	\
		.drvdown_mask = drv_down_mask,		\
		.slewrise_offset = slew_rise_offset,	\
		.slewrise_mask = slew_rise_mask,	\
		.slewfall_offset = slew_fall_offset,	\
		.slewfall_mask = slew_fall_mask,	\
	}

#define DEFAULT_DRIVE_PINGROUP(pg_name, r)		\
	[TEGRA_DRIVE_PINGROUP_ ## pg_name] = {		\
		.name = #pg_name,			\
		.reg = r,				\
		.drvup_offset = 20,			\
		.drvup_mask = 0x1f,			\
		.drvdown_offset = 12,			\
		.drvdown_mask = 0x1f,			\
		.slewrise_offset = 28,			\
		.slewrise_mask = 0x3,			\
		.slewfall_offset = 30,			\
		.slewfall_mask = 0x3,			\
	}

static const struct tegra_drive_pingroup_desc tegra_soc_drive_pingroups[TEGRA_MAX_DRIVE_PINGROUP] = {
	DEFAULT_DRIVE_PINGROUP(AO1,		0x868),
	DEFAULT_DRIVE_PINGROUP(AO2,		0x86c),
	DEFAULT_DRIVE_PINGROUP(AT1,		0x870),
	DEFAULT_DRIVE_PINGROUP(AT2,		0x874),
	DEFAULT_DRIVE_PINGROUP(CDEV1,		0x878),
	DEFAULT_DRIVE_PINGROUP(CDEV2,		0x87c),
	DEFAULT_DRIVE_PINGROUP(CSUS,		0x880),
	DEFAULT_DRIVE_PINGROUP(DAP1,		0x884),
	DEFAULT_DRIVE_PINGROUP(DAP2,		0x888),
	DEFAULT_DRIVE_PINGROUP(DAP3,		0x88c),
	DEFAULT_DRIVE_PINGROUP(DAP4,		0x890),
	DEFAULT_DRIVE_PINGROUP(DBG,		0x894),
	DEFAULT_DRIVE_PINGROUP(LCD1,		0x898),
	DEFAULT_DRIVE_PINGROUP(LCD2,		0x89c),
	DEFAULT_DRIVE_PINGROUP(SDMMC2,		0x8a0),
	DEFAULT_DRIVE_PINGROUP(SDMMC3,		0x8a4),
	DEFAULT_DRIVE_PINGROUP(SPI,		0x8a8),
	DEFAULT_DRIVE_PINGROUP(UAA,		0x8ac),
	DEFAULT_DRIVE_PINGROUP(UAB,		0x8b0),
	DEFAULT_DRIVE_PINGROUP(UART2,		0x8b4),
	DEFAULT_DRIVE_PINGROUP(UART3,		0x8b8),
	DEFAULT_DRIVE_PINGROUP(VI1,		0x8bc),
	DEFAULT_DRIVE_PINGROUP(VI2,		0x8c0),
	DEFAULT_DRIVE_PINGROUP(XM2A,		0x8c4),
	DEFAULT_DRIVE_PINGROUP(XM2C,		0x8c8),
	DEFAULT_DRIVE_PINGROUP(XM2D,		0x8cc),
	DEFAULT_DRIVE_PINGROUP(XM2CLK,		0x8d0),
	DEFAULT_DRIVE_PINGROUP(MEMCOMP,		0x8d4),
	DEFAULT_DRIVE_PINGROUP(SDIO1,		0x8e0),
	DEFAULT_DRIVE_PINGROUP(CRT,		0x8ec),
	DEFAULT_DRIVE_PINGROUP(DDC,		0x8f0),
	DEFAULT_DRIVE_PINGROUP(GMA,		0x8f4),
	DEFAULT_DRIVE_PINGROUP(GMB,		0x8f8),
	DEFAULT_DRIVE_PINGROUP(GMC,		0x8fc),
	DEFAULT_DRIVE_PINGROUP(GMD,		0x900),
	DEFAULT_DRIVE_PINGROUP(GME,		0x904),
	DEFAULT_DRIVE_PINGROUP(OWR,		0x908),
	DEFAULT_DRIVE_PINGROUP(UAD,		0x90c),
};

#define PINGROUP(pg_name, gpio_nr, vdd, f0, f1, f2, f3, f_safe,		\
		 tri_r, tri_b, mux_r, mux_b, pupd_r, pupd_b)	\
	[TEGRA_PINGROUP_ ## pg_name] = {			\
		.name = #pg_name,				\
		.vddio = TEGRA_VDDIO_ ## vdd,			\
		.funcs = {					\
			TEGRA_MUX_ ## f0,			\
			TEGRA_MUX_ ## f1,			\
			TEGRA_MUX_ ## f2,			\
			TEGRA_MUX_ ## f3,			\
		},								\
		.gpionr =	TEGRA_GPIO_ ## gpio_nr,		\
		.func_safe = TEGRA_MUX_ ## f_safe,		\
		.tri_reg = tri_r,				\
		.tri_bit = tri_b,				\
		.mux_reg = mux_r,				\
		.mux_bit = mux_b,				\
		.pupd_reg = pupd_r,				\
		.pupd_bit = pupd_b,				\
		.io_default = 0,				\
		.od_bit = -1,					\
		.lock_bit = -1,					\
		.ioreset_bit = -1,				\
}

#define PINGROUPS	\
	/*	pg_name,gpio_nr,	vdd,	f0,		f1,		f2,		f3,		f_safe,		tri_r,	tri_b,	mux_r,	mux_b,	pupd_r,	pupd_b*/\
	PINGROUP(ATA,	PI3,		NAND,	IDE,		NAND,		GMI,		RSVD,		IDE,		0x14,	0,	0x80,	24,	0xA0,	0),\
	PINGROUP(ATB,	PI2,		NAND,	IDE,		NAND,		GMI,		SDIO4,		IDE,		0x14,	1,	0x80,	16,	0xA0,	2),\
	PINGROUP(ATC,	PI5,		NAND,	IDE,		NAND,		GMI,		SDIO4,		IDE,		0x14,	2,	0x80,	22,	0xA0,	4),\
	PINGROUP(ATD,	PH0,		NAND,	IDE,		NAND,		GMI,		SDIO4,		IDE,		0x14,	3,	0x80,	20,	0xA0,	6),\
	PINGROUP(ATE,	PH4,		NAND,	IDE,		NAND,		GMI,		RSVD,		IDE,		0x18,	25,	0x80,	12,	0xA0,	8),\
	PINGROUP(CDEV1,	PW4,		AUDIO,	OSC,		PLLA_OUT,	PLLM_OUT1,	AUDIO_SYNC,	OSC,		0x14,	4,	0x88,	2,	0xA8,	0),\
	PINGROUP(CDEV2,	PW5,		AUDIO,	OSC,		AHB_CLK,	APB_CLK,	PLLP_OUT4,	OSC,		0x14,	5,	0x88,	4,	0xA8,	2),\
	PINGROUP(CRTP,	INVALID,	LCD,	CRT,		RSVD,		RSVD,		RSVD,		RSVD,		0x20,	14,	0x98,	20,	0xA4,	24),\
	PINGROUP(CSUS,	PT1,		VI,	PLLC_OUT1,	PLLP_OUT2,	PLLP_OUT3,	VI_SENSOR_CLK,	PLLC_OUT1,	0x14,	6,	0x88,	6,	0xAC,	24),\
	PINGROUP(DAP1,	PN0,		AUDIO,	DAP1,		RSVD,		GMI,		SDIO2,		DAP1,		0x14,	7,	0x88,	20,	0xA0,	10),\
	PINGROUP(DAP2,	PA2,		AUDIO,	DAP2,		TWC,		RSVD,		GMI,		DAP2,		0x14,	8,	0x88,	22,	0xA0,	12),\
	PINGROUP(DAP3,	PP0,		BB,	DAP3,		RSVD,		RSVD,		RSVD,		DAP3,		0x14,	9,	0x88,	24,	0xA0,	14),\
	PINGROUP(DAP4,	PP4,		UART,	DAP4,		RSVD,		GMI,		RSVD,		DAP4,		0x14,	10,	0x88,	26,	0xA0,	16),\
	PINGROUP(DDC,	INVALID,	LCD,	I2C2,		RSVD,		RSVD,		RSVD,		RSVD,		0x18,	31,	0x88,	0,	0xB0,	28),\
	PINGROUP(DTA,	PT4,		VI,	RSVD,		SDIO2,		VI,		RSVD,		RSVD4,		0x14,	11,	0x84,	20,	0xA0,	18),\
	PINGROUP(DTB,	PT2,		VI,	RSVD,		RSVD,		VI,		SPI1,		RSVD1,		0x14,	12,	0x84,	22,	0xA0,	20),\
	PINGROUP(DTC,	PD6,		VI,	RSVD,		RSVD,		VI,		RSVD,		RSVD1,		0x14,	13,	0x84,	26,	0xA0,	22),\
	PINGROUP(DTD,	PT0,		VI,	RSVD,		SDIO2,		VI,		RSVD,		RSVD1,		0x14,	14,	0x84,	28,	0xA0,	24),\
	PINGROUP(DTE,	PBB1,		VI,	RSVD,		RSVD,		VI,		SPI1,		RSVD1,		0x14,	15,	0x84,	30,	0xA0,	26),\
	PINGROUP(DTF,	PBB2,		VI,	I2C3,		RSVD,		VI,		RSVD,		RSVD4,		0x20,	12,	0x98,	30,	0xA0,	28),\
	PINGROUP(GMA,	PAA0,		NAND,	UARTE,		SPI3,		GMI,		SDIO4,		SPI3,		0x14,	28,	0x84,	0,	0xB0,	20),\
	PINGROUP(GMB,	PC7,		NAND,	IDE,		NAND,		GMI,		GMI_INT,	GMI,		0x18,	29,	0x88,	28,	0xB0,	22),\
	PINGROUP(GMC,	PJ7,		NAND,	UARTD,		SPI4,		GMI,		SFLASH,		SPI4,		0x14,	29,	0x84,	2,	0xB0,	24),\
	PINGROUP(GMD,	PJ0,		NAND,	RSVD,		NAND,		GMI,		SFLASH,		GMI,		0x18,	30,	0x88,	30,	0xB0,	26),\
	PINGROUP(GME,	PAA4,		NAND,	RSVD,		DAP5,		GMI,		SDIO4,		GMI,		0x18,	0,	0x8C,	0,	0xA8,	24),\
	PINGROUP(GPU,	PU0,		UART,	PWM,		UARTA,		GMI,		RSVD,		RSVD4,		0x14,	16,	0x8C,	4,	0xA4,	20),\
	PINGROUP(GPU7,	PU7,		SYS,	RTCK,		RSVD,		RSVD,		RSVD,		RTCK,		0x20,	11,	0x98,	28,	0xA4,	6),\
	PINGROUP(GPV,	PV4,		SD,	PCIE,		RSVD,		RSVD,		RSVD,		PCIE,		0x14,	17,	0x8C,	2,	0xA0,	30),\
	PINGROUP(HDINT,	PN7,		LCD,	HDMI,		RSVD,		RSVD,		RSVD,		HDMI,		0x1C,	23,	0x84,	4,	0xAC,	22),\
	PINGROUP(I2CP,	PZ6,		SYS,	I2C,		RSVD,		RSVD,		RSVD,		RSVD4,		0x14,	18,	0x88,	8,	0xA4,	2),\
	PINGROUP(IRRX,	PJ6,		UART,	UARTA,		UARTB,		GMI,		SPI4,		UARTB,		0x14,	20,	0x88,	18,	0xA8,	22),\
	PINGROUP(IRTX,	PJ5,		UART,	UARTA,		UARTB,		GMI,		SPI4,		UARTB,		0x14,	19,	0x88,	16,	0xA8,	20),\
	PINGROUP(KBCA,	PR0,		SYS,	KBC,		NAND,		SDIO2,		EMC_TEST0_DLL,	KBC,		0x14,	22,	0x88,	10,	0xA4,	8),\
	PINGROUP(KBCB,	PR7,		SYS,	KBC,		NAND,		SDIO2,		MIO,		KBC,		0x14,	21,	0x88,	12,	0xA4,	10),\
	PINGROUP(KBCC,	PQ0,		SYS,	KBC,		NAND,		TRACE,		EMC_TEST1_DLL,	KBC,		0x18,	26,	0x88,	14,	0xA4,	12),\
	PINGROUP(KBCD,	PR3,		SYS,	KBC,		NAND,		SDIO2,		MIO,		KBC,		0x20,	10,	0x98,	26,	0xA4,	14),\
	PINGROUP(KBCE,	PQ7,		SYS,	KBC,		NAND,		OWR,		RSVD,		KBC,		0x14,	26,	0x80,	28,	0xB0,	2),\
	PINGROUP(KBCF,	PQ2,		SYS,	KBC,		NAND,		TRACE,		MIO,		KBC,		0x14,	27,	0x80,	26,	0xB0,	0),\
	PINGROUP(LCSN,	PN4,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		RSVD,		RSVD4,		0x1C,	31,	0x90,	12,	0xAC,	20),\
	PINGROUP(LD0,	PE0,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	0,	0x94,	0,	0xAC,	12),\
	PINGROUP(LD1,	PE1,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	1,	0x94,	2,	0xAC,	12),\
	PINGROUP(LD10,	PF2,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	10,	0x94,	20,	0xAC,	12),\
	PINGROUP(LD11,	PF3,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	11,	0x94,	22,	0xAC,	12),\
	PINGROUP(LD12,	PF4,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	12,	0x94,	24,	0xAC,	12),\
	PINGROUP(LD13,	PF5,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	13,	0x94,	26,	0xAC,	12),\
	PINGROUP(LD14,	PF6,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	14,	0x94,	28,	0xAC,	12),\
	PINGROUP(LD15,	PF7,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	15,	0x94,	30,	0xAC,	12),\
	PINGROUP(LD16,	PM0,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	16,	0x98,	0,	0xAC,	12),\
	PINGROUP(LD17,	PM1,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x1C,	17,	0x98,	2,	0xAC,	12),\
	PINGROUP(LD2,	PE2,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	2,	0x94,	4,	0xAC,	12),\
	PINGROUP(LD3,	PE3,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	3,	0x94,	6,	0xAC,	12),\
	PINGROUP(LD4,	PE4,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	4,	0x94,	8,	0xAC,	12),\
	PINGROUP(LD5,	PE5,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	5,	0x94,	10,	0xAC,	12),\
	PINGROUP(LD6,	PE6,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	6,	0x94,	12,	0xAC,	12),\
	PINGROUP(LD7,	PE7,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	7,	0x94,	14,	0xAC,	12),\
	PINGROUP(LD8,	PF0,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	8,	0x94,	16,	0xAC,	12),\
	PINGROUP(LD9,	PF1,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	9,	0x94,	18,	0xAC,	12),\
	PINGROUP(LDC,	PN6,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x1C,	30,	0x90,	14,	0xAC,	20),\
	PINGROUP(LDI,	PM6,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x20,	6,	0x98,	16,	0xAC,	18),\
	PINGROUP(LHP0,	PM5,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x1C,	18,	0x98,	10,	0xAC,	16),\
	PINGROUP(LHP1,	PM2,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x1C,	19,	0x98,	4,	0xAC,	14),\
	PINGROUP(LHP2,	PM3,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x1C,	20,	0x98,	6,	0xAC,	14),\
	PINGROUP(LHS,	PJ3,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x20,	7,	0x90,	22,	0xAC,	22),\
	PINGROUP(LM0,	PW0,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		RSVD,		RSVD4,		0x1C,	24,	0x90,	26,	0xAC,	22),\
	PINGROUP(LM1,	PW1,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		CRT,		RSVD3,		0x1C,	25,	0x90,	28,	0xAC,	22),\
	PINGROUP(LPP,	PM7,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x20,	8,	0x98,	14,	0xAC,	18),\
	PINGROUP(LPW0,	PB2,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		HDMI,		DISPLAYA,	0x20,	3,	0x90,	0,	0xAC,	20),\
	PINGROUP(LPW1,	PC1,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x20,	4,	0x90,	2,	0xAC,	20),\
	PINGROUP(LPW2,	PC6,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		HDMI,		DISPLAYA,	0x20,	5,	0x90,	4,	0xAC,	20),\
	PINGROUP(LSC0,	PB3,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	27,	0x90,	18,	0xAC,	22),\
	PINGROUP(LSC1,	PZ3,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		HDMI,		DISPLAYA,	0x1C,	28,	0x90,	20,	0xAC,	20),\
	PINGROUP(LSCK,	PZ4,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		HDMI,		DISPLAYA,	0x1C,	29,	0x90,	16,	0xAC,	20),\
	PINGROUP(LSDA,	PN5,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		HDMI,		DISPLAYA,	0x20,	1,	0x90,	8,	0xAC,	20),\
	PINGROUP(LSDI,	PZ2,		LCD,	DISPLAYA,	DISPLAYB,	SPI3,		RSVD,		DISPLAYA,	0x20,	2,	0x90,	6,	0xAC,	20),\
	PINGROUP(LSPI,	PJ1,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		HDMI,		DISPLAYA,	0x20,	0,	0x90,	10,	0xAC,	22),\
	PINGROUP(LVP0,	PV7,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x1C,	21,	0x90,	30,	0xAC,	22),\
	PINGROUP(LVP1,	PM4,		LCD,	DISPLAYA,	DISPLAYB,	RSVD,		RSVD,		RSVD4,		0x1C,	22,	0x98,	8,	0xAC,	16),\
	PINGROUP(LVS,	PJ4,		LCD,	DISPLAYA,	DISPLAYB,	XIO,		RSVD,		RSVD4,		0x1C,	26,	0x90,	24,	0xAC,	22),\
	PINGROUP(OWC,	INVALID,	SYS,	OWR,		RSVD,		RSVD,		RSVD,		OWR,		0x14,	31,	0x84,	8,	0xB0,	30),\
	PINGROUP(PMC,	PBB0,		SYS,	PWR_ON,		PWR_INTR,	RSVD,		RSVD,		PWR_ON,		0x14,	23,	0x98,	18,	-1,	-1),\
	PINGROUP(PTA,	PT5,		NAND,	I2C2,		HDMI,		GMI,		RSVD,		RSVD,		0x14,	24,	0x98,	22,	0xA4,	4),\
	PINGROUP(RM,	PC5,		UART,	I2C,		RSVD,		RSVD,		RSVD,		RSVD4,		0x14,	25,	0x80,	14,	0xA4,	0),\
	PINGROUP(SDB,	PA7,		SD,	UARTA,		PWM,		SDIO3,		SPI2,		PWM,		0x20,	15,	0x8C,	10,	-1,	-1),\
	PINGROUP(SDC,	PB7,		SD,	PWM,		TWC,		SDIO3,		SPI3,		TWC,		0x18,	1,	0x8C,	12,	0xAC,	28),\
	PINGROUP(SDD,	PA6,		SD,	UARTA,		PWM,		SDIO3,		SPI3,		PWM,		0x18,	2,	0x8C,	14,	0xAC,	30),\
	PINGROUP(SDIO1,	PZ0,		BB,	SDIO1,		RSVD,		UARTE,		UARTA,		RSVD2,		0x14,	30,	0x80,	30,	0xB0,	18),\
	PINGROUP(SLXA,	PD1,		SD,	PCIE,		SPI4,		SDIO3,		SPI2,		PCIE,		0x18,	3,	0x84,	6,	0xA4,	22),\
	PINGROUP(SLXC,	PD3,		SD,	SPDIF,		SPI4,		SDIO3,		SPI2,		SPI4,		0x18,	5,	0x84,	10,	0xA4,	26),\
	PINGROUP(SLXD,	PD4,		SD,	SPDIF,		SPI4,		SDIO3,		SPI2,		SPI4,		0x18,	6,	0x84,	12,	0xA4,	28),\
	PINGROUP(SLXK,	PD0,		SD,	PCIE,		SPI4,		SDIO3,		SPI2,		PCIE,		0x18,	7,	0x84,	14,	0xA4,	30),\
	PINGROUP(SPDI,	PK6,		AUDIO,	SPDIF,		RSVD,		I2C,		SDIO2,		RSVD2,		0x18,	8,	0x8C,	8,	0xA4,	16),\
	PINGROUP(SPDO,	PK5,		AUDIO,	SPDIF,		RSVD,		I2C,		SDIO2,		RSVD2,		0x18,	9,	0x8C,	6,	0xA4,	18),\
	PINGROUP(SPIA,	PX0,		AUDIO,	SPI1,		SPI2,		SPI3,		GMI,		GMI,		0x18,	10,	0x8C,	30,	0xA8,	4),\
	PINGROUP(SPIB,	PX1,		AUDIO,	SPI1,		SPI2,		SPI3,		GMI,		GMI,		0x18,	11,	0x8C,	28,	0xA8,	6),\
	PINGROUP(SPIC,	PX2,		AUDIO,	SPI1,		SPI2,		SPI3,		GMI,		GMI,		0x18,	12,	0x8C,	26,	0xA8,	8),\
	PINGROUP(SPID,	PX4,		AUDIO,	SPI2,		SPI1,		SPI2_ALT,	GMI,		GMI,		0x18,	13,	0x8C,	24,	0xA8,	10),\
	PINGROUP(SPIE,	PX5,		AUDIO,	SPI2,		SPI1,		SPI2_ALT,	GMI,		GMI,		0x18,	14,	0x8C,	22,	0xA8,	12),\
	PINGROUP(SPIF,	PX7,		AUDIO,	SPI3,		SPI1,		SPI2,		RSVD,		RSVD4,		0x18,	15,	0x8C,	20,	0xA8,	14),\
	PINGROUP(SPIG,	PW2,		AUDIO,	SPI3,		SPI2,		SPI2_ALT,	I2C,		SPI2_ALT,	0x18,	16,	0x8C,	18,	0xA8,	16),\
	PINGROUP(SPIH,	PW3,		AUDIO,	SPI3,		SPI2,		SPI2_ALT,	I2C,		SPI2_ALT,	0x18,	17,	0x8C,	16,	0xA8,	18),\
	PINGROUP(UAA,	PO1,		BB,	SPI3,		MIPI_HS,	UARTA,		ULPI,		MIPI_HS,	0x18,	18,	0x80,	0,	0xAC,	0),\
	PINGROUP(UAB,	PO5,		BB,	SPI2,		MIPI_HS,	UARTA,		ULPI,		MIPI_HS,	0x18,	19,	0x80,	2,	0xAC,	2),\
	PINGROUP(UAC,	PV0,		BB,	OWR,		RSVD,		RSVD,		RSVD,		RSVD4,		0x18,	20,	0x80,	4,	0xAC,	4),\
	PINGROUP(UAD,	PC2,		UART,	IRDA,		SPDIF,		UARTA,		SPI4,		SPDIF,		0x18,	21,	0x80,	6,	0xAC,	6),\
	PINGROUP(UCA,	PW6,		UART,	UARTC,		RSVD,		GMI,		RSVD,		RSVD4,		0x18,	22,	0x84,	16,	0xAC,	8),\
	PINGROUP(UCB,	PC0,		UART,	UARTC,		PWM,		GMI,		RSVD,		RSVD4,		0x18,	23,	0x84,	18,	0xAC,	10),\
	PINGROUP(UDA,	PY0,		BB,	SPI1,		RSVD,		UARTD,		ULPI,		RSVD2,		0x20,	13,	0x80,	8,	0xB0,	16),\
	/* these pin groups only have pullup and pull down control */\
	PINGROUP(CK32,	INVALID,	SYS,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xB0,	14),\
	PINGROUP(DDRC,	INVALID,	DDR,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xAC,	26),\
	PINGROUP(PMCA,	INVALID,	SYS,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xB0,	4),\
	PINGROUP(PMCB,	INVALID,	SYS,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xB0,	6),\
	PINGROUP(PMCC,	INVALID,	SYS,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xB0,	8),\
	PINGROUP(PMCD,	INVALID,	SYS,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xB0,	10),\
	PINGROUP(PMCE,	INVALID,	SYS,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xB0,	12),\
	PINGROUP(XM2C,	INVALID,	DDR,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xA8,	30),\
	PINGROUP(XM2D,	INVALID,	DDR,	RSVD,		RSVD,		RSVD,		RSVD,		RSVD,		-1,	-1,	-1,	-1,	0xA8,	28),\
	/* END OF LIST */

static const struct tegra_pingroup_desc tegra_soc_pingroups[TEGRA_MAX_PINGROUP] = {
	PINGROUPS
};

#undef PINGROUP

#define PINGROUP(pg_name, gpio_nr, vdd, f0, f1, f2, f3, f_safe,		\
			 tri_r, tri_b, mux_r, mux_b, pupd_r, pupd_b)	\
	[TEGRA_GPIO_##gpio_nr] =  TEGRA_PINGROUP_ ##pg_name

static inline unsigned long pg_readl(unsigned long offset)
{
	return readl(IO_TO_VIRT(TEGRA_APB_MISC_BASE + offset));
}

#define HSM_EN(reg)	(((reg) >> 2) & 0x1)
#define SCHMT_EN(reg)	(((reg) >> 3) & 0x1)
#define LPMD(reg)	(((reg) >> 4) & 0x3)
#define DRVDN(reg, offset)	(((reg) >> offset) & 0x1f)
#define DRVUP(reg, offset)	(((reg) >> offset) & 0x1f)
#define SLWR(reg, offset)	(((reg) >> offset) & 0x3)
#define SLWF(reg, offset)	(((reg) >> offset) & 0x3)

static const struct tegra_pingroup_desc *const pingroups = tegra_soc_pingroups;
static const struct tegra_drive_pingroup_desc *const drive_pingroups = tegra_soc_drive_pingroups;

static char *tegra_mux_names[TEGRA_MAX_MUX] = {
#define TEGRA_MUX(mux) [TEGRA_MUX_##mux] = #mux,
	TEGRA_MUX_LIST
#undef  TEGRA_MUX
	[TEGRA_MUX_SAFE] = "<safe>",
};

static const char *tegra_drive_names[TEGRA_MAX_DRIVE] = {
	[TEGRA_DRIVE_DIV_8] = "DIV_8",
	[TEGRA_DRIVE_DIV_4] = "DIV_4",
	[TEGRA_DRIVE_DIV_2] = "DIV_2",
	[TEGRA_DRIVE_DIV_1] = "DIV_1",
};

static const char *tegra_slew_names[TEGRA_MAX_SLEW] = {
	[TEGRA_SLEW_FASTEST] = "FASTEST",
	[TEGRA_SLEW_FAST] = "FAST",
	[TEGRA_SLEW_SLOW] = "SLOW",
	[TEGRA_SLEW_SLOWEST] = "SLOWEST",
};

static const char *tri_name(unsigned long val)
{
	return val ? "TRISTATE" : "NORMAL";
}

static const char *pupd_name(unsigned long val)
{
	switch (val) {
	case 0:
		return "NORMAL";

	case 1:
		return "PULL_DOWN";

	case 2:
		return "PULL_UP";

	default:
		return "RSVD";
	}
}

#if defined(TEGRA_PINMUX_HAS_IO_DIRECTION)
static const char *io_name(unsigned long val)
{
	switch (val) {
	case 0:
		return "OUTPUT";

	case 1:
		return "INPUT";

	default:
		return "RSVD";
	}
}
#endif

static const char *drive_name(unsigned long val)
{
	if (val >= TEGRA_MAX_DRIVE)
		return "<UNKNOWN>";

	return tegra_drive_names[val];
}

static const char *slew_name(unsigned long val)
{
	if (val >= TEGRA_MAX_SLEW)
		return "<UNKNOWN>";

	return tegra_slew_names[val];
}

#ifdef	CONFIG_DEBUG_FS

#include <linux/debugfs.h>
#include <linux/seq_file.h>

static void dbg_pad_field(struct seq_file *s, int len)
{
	seq_putc(s, ',');

	while (len-- > -1)
		seq_putc(s, ' ');
}

static int dbg_pinmux_show(struct seq_file *s, void *unused)
{
	int i;
	int len;

	for (i = 0; i < TEGRA_MAX_PINGROUP; i++) {
		unsigned long tri;
		unsigned long mux;
		unsigned long pupd;

		seq_printf(s, "\t{TEGRA_PINGROUP_%s", pingroups[i].name);
		len = strlen(pingroups[i].name);
		dbg_pad_field(s, 15 - len);

		if (pingroups[i].mux_reg <= 0) {
			seq_printf(s, "TEGRA_MUX_NONE");
			len = strlen("NONE");
		} else {
			mux = (pg_readl(pingroups[i].mux_reg) >>
			       pingroups[i].mux_bit) & 0x3;
			BUG_ON(pingroups[i].funcs[mux] == 0);
			if (pingroups[i].funcs[mux] ==  TEGRA_MUX_INVALID) {
				seq_printf(s, "TEGRA_MUX_INVALID");
				len = 7;
			} else if (pingroups[i].funcs[mux] & TEGRA_MUX_RSVD) {
				seq_printf(s, "TEGRA_MUX_RSVD%1lu", mux+1);
				len = 5;
			} else {
				BUG_ON(!tegra_mux_names[pingroups[i].funcs[mux]]);
				seq_printf(s, "TEGRA_MUX_%s",
					   tegra_mux_names[pingroups[i].funcs[mux]]);
				len = strlen(tegra_mux_names[pingroups[i].funcs[mux]]);
			}
		}
		dbg_pad_field(s, 13-len);

#if defined(TEGRA_PINMUX_HAS_IO_DIRECTION)
		{
			unsigned long io;
			io = (pg_readl(pingroups[i].mux_reg) >> 5) & 0x1;
			seq_printf(s, "TEGRA_PIN_%s", io_name(io));
			len = strlen(io_name(io));
			dbg_pad_field(s, 6 - len);
		}
#endif
		if (pingroups[i].pupd_reg <= 0) {
			seq_printf(s, "TEGRA_PUPD_NORMAL");
			len = strlen("NORMAL");
		} else {
			pupd = (pg_readl(pingroups[i].pupd_reg) >>
				pingroups[i].pupd_bit) & 0x3;
			seq_printf(s, "TEGRA_PUPD_%s", pupd_name(pupd));
			len = strlen(pupd_name(pupd));
		}
		dbg_pad_field(s, 9 - len);

		if (pingroups[i].tri_reg <= 0) {
			seq_printf(s, "TEGRA_TRI_NORMAL");
		} else {
			tri = (pg_readl(pingroups[i].tri_reg) >>
			       pingroups[i].tri_bit) & 0x1;

			seq_printf(s, "TEGRA_TRI_%s", tri_name(tri));
		}
		seq_printf(s, "},\n");
	}
	return 0;
}

static int dbg_pinmux_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_pinmux_show, &inode->i_private);
}

static const struct file_operations debug_fops = {
	.open		= dbg_pinmux_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int dbg_drive_pinmux_show(struct seq_file *s, void *unused)
{
	int i;
	int len;
	u8 offset;

	for (i = 0; i < TEGRA_MAX_DRIVE_PINGROUP; i++) {
		u32 reg;

		seq_printf(s, "\t{TEGRA_DRIVE_PINGROUP_%s",
			drive_pingroups[i].name);
		len = strlen(drive_pingroups[i].name);
		dbg_pad_field(s, 7 - len);


		reg = pg_readl(drive_pingroups[i].reg);
		if (HSM_EN(reg)) {
			seq_printf(s, "TEGRA_HSM_ENABLE");
			len = 16;
		} else {
			seq_printf(s, "TEGRA_HSM_DISABLE");
			len = 17;
		}
		dbg_pad_field(s, 17 - len);

		if (SCHMT_EN(reg)) {
			seq_printf(s, "TEGRA_SCHMITT_ENABLE");
			len = 21;
		} else {
			seq_printf(s, "TEGRA_SCHMITT_DISABLE");
			len = 22;
		}
		dbg_pad_field(s, 22 - len);

		seq_printf(s, "TEGRA_DRIVE_%s", drive_name(LPMD(reg)));
		len = strlen(drive_name(LPMD(reg)));
		dbg_pad_field(s, 5 - len);

		offset = drive_pingroups[i].drvdown_offset;
		seq_printf(s, "TEGRA_PULL_%d", DRVDN(reg, offset));
		len = DRVDN(reg, offset) < 10 ? 1 : 2;
		dbg_pad_field(s, 2 - len);

		offset = drive_pingroups[i].drvup_offset;
		seq_printf(s, "TEGRA_PULL_%d", DRVUP(reg, offset));
		len = DRVUP(reg, offset) < 10 ? 1 : 2;
		dbg_pad_field(s, 2 - len);

		offset = drive_pingroups[i].slewrise_offset;
		seq_printf(s, "TEGRA_SLEW_%s", slew_name(SLWR(reg, offset)));
		len = strlen(slew_name(SLWR(reg, offset)));
		dbg_pad_field(s, 7 - len);

		offset= drive_pingroups[i].slewfall_offset;
		seq_printf(s, "TEGRA_SLEW_%s", slew_name(SLWF(reg, offset)));

		seq_printf(s, "},\n");
	}
	return 0;
}

static int dbg_drive_pinmux_open(struct inode *inode, struct file *file)
{
	return single_open(file, dbg_drive_pinmux_show, &inode->i_private);
}

static const struct file_operations debug_drive_fops = {
	.open		= dbg_drive_pinmux_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init tegra_pinmux_debuginit(void)
{
	if (!of_machine_is_compatible("nvidia,tegra20"))
		return 0;

	(void) debugfs_create_file("tegra_pinmux", S_IRUGO,
					NULL, NULL, &debug_fops);
	(void) debugfs_create_file("tegra_pinmux_drive", S_IRUGO,
					NULL, NULL, &debug_drive_fops);
	return 0;
}
late_initcall(tegra_pinmux_debuginit);
#endif
