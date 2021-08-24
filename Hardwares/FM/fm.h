//
// Created by yekai on 2021/8/16.
//

#ifndef SMART_CONFIG_FM_H
#define SMART_CONFIG_FM_H

#include "main.h"

#define RADIO_REG_CTRL    0x02
#define RADIO_REG_CTRL_OUTPUT 0x8000
#define RADIO_REG_CTRL_UNMUTE 0x4000
#define RADIO_REG_CTRL_MONO   0x2000
#define RADIO_REG_CTRL_BASS   0x1000
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK   0x0100
#define RADIO_REG_CTRL_RDS    0x0008
#define RADIO_REG_CTRL_NEW    0x0004
#define RADIO_REG_CTRL_RESET  0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001

#define RADIO_REG_CHAN    0x03
#define RADIO_REG_CHAN_SPACE     0x0003
#define RADIO_REG_CHAN_SPACE_100 0x0000
#define RADIO_REG_CHAN_BAND      0x000C
#define RADIO_REG_CHAN_BAND_US      0x0000
#define RADIO_REG_CHAN_BAND_JP      0x0004
#define RADIO_REG_CHAN_BAND_WW       0x0008
#define RADIO_REG_CHAN_BAND_EE      0x000C
#define RADIO_REG_CHAN_TUNE   0x0010
//      RADIO_REG_CHAN_TEST   0x0020
#define RADIO_REG_CHAN_NR     0x7FC0

#define RADIO_REG_R4    0x04
#define RADIO_REG_R4_EM50   0x0800
//      RADIO_REG_R4_RES   0x0400
#define RADIO_REG_R4_SOFTMUTE   0x0200
#define RADIO_REG_R4_AFC   0x0100


#define RADIO_REG_VOL     0x05
#define RADIO_REG_VOL_VOL   0x000F


#define RADIO_REG_RA      0x0A
#define RADIO_REG_RA_RDS       0x8000
#define RADIO_REG_RA_RDSBLOCK  0x0800
#define RADIO_REG_RA_STEREO    0x0400
#define RADIO_REG_RA_NR        0x03FF

#define RADIO_REG_RB          0x0B
#define RADIO_REG_RB_FMTRUE   0x0100
#define RADIO_REG_RB_FMREADY  0x0080


#define RADIO_REG_RDSA   0x0C
#define RADIO_REG_RDSB   0x0D
#define RADIO_REG_RDSC   0x0E
#define RADIO_REG_RDSD   0x0F


// ----- type definitions -----

/// Band datatype.
/// The BANDs a receiver probably can implement.
enum RADIO_BAND {
    RADIO_BAND_US = 0x01,       ///< FM band 87 - 108 MHz (USA, Europe) selected.
    RADIO_BAND_JP = 0x02,       ///< FM band 76 - 91 MHz (Japan) selected.
    RADIO_BAND_WW = 0x03,       ///< FM band 76 - 108 MHz (World Wide) selected.
    RADIO_BAND_EE = 0x04,       ///< FM band 65 - 76 MHz (East Europe) selected.
};


/// Frequency data type. Unit:kHz
/// Only 16 bits are used for any frequency value (not the real one)
typedef uint32_t RADIO_FREQ;


/// A structure that contains information about the radio features from the chip.
typedef struct RADIO_INFO {
    bool active;                ///< receiving is active.
    uint8_t rssi;               ///< Radio Station Strength Information.
    uint8_t snr;                ///< Signal Noise Ratio.
    bool rds;                   ///< RDS information is available.
    bool tuned;                 ///< A stable frequency is tuned.
    bool mono;                  ///< Mono mode is on.
    bool stereo;                ///< Stereo audio is available
} RADIO_INFO;


/// a structure that contains information about the audio features
typedef struct AUDIO_INFO {
    uint8_t volume;
    bool mute;
    bool softmute;
    bool bassBoost;
} AUDIO_INFO;

typedef struct RDA_Handler {
    enum RADIO_BAND band;
    uint8_t channelSpacing;
    RADIO_FREQ freq;
    struct RADIO_INFO radioInfo;
    struct AUDIO_INFO audioInfo;
    // from 0x02 high to 0x07 low
    uint16_t regList[8];
} RDA_Handler;

void show_fm(void *pvParameters);

void RDA_ReadAllInfo();

void RDA_WriteAllInfo();

void RDA_Init();

void RDA_Reset();

void RDA_SetBandFrequency(enum RADIO_BAND newBand, RADIO_FREQ newFreq);

void RDA_SetFrequency(RADIO_FREQ newFreq);

void RDA_SetBand(enum RADIO_BAND newBand);

extern struct RDA_Handler RDA5807;

#endif //SMART_CONFIG_FM_H
