// #include <PN532_HSU.h>
// #include <PN532.h>
// #include "stdinclude.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

uint8_t AimeKey[6], BanaKey[6];

// PN532_HSU pn532_hsu;
// PN532 nfc(pn532_hsu);

// auto card_light = PicoLed::addLeds<PicoLed::WS2812B>(pio1, 0,
//                                                      CARD_LIGHT_PIN, 16, PicoLed::FORMAT_GRB);

enum {
    // kNFCCMD_NFC_THROUGH subcommands
    FELICA_CMD_POLL = 0x00,
    FELICA_CMD_NDA_06 = 0x06,
    FELICA_CMD_NDA_08 = 0x08,
    FELICA_CMD_GET_SYSTEM_CODE = 0x0C,
    FELICA_CMD_NDA_A4 = 0xA4,
};

typedef union packet_req {
    uint8_t bytes[128];
    struct {
        uint8_t frame_len;
        uint8_t addr;
        uint8_t seq_no;
        uint8_t cmd;
        uint8_t payload_len;
        union {
            uint8_t key[6]; //sg_nfc_req_mifare_set_key(bana or aime)
            uint8_t color_payload[3];//sg_led_req_set_color
            struct { //sg_nfc_cmd_mifare_select_tag,sg_nfc_cmd_mifare_authenticate,sg_nfc_cmd_mifare_read_block
                uint8_t uid[4];
                uint8_t block_no;
            };
            struct { //sg_nfc_req_felica_encap
                uint8_t IDm[8];
                uint8_t encap_len;
                uint8_t code;
                uint8_t felica_payload[113];
            };
        };
    };

} packet_req_t;

typedef union packet_res {
    uint8_t bytes[128];
    struct {
        uint8_t frame_len;
        uint8_t addr;
        uint8_t seq_no;
        uint8_t cmd;
        uint8_t status;
        uint8_t payload_len;
        union {
            char version[23]; //sg_nfc_res_get_fw_version,sg_nfc_res_get_hw_version
            uint8_t reset_payload; //sg_led_res_reset
            uint8_t info_payload[9]; //sg_led_res_get_info
            uint8_t block[16]; //sg_nfc_res_mifare_read_block
            struct { //sg_nfc_res_poll
                uint8_t count;
                uint8_t type;
                uint8_t id_len;
                union {
                    uint8_t mifare_uid[4];
                    struct {
                        uint8_t IDm[8];
                        uint8_t PMm[8];
                    };
                };
            };
            struct { //sg_nfc_res_felica_encap
                uint8_t encap_len;
                uint8_t code;
                uint8_t encap_IDm[8];
                union {
                    struct {//FELICA_CMD_POLL
                        uint8_t encap_PMm[8];
                        uint8_t system_code[2];
                    };
                    struct {//NDA06
                        uint8_t NDA06_code[3];
                        uint8_t NDA06_IDm[8];
                        uint8_t NDA06_Data[8];
                    };
                    uint8_t felica_payload[112];
                };
            };
        };
    };
} packet_res_t;