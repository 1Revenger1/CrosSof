/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */
/* Copyright(c) 2015-17 Intel Corporation. */

#pragma once

#include "bitutil.h"

/*********************************************************************
 * cAVS and ACE1.x definitions
 *********************************************************************/

#define SDW_SHIM_BASE            0x2C000
#define SDW_ALH_BASE            0x2C800
#define SDW_SHIM_BASE_ACE        0x38000
#define SDW_ALH_BASE_ACE        0x24000
#define SDW_LINK_BASE            0x30000
#define SDW_LINK_SIZE            0x10000

/* Intel SHIM Registers Definition */
/* LCAP */
#define SDW_SHIM_LCAP            0x0
#define SDW_SHIM_LCAP_LCOUNT_MASK    GENMASK(2, 0)

/* LCTL */
#define SDW_SHIM_LCTL            0x4

#define SDW_SHIM_LCTL_SPA        BIT(0)
#define SDW_SHIM_LCTL_SPA_MASK        GENMASK(3, 0)
#define SDW_SHIM_LCTL_CPA        BIT(8)
#define SDW_SHIM_LCTL_CPA_MASK        GENMASK(11, 8)

/* SYNC */
#define SDW_SHIM_SYNC            0xC

#define SDW_SHIM_SYNC_SYNCPRD_VAL_24    (24000 / SDW_CADENCE_GSYNC_KHZ - 1)
#define SDW_SHIM_SYNC_SYNCPRD_VAL_38_4    (38400 / SDW_CADENCE_GSYNC_KHZ - 1)
#define SDW_SHIM_SYNC_SYNCPRD        GENMASK(14, 0)
#define SDW_SHIM_SYNC_SYNCCPU        BIT(15)
#define SDW_SHIM_SYNC_CMDSYNC_MASK    GENMASK(19, 16)
#define SDW_SHIM_SYNC_CMDSYNC        BIT(16)
#define SDW_SHIM_SYNC_SYNCGO        BIT(24)

/* Control stream capabililities and channel mask */
#define SDW_SHIM_CTLSCAP(x)        (0x010 + 0x60 * (x))
#define SDW_SHIM_CTLS0CM(x)        (0x012 + 0x60 * (x))
#define SDW_SHIM_CTLS1CM(x)        (0x014 + 0x60 * (x))
#define SDW_SHIM_CTLS2CM(x)        (0x016 + 0x60 * (x))
#define SDW_SHIM_CTLS3CM(x)        (0x018 + 0x60 * (x))

/* PCM Stream capabilities */
#define SDW_SHIM_PCMSCAP(x)        (0x020 + 0x60 * (x))

#define SDW_SHIM_PCMSCAP_ISS        GENMASK(3, 0)
#define SDW_SHIM_PCMSCAP_OSS        GENMASK(7, 4)
#define SDW_SHIM_PCMSCAP_BSS        GENMASK(12, 8)

/* PCM Stream Channel Map */
#define SDW_SHIM_PCMSYCHM(x, y)        (0x022 + (0x60 * (x)) + (0x2 * (y)))

/* PCM Stream Channel Count */
#define SDW_SHIM_PCMSYCHC(x, y)        (0x042 + (0x60 * (x)) + (0x2 * (y)))

#define SDW_SHIM_PCMSYCM_LCHN        GENMASK(3, 0)
#define SDW_SHIM_PCMSYCM_HCHN        GENMASK(7, 4)
#define SDW_SHIM_PCMSYCM_STREAM        GENMASK(13, 8)
#define SDW_SHIM_PCMSYCM_DIR        BIT(15)

/* IO control */
#define SDW_SHIM_IOCTL(x)        (0x06C + 0x60 * (x))

#define SDW_SHIM_IOCTL_MIF        BIT(0)
#define SDW_SHIM_IOCTL_CO        BIT(1)
#define SDW_SHIM_IOCTL_COE        BIT(2)
#define SDW_SHIM_IOCTL_DO        BIT(3)
#define SDW_SHIM_IOCTL_DOE        BIT(4)
#define SDW_SHIM_IOCTL_BKE        BIT(5)
#define SDW_SHIM_IOCTL_WPDD        BIT(6)
#define SDW_SHIM_IOCTL_CIBD        BIT(8)
#define SDW_SHIM_IOCTL_DIBD        BIT(9)

/* Wake Enable*/
#define SDW_SHIM_WAKEEN            0x190

#define SDW_SHIM_WAKEEN_ENABLE        BIT(0)

/* Wake Status */
#define SDW_SHIM_WAKESTS        0x192

#define SDW_SHIM_WAKESTS_STATUS        BIT(0)

/* AC Timing control */
#define SDW_SHIM_CTMCTL(x)        (0x06E + 0x60 * (x))

#define SDW_SHIM_CTMCTL_DACTQE        BIT(0)
#define SDW_SHIM_CTMCTL_DODS        BIT(1)
#define SDW_SHIM_CTMCTL_DOAIS        GENMASK(4, 3)

/* Intel ALH Register definitions */
#define SDW_ALH_STRMZCFG(x)        (0x000 + (0x4 * (x)))
#define SDW_ALH_NUM_STREAMS        64

#define SDW_ALH_STRMZCFG_DMAT_VAL    0x3
#define SDW_ALH_STRMZCFG_DMAT        GENMASK(7, 0)
#define SDW_ALH_STRMZCFG_CHN        GENMASK(19, 16)

/*********************************************************************
 * ACE2.x definitions for SHIM registers - only accessible when the
 * HDAudio extended link LCTL.SPA/CPA = 1.
 *********************************************************************/
/* x variable is link index */
#define SDW_SHIM2_GENERIC_BASE(x)    (0x00030000 + 0x8000 * (x))
#define SDW_IP_BASE(x)            (0x00030100 + 0x8000 * (x))
#define SDW_SHIM2_VS_BASE(x)        (0x00036000 + 0x8000 * (x))

/* SHIM2 Generic Registers */
/* Read-only capabilities */
#define SDW_SHIM2_LECAP            0x00
#define SDW_SHIM2_LECAP_HDS        BIT(0)        /* unset -> Host mode */
#define SDW_SHIM2_LECAP_MLC        GENMASK(3, 1)    /* Number of Lanes */

/* PCM Stream capabilities */
#define SDW_SHIM2_PCMSCAP        0x10
#define SDW_SHIM2_PCMSCAP_ISS        GENMASK(3, 0)    /* Input-only streams */
#define SDW_SHIM2_PCMSCAP_OSS        GENMASK(7, 4)    /* Output-only streams */
#define SDW_SHIM2_PCMSCAP_BSS        GENMASK(12, 8)    /* Bidirectional streams */

/* Read-only PCM Stream Channel Count, y variable is stream */
#define SDW_SHIM2_PCMSYCHC(y)        (0x14 + (0x4 * (y)))
#define SDW_SHIM2_PCMSYCHC_CS        GENMASK(3, 0)    /* Channels Supported */

/* PCM Stream Channel Map */
#define SDW_SHIM2_PCMSYCHM(y)        (0x16 + (0x4 * (y)))
#define SDW_SHIM2_PCMSYCHM_LCHAN    GENMASK(3, 0)    /* Lowest channel used by the FIFO port */
#define SDW_SHIM2_PCMSYCHM_HCHAN    GENMASK(7, 4)    /* Lowest channel used by the FIFO port */
#define SDW_SHIM2_PCMSYCHM_STRM        GENMASK(13, 8)    /* HDaudio stream tag */
#define SDW_SHIM2_PCMSYCHM_DIR        BIT(15)        /* HDaudio stream direction */

/* SHIM2 vendor-specific registers */
#define SDW_SHIM2_INTEL_VS_LVSCTL    0x04
#define SDW_SHIM2_INTEL_VS_LVSCTL_FCG    BIT(26)
#define SDW_SHIM2_INTEL_VS_LVSCTL_MLCS    GENMASK(29, 27)
#define SDW_SHIM2_INTEL_VS_LVSCTL_DCGD    BIT(30)
#define SDW_SHIM2_INTEL_VS_LVSCTL_ICGD    BIT(31)

#define SDW_SHIM2_MLCS_XTAL_CLK        0x0
#define SDW_SHIM2_MLCS_CARDINAL_CLK    0x1
#define SDW_SHIM2_MLCS_AUDIO_PLL_CLK    0x2
#define SDW_SHIM2_MLCS_MCLK_INPUT_CLK    0x3
#define SDW_SHIM2_MLCS_WOV_RING_OSC_CLK 0x4

#define SDW_SHIM2_INTEL_VS_WAKEEN    0x08
#define SDW_SHIM2_INTEL_VS_WAKEEN_PWE    BIT(0)

#define SDW_SHIM2_INTEL_VS_WAKESTS    0x0A
#define SDW_SHIM2_INTEL_VS_WAKEEN_PWS    BIT(0)

#define SDW_SHIM2_INTEL_VS_IOCTL    0x0C
#define SDW_SHIM2_INTEL_VS_IOCTL_MIF    BIT(0)
#define SDW_SHIM2_INTEL_VS_IOCTL_CO    BIT(1)
#define SDW_SHIM2_INTEL_VS_IOCTL_COE    BIT(2)
#define SDW_SHIM2_INTEL_VS_IOCTL_DO    BIT(3)
#define SDW_SHIM2_INTEL_VS_IOCTL_DOE    BIT(4)
#define SDW_SHIM2_INTEL_VS_IOCTL_BKE    BIT(5)
#define SDW_SHIM2_INTEL_VS_IOCTL_WPDD    BIT(6)
#define SDW_SHIM2_INTEL_VS_IOCTL_ODC    BIT(7)
#define SDW_SHIM2_INTEL_VS_IOCTL_CIBD    BIT(8)
#define SDW_SHIM2_INTEL_VS_IOCTL_DIBD    BIT(9)
#define SDW_SHIM2_INTEL_VS_IOCTL_HAMIFD    BIT(10)

#define SDW_SHIM2_INTEL_VS_ACTMCTL    0x0E
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DACTQE    BIT(0)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DODS        BIT(1)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DODSE    BIT(2)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DOAIS    GENMASK(4, 3)
#define SDW_SHIM2_INTEL_VS_ACTMCTL_DOAISE    BIT(5)

/**
 * struct sdw_intel_stream_params_data: configuration passed during
 * the @params_stream callback, e.g. for interaction with DSP
 * firmware.
 */
struct sdw_intel_stream_params_data {
    struct snd_pcm_substream *substream;
    struct snd_soc_dai *dai;
    struct snd_pcm_hw_params *hw_params;
    int link_id;
    int alh_stream_id;
};

/**
 * struct sdw_intel_stream_free_data: configuration passed during
 * the @free_stream callback, e.g. for interaction with DSP
 * firmware.
 */
struct sdw_intel_stream_free_data {
    struct snd_pcm_substream *substream;
    struct snd_soc_dai *dai;
    int link_id;
};

struct sdw_intel_link_dev;

/* Intel clock-stop/pm_runtime quirk definitions */

/*
 * Force the clock to remain on during pm_runtime suspend. This might
 * be needed if Slave devices do not have an alternate clock source or
 * if the latency requirements are very strict.
 */
#define SDW_INTEL_CLK_STOP_NOT_ALLOWED        BIT(0)

/*
 * Stop the bus during pm_runtime suspend. If set, a complete bus
 * reset and re-enumeration will be performed when the bus
 * restarts. This mode shall not be used if Slave devices can generate
 * in-band wakes.
 */
#define SDW_INTEL_CLK_STOP_TEARDOWN        BIT(1)

/*
 * Stop the bus during pm_suspend if Slaves are not wake capable
 * (e.g. speaker amplifiers). The clock-stop mode is typically
 * slightly higher power than when the IP is completely powered-off.
 */
#define SDW_INTEL_CLK_STOP_WAKE_CAPABLE_ONLY    BIT(2)

/*
 * Require a bus reset (and complete re-enumeration) when exiting
 * clock stop modes. This may be needed if the controller power was
 * turned off and all context lost. This quirk shall not be used if a
 * Slave device needs to remain enumerated and keep its context,
 * e.g. to provide the reasons for the wake, report acoustic events or
 * pass a history buffer.
 */
#define SDW_INTEL_CLK_STOP_BUS_RESET        BIT(3)

/*
 * IDA min selected to allow for 5 unconstrained devices per link,
 * and 6 system-unique Device Numbers for wake-capable devices.
 */

#define SDW_INTEL_DEV_NUM_IDA_MIN           6
