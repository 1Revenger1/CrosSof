/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */
/*
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * Copyright(c) 2017 Intel Corporation. All rights reserved.
 *
 * Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
 */

#pragma once

#include <libkern/c++/OSObject.h>

#include "bitutil.h"
#include "hdaudio_ext.h"
#include "shim.h"

/* PCI registers */
#define PCI_TCSEL                   0x44
#define PCI_PGCTL                   PCI_TCSEL
#define PCI_CGCTL                   0x48

/* PCI_PGCTL bits */
#define PCI_PGCTL_ADSPPGD           BIT(2)
#define PCI_PGCTL_LSRMD_MASK        BIT(4)

/* PCI_CGCTL bits */
#define PCI_CGCTL_MISCBDCGE_MASK    BIT(6)
#define PCI_CGCTL_ADSPDCGE          BIT(1)

/* Legacy HDA registers and bits used - widths are variable */
#define SOF_HDA_GCAP                0x0
#define SOF_HDA_GCTL                0x8
/* accept unsol. response enable */
#define SOF_HDA_GCTL_UNSOL          BIT(8)
#define SOF_HDA_LLCH                0x14
#define SOF_HDA_INTCTL              0x20
#define SOF_HDA_INTSTS              0x24
#define SOF_HDA_WAKESTS             0x0E
#define SOF_HDA_WAKESTS_INT_MASK    GENMASK(8)
#define SOF_HDA_RIRBSTS             0x5d

/* SOF_HDA_GCTL register bist */
#define SOF_HDA_GCTL_RESET          BIT(0)

/* SOF_HDA_INCTL regs */
#define SOF_HDA_INT_GLOBAL_EN       BIT(31)
#define SOF_HDA_INT_CTRL_EN         BIT(30)
#define SOF_HDA_INT_ALL_STREAM      0xff

/* SOF_HDA_INTSTS regs */
#define SOF_HDA_INTSTS_GIS          BIT(31)

#define SOF_HDA_MAX_CAPS        10
#define SOF_HDA_CAP_ID_OFF        16
#define SOF_HDA_CAP_ID_MASK        GENMASK(SOF_HDA_CAP_ID_OFF + 11,\
                        SOF_HDA_CAP_ID_OFF)
#define SOF_HDA_CAP_NEXT_MASK        0xFFFF

enum hda_cap_ids_t {
    SOF_HDA_GTS_CAP_ID = 1,
    SOF_HDA_ML_CAP_ID = 2,
    SOF_HDA_PP_CAP_ID = 3,
    SOF_HDA_SPIB_CAP_ID = 4,
    SOF_HDA_DRSM_CAP_ID = 5
};

// PP Cap
#define SOF_HDA_REG_PP_PPCH        0x10
#define SOF_HDA_REG_PP_PPCTL        0x04
#define SOF_HDA_REG_PP_PPSTS        0x08
#define SOF_HDA_PPCTL_PIE        BIT(31)
#define SOF_HDA_PPCTL_GPROCEN        BIT(30)

/*Vendor Specific Registers*/
#define SOF_HDA_VS_D0I3C        0x104A

/* D0I3C Register fields */
#define SOF_HDA_VS_D0I3C_CIP        BIT(0) /* Command-In-Progress */
#define SOF_HDA_VS_D0I3C_I3        BIT(2) /* D0i3 enable bit */

/* DPIB entry size: 8 Bytes = 2 DWords */
#define SOF_HDA_DPIB_ENTRY_SIZE    0x8

#define SOF_HDA_SPIB_BASE        0x08
#define SOF_HDA_SPIB_INTERVAL        0x08
#define SOF_HDA_SPIB_SPIB        0x00
#define SOF_HDA_SPIB_MAXFIFO        0x04

#define SOF_HDA_PPHC_BASE        0x10
#define SOF_HDA_PPHC_INTERVAL        0x10

#define SOF_HDA_PPLC_BASE        0x10
#define SOF_HDA_PPLC_MULTI        0x10
#define SOF_HDA_PPLC_INTERVAL        0x10

#define SOF_HDA_DRSM_BASE        0x08
#define SOF_HDA_DRSM_INTERVAL        0x08

/* Descriptor error interrupt */
#define SOF_HDA_CL_DMA_SD_INT_DESC_ERR        0x10

/* FIFO error interrupt */
#define SOF_HDA_CL_DMA_SD_INT_FIFO_ERR        0x08

/* Buffer completion interrupt */
#define SOF_HDA_CL_DMA_SD_INT_COMPLETE        0x04

#define SOF_HDA_CL_DMA_SD_INT_MASK \
    (SOF_HDA_CL_DMA_SD_INT_DESC_ERR | \
    SOF_HDA_CL_DMA_SD_INT_FIFO_ERR | \
    SOF_HDA_CL_DMA_SD_INT_COMPLETE)
#define SOF_HDA_SD_CTL_DMA_START        0x02 /* Stream DMA start bit */

/* Intel HD Audio Code Loader DMA Registers */
#define SOF_HDA_ADSP_LOADER_BASE        0x80
#define SOF_HDA_ADSP_DPLBASE            0x70
#define SOF_HDA_ADSP_DPUBASE            0x74
#define SOF_HDA_ADSP_DPLBASE_ENABLE        0x01

/* Stream Registers */
#define SOF_HDA_ADSP_REG_SD_CTL            0x00
#define SOF_HDA_ADSP_REG_SD_STS            0x03
#define SOF_HDA_ADSP_REG_SD_LPIB        0x04
#define SOF_HDA_ADSP_REG_SD_CBL            0x08
#define SOF_HDA_ADSP_REG_SD_LVI            0x0C
#define SOF_HDA_ADSP_REG_SD_FIFOW        0x0E
#define SOF_HDA_ADSP_REG_SD_FIFOSIZE        0x10
#define SOF_HDA_ADSP_REG_SD_FORMAT        0x12
#define SOF_HDA_ADSP_REG_SD_FIFOL        0x14
#define SOF_HDA_ADSP_REG_SD_BDLPL        0x18
#define SOF_HDA_ADSP_REG_SD_BDLPU        0x1C
#define SOF_HDA_ADSP_SD_ENTRY_SIZE        0x20

/* SDxFIFOS FIFOS */
#define SOF_HDA_SD_FIFOSIZE_FIFOS_MASK        GENMASK(15, 0)

/* CL: Software Position Based FIFO Capability Registers */
#define SOF_DSP_REG_CL_SPBFIFO \
    (SOF_HDA_ADSP_LOADER_BASE + 0x20)
#define SOF_HDA_ADSP_REG_CL_SPBFIFO_SPBFCH    0x0
#define SOF_HDA_ADSP_REG_CL_SPBFIFO_SPBFCCTL    0x4
#define SOF_HDA_ADSP_REG_CL_SPBFIFO_SPIB    0x8
#define SOF_HDA_ADSP_REG_CL_SPBFIFO_MAXFIFOS    0xc

/* Stream Number */
#define SOF_HDA_CL_SD_CTL_STREAM_TAG_SHIFT    20
#define SOF_HDA_CL_SD_CTL_STREAM_TAG_MASK \
    GENMASK(SOF_HDA_CL_SD_CTL_STREAM_TAG_SHIFT + 3,\
        SOF_HDA_CL_SD_CTL_STREAM_TAG_SHIFT)

enum hda_bars_t {
    HDA_DSP_HDA_BAR,
    HDA_DSP_PP_BAR,
    HDA_DSP_SPIB_BAR,
    HDA_DSP_DRSM_BAR,
    HDA_DSP_DSP_BAR,
    HDA_DSP_MAX_BAR
};

#define SRAM_WINDOW_OFFSET(x)            (0x80000 + (x) * 0x20000)

#define HDA_DSP_MBOX_OFFSET            SRAM_WINDOW_OFFSET(0)

#define HDA_DSP_PANIC_OFFSET(x) \
    (((x) & 0xFFFFFF) + HDA_DSP_MBOX_OFFSET)

/* SRAM window 0 FW "registers" */
#define HDA_DSP_SRAM_REG_ROM_STATUS        (HDA_DSP_MBOX_OFFSET + 0x0)
#define HDA_DSP_SRAM_REG_ROM_ERROR        (HDA_DSP_MBOX_OFFSET + 0x4)
/* FW and ROM share offset 4 */
#define HDA_DSP_SRAM_REG_FW_STATUS        (HDA_DSP_MBOX_OFFSET + 0x4)
#define HDA_DSP_SRAM_REG_FW_TRACEP        (HDA_DSP_MBOX_OFFSET + 0x8)
#define HDA_DSP_SRAM_REG_FW_END            (HDA_DSP_MBOX_OFFSET + 0xc)

#define HDA_DSP_MBOX_UPLINK_OFFSET        0x81000

#define HDA_DSP_STREAM_RESET_TIMEOUT        300
/*
 * Timeout in us, for setting the stream RUN bit, during
 * start/stop the stream. The timeout expires if new RUN bit
 * value cannot be read back within the specified time.
 */
#define HDA_DSP_STREAM_RUN_TIMEOUT        300

#define HDA_DSP_SPIB_ENABLE            1
#define HDA_DSP_SPIB_DISABLE            0

#define SOF_HDA_MAX_BUFFER_SIZE            (32 * PAGE_SIZE)

#define HDA_DSP_STACK_DUMP_SIZE            32

/* ROM/FW status register */
#define FSR_STATE_MASK                GENMASK(23, 0)
#define FSR_WAIT_STATE_MASK            GENMASK(27, 24)
#define FSR_MODULE_MASK                GENMASK(30, 28)
#define FSR_HALTED                BIT(31)
#define FSR_TO_STATE_CODE(x)            ((x) & FSR_STATE_MASK)
#define FSR_TO_WAIT_STATE_CODE(x)        (((x) & FSR_WAIT_STATE_MASK) >> 24)
#define FSR_TO_MODULE_CODE(x)            (((x) & FSR_MODULE_MASK) >> 28)

/* Wait states */
#define FSR_WAIT_FOR_IPC_BUSY            0x1
#define FSR_WAIT_FOR_IPC_DONE            0x2
#define FSR_WAIT_FOR_CACHE_INVALIDATION        0x3
#define FSR_WAIT_FOR_LP_SRAM_OFF        0x4
#define FSR_WAIT_FOR_DMA_BUFFER_FULL        0x5
#define FSR_WAIT_FOR_CSE_CSR            0x6

/* Module codes */
#define FSR_MOD_ROM                0x0
#define FSR_MOD_ROM_BYP                0x1
#define FSR_MOD_BASE_FW                0x2
#define FSR_MOD_LP_BOOT                0x3
#define FSR_MOD_BRNGUP                0x4
#define FSR_MOD_ROM_EXT                0x5

/* State codes (module dependent) */
/* Module independent states */
#define FSR_STATE_INIT                0x0
#define FSR_STATE_INIT_DONE            0x1
#define FSR_STATE_FW_ENTERED            0x5

/* ROM states */
#define FSR_STATE_ROM_INIT            FSR_STATE_INIT
#define FSR_STATE_ROM_INIT_DONE            FSR_STATE_INIT_DONE
#define FSR_STATE_ROM_CSE_MANIFEST_LOADED    0x2
#define FSR_STATE_ROM_FW_MANIFEST_LOADED    0x3
#define FSR_STATE_ROM_FW_FW_LOADED        0x4
#define FSR_STATE_ROM_FW_ENTERED        FSR_STATE_FW_ENTERED
#define FSR_STATE_ROM_VERIFY_FEATURE_MASK    0x6
#define FSR_STATE_ROM_GET_LOAD_OFFSET        0x7
#define FSR_STATE_ROM_FETCH_ROM_EXT        0x8
#define FSR_STATE_ROM_FETCH_ROM_EXT_DONE    0x9
#define FSR_STATE_ROM_BASEFW_ENTERED        0xf /* SKL */

/* (ROM) CSE states */
#define FSR_STATE_ROM_CSE_IMR_REQUEST            0x10
#define FSR_STATE_ROM_CSE_IMR_GRANTED            0x11
#define FSR_STATE_ROM_CSE_VALIDATE_IMAGE_REQUEST    0x12
#define FSR_STATE_ROM_CSE_IMAGE_VALIDATED        0x13

#define FSR_STATE_ROM_CSE_IPC_IFACE_INIT    0x20
#define FSR_STATE_ROM_CSE_IPC_RESET_PHASE_1    0x21
#define FSR_STATE_ROM_CSE_IPC_OPERATIONAL_ENTRY    0x22
#define FSR_STATE_ROM_CSE_IPC_OPERATIONAL    0x23
#define FSR_STATE_ROM_CSE_IPC_DOWN        0x24

/* BRINGUP (or BRNGUP) states */
#define FSR_STATE_BRINGUP_INIT            FSR_STATE_INIT
#define FSR_STATE_BRINGUP_INIT_DONE        FSR_STATE_INIT_DONE
#define FSR_STATE_BRINGUP_HPSRAM_LOAD        0x2
#define FSR_STATE_BRINGUP_UNPACK_START        0X3
#define FSR_STATE_BRINGUP_IMR_RESTORE        0x4
#define FSR_STATE_BRINGUP_FW_ENTERED        FSR_STATE_FW_ENTERED

/* ROM  status/error values */
#define HDA_DSP_ROM_CSE_ERROR            40
#define HDA_DSP_ROM_CSE_WRONG_RESPONSE        41
#define HDA_DSP_ROM_IMR_TO_SMALL        42
#define HDA_DSP_ROM_BASE_FW_NOT_FOUND        43
#define HDA_DSP_ROM_CSE_VALIDATION_FAILED    44
#define HDA_DSP_ROM_IPC_FATAL_ERROR        45
#define HDA_DSP_ROM_L2_CACHE_ERROR        46
#define HDA_DSP_ROM_LOAD_OFFSET_TO_SMALL    47
#define HDA_DSP_ROM_API_PTR_INVALID        50
#define HDA_DSP_ROM_BASEFW_INCOMPAT        51
#define HDA_DSP_ROM_UNHANDLED_INTERRUPT        0xBEE00000
#define HDA_DSP_ROM_MEMORY_HOLE_ECC        0xECC00000
#define HDA_DSP_ROM_KERNEL_EXCEPTION        0xCAFE0000
#define HDA_DSP_ROM_USER_EXCEPTION        0xBEEF0000
#define HDA_DSP_ROM_UNEXPECTED_RESET        0xDECAF000
#define HDA_DSP_ROM_NULL_FW_ENTRY        0x4c4c4e55

#define HDA_DSP_ROM_IPC_CONTROL            0x01000000
#define HDA_DSP_ROM_IPC_PURGE_FW        0x00004000

/* various timeout values */
#define HDA_DSP_PU_TIMEOUT        50
#define HDA_DSP_PD_TIMEOUT        50
#define HDA_DSP_RESET_TIMEOUT_US    50000
#define HDA_DSP_BASEFW_TIMEOUT_US       3000000
#define HDA_DSP_INIT_TIMEOUT_US    500000
#define HDA_DSP_CTRL_RESET_TIMEOUT        100
#define HDA_DSP_WAIT_TIMEOUT        500    /* 500 msec */
#define HDA_DSP_REG_POLL_INTERVAL_US        500    /* 0.5 msec */
#define HDA_DSP_REG_POLL_RETRY_COUNT        50

#define HDA_DSP_ADSPIC_IPC            BIT(0)
#define HDA_DSP_ADSPIS_IPC            BIT(0)

/* Intel HD Audio General DSP Registers */
#define HDA_DSP_GEN_BASE        0x0
#define HDA_DSP_REG_ADSPCS        (HDA_DSP_GEN_BASE + 0x04)
#define HDA_DSP_REG_ADSPIC        (HDA_DSP_GEN_BASE + 0x08)
#define HDA_DSP_REG_ADSPIS        (HDA_DSP_GEN_BASE + 0x0C)
#define HDA_DSP_REG_ADSPIC2        (HDA_DSP_GEN_BASE + 0x10)
#define HDA_DSP_REG_ADSPIS2        (HDA_DSP_GEN_BASE + 0x14)

#define HDA_DSP_REG_ADSPIC2_SNDW    BIT(5)
#define HDA_DSP_REG_ADSPIS2_SNDW    BIT(5)

/* Intel HD Audio Inter-Processor Communication Registers */
#define HDA_DSP_IPC_BASE        0x40
#define HDA_DSP_REG_HIPCT        (HDA_DSP_IPC_BASE + 0x00)
#define HDA_DSP_REG_HIPCTE        (HDA_DSP_IPC_BASE + 0x04)
#define HDA_DSP_REG_HIPCI        (HDA_DSP_IPC_BASE + 0x08)
#define HDA_DSP_REG_HIPCIE        (HDA_DSP_IPC_BASE + 0x0C)
#define HDA_DSP_REG_HIPCCTL        (HDA_DSP_IPC_BASE + 0x10)

/* Intel Vendor Specific Registers */
#define HDA_VS_INTEL_EM2        0x1030
#define HDA_VS_INTEL_EM2_L1SEN        BIT(13)
#define HDA_VS_INTEL_LTRP        0x1048
#define HDA_VS_INTEL_LTRP_GB_MASK    0x3F

/*  HIPCI */
#define HDA_DSP_REG_HIPCI_BUSY        BIT(31)
#define HDA_DSP_REG_HIPCI_MSG_MASK    0x7FFFFFFF

/* HIPCIE */
#define HDA_DSP_REG_HIPCIE_DONE    BIT(30)
#define HDA_DSP_REG_HIPCIE_MSG_MASK    0x3FFFFFFF

/* HIPCCTL */
#define HDA_DSP_REG_HIPCCTL_DONE    BIT(1)
#define HDA_DSP_REG_HIPCCTL_BUSY    BIT(0)

/* HIPCT */
#define HDA_DSP_REG_HIPCT_BUSY        BIT(31)
#define HDA_DSP_REG_HIPCT_MSG_MASK    0x7FFFFFFF

/* HIPCTE */
#define HDA_DSP_REG_HIPCTE_MSG_MASK    0x3FFFFFFF

#define HDA_DSP_ADSPIC_CL_DMA        BIT(1)
#define HDA_DSP_ADSPIS_CL_DMA        BIT(1)

/* Delay before scheduling D0i3 entry */
#define BXT_D0I3_DELAY 5000

#define FW_CL_STREAM_NUMBER        0x1
#define HDA_FW_BOOT_ATTEMPTS        3

/* ADSPCS - Audio DSP Control & Status */

/*
 * Core Reset - asserted high
 * CRST Mask for a given core mask pattern, cm
 */
#define HDA_DSP_ADSPCS_CRST_SHIFT    0
#define HDA_DSP_ADSPCS_CRST_MASK(cm)    ((cm) << HDA_DSP_ADSPCS_CRST_SHIFT)

/*
 * Core run/stall - when set to '1' core is stalled
 * CSTALL Mask for a given core mask pattern, cm
 */
#define HDA_DSP_ADSPCS_CSTALL_SHIFT    8
#define HDA_DSP_ADSPCS_CSTALL_MASK(cm)    ((cm) << HDA_DSP_ADSPCS_CSTALL_SHIFT)

/*
 * Set Power Active - when set to '1' turn cores on
 * SPA Mask for a given core mask pattern, cm
 */
#define HDA_DSP_ADSPCS_SPA_SHIFT    16
#define HDA_DSP_ADSPCS_SPA_MASK(cm)    ((cm) << HDA_DSP_ADSPCS_SPA_SHIFT)

/*
 * Current Power Active - power status of cores, set by hardware
 * CPA Mask for a given core mask pattern, cm
 */
#define HDA_DSP_ADSPCS_CPA_SHIFT    24
#define HDA_DSP_ADSPCS_CPA_MASK(cm)    ((cm) << HDA_DSP_ADSPCS_CPA_SHIFT)

/*
 * Mask for a given number of cores
 * nc = number of supported cores
 */
#define SOF_DSP_CORES_MASK(nc)    GENMASK(((nc) - 1), 0)

/* Intel HD Audio Inter-Processor Communication Registers for Cannonlake*/
#define CNL_DSP_IPC_BASE        0xc0
#define CNL_DSP_REG_HIPCTDR        (CNL_DSP_IPC_BASE + 0x00)
#define CNL_DSP_REG_HIPCTDA        (CNL_DSP_IPC_BASE + 0x04)
#define CNL_DSP_REG_HIPCTDD        (CNL_DSP_IPC_BASE + 0x08)
#define CNL_DSP_REG_HIPCIDR        (CNL_DSP_IPC_BASE + 0x10)
#define CNL_DSP_REG_HIPCIDA        (CNL_DSP_IPC_BASE + 0x14)
#define CNL_DSP_REG_HIPCIDD        (CNL_DSP_IPC_BASE + 0x18)
#define CNL_DSP_REG_HIPCCTL        (CNL_DSP_IPC_BASE + 0x28)

/*  HIPCI */
#define CNL_DSP_REG_HIPCIDR_BUSY        BIT(31)
#define CNL_DSP_REG_HIPCIDR_MSG_MASK    0x7FFFFFFF

/* HIPCIE */
#define CNL_DSP_REG_HIPCIDA_DONE    BIT(31)
#define CNL_DSP_REG_HIPCIDA_MSG_MASK    0x7FFFFFFF

/* HIPCCTL */
#define CNL_DSP_REG_HIPCCTL_DONE    BIT(1)
#define CNL_DSP_REG_HIPCCTL_BUSY    BIT(0)

/* HIPCT */
#define CNL_DSP_REG_HIPCTDR_BUSY        BIT(31)
#define CNL_DSP_REG_HIPCTDR_MSG_MASK    0x7FFFFFFF

/* HIPCTDA */
#define CNL_DSP_REG_HIPCTDA_DONE    BIT(31)
#define CNL_DSP_REG_HIPCTDA_MSG_MASK    0x7FFFFFFF

/* HIPCTDD */
#define CNL_DSP_REG_HIPCTDD_MSG_MASK    0x7FFFFFFF

/* BDL */
#define HDA_DSP_BDL_SIZE            4096
#define HDA_DSP_MAX_BDL_ENTRIES            \
    (HDA_DSP_BDL_SIZE / sizeof(struct sof_intel_dsp_bdl))

/* Number of DAIs */
#define SOF_SKL_NUM_DAIS_NOCODEC    8

#ifdef CONFIG_SND_SOC_SOF_HDA_AUDIO_CODEC
#define SOF_SKL_NUM_DAIS        15
#else
#define SOF_SKL_NUM_DAIS        SOF_SKL_NUM_DAIS_NOCODEC
#endif

/* Intel HD Audio SRAM Window 0*/
#define HDA_DSP_SRAM_REG_ROM_STATUS_SKL    0x8000
#define HDA_ADSP_SRAM0_BASE_SKL        0x8000

/* Firmware status window */
#define HDA_ADSP_FW_STATUS_SKL        HDA_ADSP_SRAM0_BASE_SKL
#define HDA_ADSP_ERROR_CODE_SKL        (HDA_ADSP_FW_STATUS_SKL + 0x4)

/* Host Device Memory Space */
#define APL_SSP_BASE_OFFSET    0x2000
#define CNL_SSP_BASE_OFFSET    0x10000

/* Host Device Memory Size of a Single SSP */
#define SSP_DEV_MEM_SIZE    0x1000

/* SSP Count of the Platform */
#define APL_SSP_COUNT        6
#define CNL_SSP_COUNT        3
#define ICL_SSP_COUNT        6
#define TGL_SSP_COUNT        3
#define MTL_SSP_COUNT        3

/* SSP Registers */
#define SSP_SSC1_OFFSET        0x4
#define SSP_SET_SCLK_CONSUMER    BIT(25)
#define SSP_SET_SFRM_CONSUMER    BIT(24)
#define SSP_SET_CBP_CFP        (SSP_SET_SCLK_CONSUMER | SSP_SET_SFRM_CONSUMER)

#define HDA_IDISP_ADDR        2
#define HDA_IDISP_CODEC(x) ((x) & BIT(HDA_IDISP_ADDR))

struct sof_intel_dsp_bdl {
    SInt32 addr_l;
    SInt32 addr_h;
    SInt32 size;
    SInt32 ioc;
} __attribute((packed));

#define SOF_HDA_PLAYBACK_STREAMS    16
#define SOF_HDA_CAPTURE_STREAMS        16
#define SOF_HDA_PLAYBACK        0
#define SOF_HDA_CAPTURE            1

/* stream flags */
#define SOF_HDA_STREAM_DMI_L1_COMPATIBLE    1

/*
 * Time in ms for opportunistic D0I3 entry delay.
 * This has been deliberately chosen to be long to avoid race conditions.
 * Could be optimized in future.
 */
#define SOF_HDA_D0I3_WORK_DELAY_MS    5000

/* HDA DSP D0 substate */
enum sof_hda_D0_substate {
    SOF_HDA_DSP_PM_D0I0,    /* default D0 substate */
    SOF_HDA_DSP_PM_D0I3,    /* low power D0 substate */
};

#define SOF_STREAM_SD_OFFSET(s) \
    (SOF_HDA_ADSP_SD_ENTRY_SIZE * ((s)->index) \
     + SOF_HDA_ADSP_LOADER_BASE)

class SOFIntelHdaStream : public OSObject {
    OSDeclareDefaultStructors(SOFIntelHdaStream);
public:
    struct hdac_ext_stream hext_stream;
    struct sof_intel_stream sof_intel_stream;
    int host_reserved; /* reserve host DMA channel */
    UInt32 flags;
};
