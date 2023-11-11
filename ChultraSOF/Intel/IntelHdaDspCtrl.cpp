//
//  IntelHdaDspCtrl.cpp
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#include "IntelHdaDsp.hpp"
#include "hda.h"
#include "Logger.h"

IOReturn IntelHdaDsp::ctrlLinkReset(bool reset) {
    UInt32 timeout;
    UInt32 gctl = 0;
    UInt32 val;

    /* 0 to enter reset and 1 to exit reset */
    val = reset ? 0 : SOF_HDA_GCTL_RESET;

    /* enter/exit HDA controller reset */
    dspUpdate(HDA_DSP_HDA_BAR, SOF_HDA_GCTL, SOF_HDA_GCTL_RESET, val);

    /* wait to enter/exit reset */
    timeout = HDA_DSP_CTRL_RESET_TIMEOUT;
    while (timeout > 0) {
        gctl = read32(HDA_DSP_HDA_BAR,  SOF_HDA_GCTL);
        if ((gctl & SOF_HDA_GCTL_RESET) == val)
            return kIOReturnSuccess;
        IODelay(1000);
        timeout--;
    }

    /* enter/exit reset failed */
    IOLogError("Failed to %s HDA controller gctl 0x%x",
               reset ? "reset" : "ready", gctl);
    return -kIOReturnNoDevice;
}

IOReturn IntelHdaDsp::ctrlGetCaps() {
    IOReturn ret;
    UInt32 count = 0;
    
    /*
     * On some devices, one reset cycle is necessary before reading
     * capabilities
     */
    ret = ctrlLinkReset(true);
    if (ret != kIOReturnSuccess)
        return ret;
    ret = ctrlLinkReset(false);
    if (ret != kIOReturnSuccess)
        return ret;
    
    uint32_t offset = read32(HDA_DSP_HDA_BAR, SOF_HDA_LLCH);
    
    do {
        
        IOLogDebug("Checking for capabilities at offset 0x%x\n", offset & SOF_HDA_CAP_NEXT_MASK);
        UInt32 capability = read32(HDA_DSP_HDA_BAR, offset);
        UInt32 feature = (capability & SOF_HDA_CAP_ID_MASK) >> SOF_HDA_CAP_ID_OFF;
        
        switch (feature) {
            case SOF_HDA_PP_CAP_ID:
                IOLogDebug("Found DSP Capability at 0x%x", offset);
                bars[HDA_DSP_PP_BAR] = bars[HDA_DSP_HDA_BAR] + offset;
                break;
            case SOF_HDA_SPIB_CAP_ID:
                IOLogDebug("Found SPIB Capability at 0x%x", offset);
                bars[HDA_DSP_SPIB_BAR] = bars[HDA_DSP_HDA_BAR] + offset;
                break;
            case SOF_HDA_DRSM_CAP_ID:
                IOLogDebug("Found DRSM Capability at 0x%x", offset);
                bars[HDA_DSP_DRSM_BAR] = bars[HDA_DSP_HDA_BAR] + offset;
                break;
            case SOF_HDA_GTS_CAP_ID:
                IOLogDebug("Found GTS Capability at 0x%x", offset);
                break;
            case SOF_HDA_ML_CAP_ID:
                IOLogDebug("Found ML Capability at 0x%x", offset);
                break;
            default:
                IOLogDebug("Found capability %d at 0x%d", feature, offset);
                break;
        }
        
        offset = capability & SOF_HDA_CAP_NEXT_MASK;
    } while (count++ <= SOF_HDA_MAX_CAPS && offset);
        
    return kIOReturnSuccess;
}


void IntelHdaDsp::ctrlPpcapEnable(bool enable)
{
    UInt32 val = enable ? SOF_HDA_PPCTL_GPROCEN : 0;
    dspUpdate(HDA_DSP_PP_BAR, SOF_HDA_REG_PP_PPCTL, SOF_HDA_PPCTL_GPROCEN, val);
}

void IntelHdaDsp::ctrlPpcapIntEnable(bool enable)
{
    UInt32 val = enable ? SOF_HDA_PPCTL_PIE : 0;
    dspUpdate(HDA_DSP_PP_BAR, SOF_HDA_REG_PP_PPCTL, SOF_HDA_PPCTL_PIE, val);
}

void IntelHdaDsp::ctrlMiscClockGating(bool enable)
{
    UInt32 val = enable ? PCI_CGCTL_MISCBDCGE_MASK : 0;
    pciCfgUpdate(PCI_CGCTL, PCI_CGCTL_MISCBDCGE_MASK, val);
}

/*
 * enable/disable audio dsp clock gating and power gating bits.
 * This allows the HW to opportunistically power and clock gate
 * the audio dsp when it is idle
 */
IOReturn IntelHdaDsp::ctrlClockPowerGating(bool enable)
{
    UInt32 val;

    /* enable/disable audio dsp clock gating */
    val = enable ? PCI_CGCTL_ADSPDCGE : 0;
    pciCfgUpdate(PCI_CGCTL, PCI_CGCTL_ADSPDCGE, val);

    /* disable the DMI link when requested. But enable only if it wasn't disabled previously */
    val = enable ? HDA_VS_INTEL_EM2_L1SEN : 0;
    if (!enable || !l1Disabled)
        dspUpdate(HDA_DSP_HDA_BAR, HDA_VS_INTEL_EM2,
                    HDA_VS_INTEL_EM2_L1SEN, val);

    /* enable/disable audio dsp power gating */
    val = enable ? 0 : PCI_PGCTL_ADSPPGD;
    pciCfgUpdate(PCI_PGCTL, PCI_PGCTL_ADSPPGD, val);

    return kIOReturnSuccess;
}
