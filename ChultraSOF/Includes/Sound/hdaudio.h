//
//  hdaudio.h
//  ChultraSOF
//
//  Created by Gwydien on 11/10/23.
//

#pragma once

#include <IOKit/IOMemoryDescriptor.h>
#include <IOKit/audio/IOAudioTypes.h>

/*
 * HD-audio stream
 */
struct hdac_stream {
    IOMemoryDescriptor *bdl; /* BDL buffer */
    UInt32 *posbuf;        /* position buffer pointer */
    IOAudioStreamDirection direction;        /* playback / capture (SNDRV_PCM_STREAM_*) */

    unsigned int bufsize;    /* size of the play buffer in bytes */
    unsigned int period_bytes; /* size of the period in bytes */
    unsigned int frags;    /* number for period in the play buffer */
    unsigned int fifo_size;    /* FIFO size */

    IOVirtualAddress sd_addr;    /* stream descriptor pointer */

    IOVirtualAddress spib_addr; /* software position in buffers stream pointer */
    IOVirtualAddress fifo_addr; /* software position Max fifos stream pointer */

    IOVirtualAddress dpibr_addr; /* DMA position in buffer resume pointer */
    UInt32 dpib;        /* DMA position in buffer */
    UInt32 lpib;        /* Linear position in buffer */

    UInt32 sd_int_sta_mask;    /* stream int status mask */

    /* pcm support */
//    struct snd_pcm_substream *substream;    /* assigned substream,
//                         * set in PCM open
//                         */
//    struct snd_compr_stream *cstream;
    unsigned int format_val;    /* format value to be set in the
                     * controller and the codec
                     */
    unsigned char stream_tag;    /* assigned stream */
    unsigned char index;        /* stream index */
    int assigned_key;        /* last device# key assigned to */

    bool opened:1;
    bool running:1;
    bool prepared:1;
    bool no_period_wakeup:1;
    bool locked:1;
    bool stripe:1;            /* apply stripe control */

    UInt64 curr_pos;
    /* timestamp */
    unsigned long start_wallclk;    /* start + minimum wallclk */
    unsigned long period_wallclk;    /* wallclk for period */
//    struct timecounter  tc;
//    struct cyclecounter cc;
    int delay_negative_threshold;

//    struct list_head list;
#ifdef CONFIG_SND_HDA_DSP_LOADER
    /* DSP access mutex */
//    struct mutex dsp_mutex;
#endif
};
