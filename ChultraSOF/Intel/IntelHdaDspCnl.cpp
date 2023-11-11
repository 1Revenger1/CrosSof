//
//  ChultraSofCml.cpp
//  ChultraSOF
//
//  Created by Gwydien on 8/21/23.
//

#include "IntelHdaDspCnl.hpp"
#include "shim.h"
#include "sdw_intel.h"

#define super IntelHdaDsp
OSDefineMetaClassAndStructors(IntelHdaDspCnl, IntelHdaDsp);
OSDefineMetaClassAndStructors(IntelHdaDspCml, IntelHdaDspCnl);

const struct SOFIntelChipInfo cnl_chip_info = {
    /* Cannonlake */
    .cores_num = 4,
    .init_core_mask = 1,
    .host_managed_cores_mask = GENMASK(3, 0),
    .ipc_req = CNL_DSP_REG_HIPCIDR,
    .ipc_req_mask = CNL_DSP_REG_HIPCIDR_BUSY,
    .ipc_ack = CNL_DSP_REG_HIPCIDA,
    .ipc_ack_mask = CNL_DSP_REG_HIPCIDA_DONE,
    .ipc_ctl = CNL_DSP_REG_HIPCCTL,
    .rom_status_reg = HDA_DSP_SRAM_REG_ROM_STATUS,
    .rom_init_timeout    = 300,
    .ssp_count = CNL_SSP_COUNT,
    .ssp_base_offset = CNL_SSP_BASE_OFFSET,
    .sdw_shim_base = SDW_SHIM_BASE,
    .sdw_alh_base = SDW_ALH_BASE,
    .d0i3_offset = SOF_HDA_VS_D0I3C,
    .hw_ip_version = SOF_INTEL_CAVS_1_8
};

static const struct SOFDeviceDesc cnlDesc = {
    .use_acpi_target_states     = true,
    .resindex_lpe_base          = 0,
    .resindex_pcicfg_base       = -1,
    .resindex_imr_base          = -1,
    .irqindex_host_ipc          = -1,
    .chip_info = &cnl_chip_info,
    .dspless_mode_supported     = true,        /* Only supported for HDaudio */
    .default_fw_path =          "intel/sof",
    .default_tplg_path =        "intel/sof-tplg",
    .default_fw_filename =      "sof-cnl.ri",
    .nocodec_tplg_filename =    "sof-cnl-nocodec.tplg",
};

static const struct SOFDeviceDesc cmlDesc = {
    .use_acpi_target_states     = true,
    .resindex_lpe_base          = 0,
    .resindex_pcicfg_base       = -1,
    .resindex_imr_base          = -1,
    .irqindex_host_ipc          = -1,
    .chip_info = &cnl_chip_info,
    .dspless_mode_supported     = true,        /* Only supported for HDaudio */
    .default_fw_path =          "intel/sof",
    .default_tplg_path =        "intel/sof-tplg",
    .default_fw_filename =      "sof-cml.ri",
    .nocodec_tplg_filename =    "sof-cnl-nocodec.tplg"
};

//
// Cannon Lake
//

const SOFIntelChipInfo *IntelHdaDspCnl::getChipDesc() {
    return &cnl_chip_info;
}

const SOFDeviceDesc *IntelHdaDspCnl::getDeviceDesc() {
    return &cnlDesc;
}

//
// Comet Lake
//

const SOFDeviceDesc *IntelHdaDspCml::getDeviceDesc() {
    return &cmlDesc;
}
