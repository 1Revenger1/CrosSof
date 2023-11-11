//
//  sof.h
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#pragma once

/**
 * enum sof_fw_state - DSP firmware state definitions
 * @SOF_FW_BOOT_NOT_STARTED:    firmware boot is not yet started
 * @SOF_DSPLESS_MODE:        DSP is not used
 * @SOF_FW_BOOT_PREPARE:    preparing for boot (firmware loading for exaqmple)
 * @SOF_FW_BOOT_IN_PROGRESS:    firmware boot is in progress
 * @SOF_FW_BOOT_FAILED:        firmware boot failed
 * @SOF_FW_BOOT_READY_FAILED:    firmware booted but fw_ready op failed
 * @SOF_FW_BOOT_READY_OK:    firmware booted and fw_ready op passed
 * @SOF_FW_BOOT_COMPLETE:    firmware is booted up and functional
 * @SOF_FW_CRASHED:        firmware crashed after successful boot
 */
enum sof_fw_state_t {
    SOF_FW_BOOT_NOT_STARTED = 0,
    SOF_DSPLESS_MODE,
    SOF_FW_BOOT_PREPARE,
    SOF_FW_BOOT_IN_PROGRESS,
    SOF_FW_BOOT_FAILED,
    SOF_FW_BOOT_READY_FAILED,
    SOF_FW_BOOT_READY_OK,
    SOF_FW_BOOT_COMPLETE,
    SOF_FW_CRASHED,
};

/* DSP power states */
enum sof_dsp_power_states_t {
    SOF_DSP_PM_D0,
    SOF_DSP_PM_D1,
    SOF_DSP_PM_D2,
    SOF_DSP_PM_D3,
};

struct SOFMailbox {
    uint32_t offset;
    size_t size;
};

// Assume Type3 IPCs
struct SOFDeviceDesc {
    bool use_acpi_target_states;

    /* Platform resource indexes in BAR / ACPI resources. */
    /* Must set to -1 if not used - add new items to end */
    int resindex_lpe_base;
    int resindex_pcicfg_base;
    int resindex_imr_base;
    int irqindex_host_ipc;

    /* IPC timeouts in ms */
    int ipc_timeout;
    int boot_timeout;

    /* chip information for dsp */
    const void *chip_info;

    /* defaults for no codec mode */
    const char *nocodec_tplg_filename;

    /* The platform supports DSPless mode */
    bool dspless_mode_supported;

    /* defaults paths for firmware, library and topology files */
    const char *default_fw_path;
    const char *default_lib_path;
    const char *default_tplg_path;

    /* default firmware name */
    const char *default_fw_filename;
};
