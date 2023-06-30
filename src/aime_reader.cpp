#include "aime_reader.h"
#include "aime_cmd.h"

#include <PicoLed.hpp>
#include <PN532_HSU.h>
#include <PN532.h>

#include "USB_Serial.h"

PN532_HSU pn532_hsu;
PN532 nfc(pn532_hsu);

#define CARD_LIGHT_PIN 2

auto card_light = PicoLed::addLeds<PicoLed::WS2812B>(pio1, 0,CARD_LIGHT_PIN, 16, PicoLed::FORMAT_GRB);

static packet_req_t req;
static packet_res_t res;

void Aime_Process_Packet();

void sg_res_init(uint8_t payload_len);
void sg_nfc_cmd_reset();
void sg_nfc_cmd_get_fw_version();
void sg_nfc_cmd_get_hw_version();
void sg_nfc_cmd_mifare_set_key_aime();
void sg_nfc_cmd_mifare_set_key_bana();
void sg_led_cmd_reset();
void sg_led_cmd_get_info();
void sg_led_cmd_set_color();
void sg_nfc_cmd_radio_on();
void sg_nfc_cmd_radio_off();
void sg_nfc_cmd_poll();
void sg_nfc_cmd_mifare_select_tag();
void sg_nfc_cmd_aime_authenticate();
void sg_nfc_cmd_bana_authenticate();
void sg_nfc_cmd_mifare_read_block();
void sg_nfc_cmd_felica_encap();

void Aime_Process_Packet()
{
    // printf("Aime_Process_Packet!Lenth:%d,Req_addr:0x%x,Req_Num:%d,CMD:0x%x,Payload_Len:%d\r\n",req.frame_len,req.addr,req.seq_no,req.cmd,req.payload_len);
    switch (req.cmd)
    {
    case kNFCCMD_TO_NORMAL_MODE:
    {
        printf("/----------------------------\r\n");
        printf("Aime Board: To Normal Mode\n");
        sg_nfc_cmd_reset();
        // printf("Aime_Process_CALLBack!frame_len:%d,addr:0x%x,seq_num:%d,cmd:%x,ststus:%d,payload_len:%d\r\n",\
        // res.frame_len,res.addr,res.seq_no,res.status,res.payload_len);
        break;
    }
    case kNFCCMD_GET_FW_VERSION:
        // printf("Aime Board: Get FW Version\n");
        sg_nfc_cmd_get_fw_version();
        break;
    case kNFCCMD_GET_HW_VERSION:
        // printf("Aime Board: Get HW Version\n");
        sg_nfc_cmd_get_hw_version();
        break;
    case kNFCCMD_CARD_DETECT:
        // printf("Aime Board: Card Detect\n");
        sg_nfc_cmd_poll();
        break;
    case kNFCCMD_MIFARE_READ:
        // printf("Aime Board: Mifare Read\n");
        sg_nfc_cmd_mifare_read_block();
        break;
    case kNFCCMD_NFC_THROUGH:
        // printf("Aime Board: NFC Though\n");
        sg_nfc_cmd_felica_encap();
        break;
    case kNFCCMD_MIFARE_AUTHORIZE_B:
        // printf("Aime Board: Aime Auth\n");
        sg_nfc_cmd_aime_authenticate();
        break;
    case kNFCCMD_MIFARE_AUTHORIZE_A:
        // printf("Aime Board: Bana Auth\n");
        sg_nfc_cmd_bana_authenticate();
        break;
    case kNFCCMD_CARD_SELECT:
        // printf("Aime Board: Select Tag\n");
        sg_nfc_cmd_mifare_select_tag();
        break;
    case kNFCCMD_MIFARE_KEY_SET_B:
        // printf("Aime Board: Set Aime Key\n");
        sg_nfc_cmd_mifare_set_key_aime();
        break;
    case kNFCCMD_MIFARE_KEY_SET_A:
        // printf("Aime Board: Set Bana Key\n");
        sg_nfc_cmd_mifare_set_key_bana();
        break;
    case kNFCCMD_START_POLLING:
        // printf("Aime Board: Start Polling\n");
        sg_nfc_cmd_radio_on();
        break;
    case kNFCCMD_STOP_POLLING:
        // printf("Aime Board: Stop Polling\n");
        sg_nfc_cmd_radio_off();
        break;
    case kNFCCMD_EXT_TO_NORMAL_MODE:
        printf("Aime LED Board: To Normal Mode\n");
        sg_led_cmd_reset();
        break;
    case kNFCCMD_EXT_BOARD_INFO:
        printf("Aime LED Board: Get Board Info\n");
        sg_led_cmd_get_info();
        break;
    case kNFCCMD_EXT_BOARD_LED_RGB:
        printf("Aime LED Board: Set LED RGB Color\n");
        sg_led_cmd_set_color();
        break;
    default:
        sg_res_init(0);
        // printf("Aime Default Orpran\r\n");
        break;
    }
    

    if (res.cmd == 0)
        return;

    uint8_t checksum = 0;

    USB_Serial_WriteHead();
    for (uint8_t i = 0; i < res.frame_len; i++)
    {
        checksum += res.bytes[i];
        USB_Serial_Write(res.bytes[i]);
    }
    USB_Serial_Write(checksum);
    USB_Serial_Flush();

    res.cmd = 0;
}

uint8_t in_size, checksum;
void Aime_Process(){

    while(USB_Serial_Available()){

        uint8_t packet;
        uint8_t is_escaped = USB_Serial_Read(&packet);
        if(packet == 0xE0 && !is_escaped){              //包头是E0还是0E？
            in_size = 0;
            checksum = 0;
            // printf("//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");
            // printf("GET 0xE0! Package RESET!\r\n");
            continue;
        }
        
        //printf("Aime_Reader: Recv! Packet:0x%02x,%drd Byte,Packet_len:%d,is_escaped:0x%02x,checksum:0X%02x\r\n",packet,in_size,req.frame_len,is_escaped,checksum);
        //printf("In to Aime_Process?%d\r\n",(in_size > 2 && in_size - 1 == req.frame_len && checksum == packet));
        req.bytes[in_size++] = packet;

        //printf("Aime Reader: Recv %d bytes, checksum %d\n", in_size, checksum);
        if(in_size > 2 && (in_size - 1 == req.frame_len) && (checksum == packet)){
            Aime_Process_Packet();
        }

        checksum += packet;

    }

}


void sg_res_init(uint8_t payload_len) { //初始化模板
    res.frame_len = 6 + payload_len;
    res.addr = req.addr;
    res.seq_no = req.seq_no;
    res.cmd = req.cmd;
    res.status = 0;
    res.payload_len = payload_len;
}

void sg_nfc_cmd_reset() { //重置读卡器
    card_light.setBrightness(0x80);
    card_light.clear();
    printf("LED Color Clear!\r\n");
    //card_light.show();
    nfc.begin();
    nfc.setPassiveActivationRetries(0x10); //设定等待次数,0xFF永远等待
    nfc.SAMConfig();
    if (nfc.getFirmwareVersion()) {
        nfc.SAMConfig();
        sg_res_init(0);
        res.status = 3;
        return;
    }
    // FastLED.showColor(0xFF0000);
}

void sg_nfc_cmd_get_fw_version() {
    sg_res_init(23);
    memcpy(res.version, "TN32MSEC003S F/W Ver1.2", 23);
    //  sg_res_init(1);
    //  memset(res.version, 0x94, 1);
}

void sg_nfc_cmd_get_hw_version() {
    sg_res_init(23);
    memcpy(res.version, "TN32MSEC003S H/W Ver3.0", 23);
    //  sg_res_init(9);
    //  memcpy(res.version, "837-15396", 9);
}

void sg_nfc_cmd_mifare_set_key_aime() {
    sg_res_init(0);
    memcpy(AimeKey, req.key, 6);
}

void sg_nfc_cmd_mifare_set_key_bana() {
    sg_res_init(0);
    memcpy(BanaKey, req.key, 6);
}

void sg_led_cmd_reset() {
    sg_res_init(0);
    // FastLED.showColor(0);
}

void sg_led_cmd_get_info() {
    sg_res_init(9);
    static uint8_t info[9] = {'1', '5', '0', '8', '4', 0xFF, 0x10, 0x00, 0x12};
    memcpy(res.info_payload, info, 9);
}

void sg_led_cmd_set_color() {
    card_light.fill(PicoLed::RGB(req.color_payload[0], req.color_payload[1], req.color_payload[2]));
    card_light.show();
}

void sg_nfc_cmd_radio_on() {
    sg_res_init(0);
    nfc.setRFField(0x00, 0x01);
}

void sg_nfc_cmd_radio_off() {
    sg_res_init(0);
    nfc.setRFField(0x00, 0x00);
}

void sg_nfc_cmd_poll() { //卡号发送
    uint16_t SystemCode;
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, res.mifare_uid, &res.id_len)) {
        sg_res_init(0x07);
        res.count = 1;
        res.type = 0x10;
        return;
    } else if (nfc.felica_Polling(0xFFFF, 0x00, res.IDm, res.PMm, &SystemCode, 0x0F) == 1) {//< 0: error
        sg_res_init(0x13);
        res.count = 1;
        res.type = 0x20;
        res.id_len = 0x10;
        return;
    } else {
        sg_res_init(1);
        res.count = 0;
        return;
    }
}

void sg_nfc_cmd_mifare_select_tag() {
    sg_res_init(0);
}

void sg_nfc_cmd_aime_authenticate() {
    sg_res_init(0);
    //AuthenticateBlock(uid,uidLen,block,keyType(A=0,B=1),keyData)
    if (nfc.mifareclassic_AuthenticateBlock(req.uid, 4, req.block_no, 1, AimeKey)) {
        return;
    } else {
        res.status = 1;
    }
}

void sg_nfc_cmd_bana_authenticate() {
    sg_res_init(0);
    //AuthenticateBlock(uid,uidLen,block,keyType(A=0,B=1),keyData)
    if (nfc.mifareclassic_AuthenticateBlock(req.uid, 4, req.block_no, 0, BanaKey)) {
        return;
    } else {
        res.status = 1;
    }
}

void sg_nfc_cmd_mifare_read_block() {//读取卡扇区数据
    if (nfc.mifareclassic_ReadDataBlock(req.block_no, res.block)) {
        sg_res_init(0x10);
        return;
    }
    sg_res_init(0);
    res.status = 1;
}

void sg_nfc_cmd_felica_encap() {
    uint16_t SystemCode;
    if (nfc.felica_Polling(0xFFFF, 0x01, res.encap_IDm, res.encap_PMm, &SystemCode, 0x0F) == 1) {
        SystemCode = SystemCode >> 8 | SystemCode << 8;//SystemCode，大小端反转注意
    } else {
        sg_res_init(0);
        res.status = 1;
        return;
    }
    uint8_t code = req.code;
    res.code = code + 1;
    switch (code) {
        case FELICA_CMD_POLL:
            sg_res_init(0x14);
            memcpy(res.system_code, &SystemCode, 2);
            break;
        case FELICA_CMD_GET_SYSTEM_CODE:
            sg_res_init(0x0D);
            res.felica_payload[0] = 0x01;//未知
            res.felica_payload[1] = SystemCode & 0xFF;//SystemCode
            res.felica_payload[2] = SystemCode >> 8;
            break;
        case FELICA_CMD_NDA_A4:
            sg_res_init(0x0B);
            res.felica_payload[0] = 0x00;
            break;
        case FELICA_CMD_NDA_06:
            sg_res_init(0x1D);
            memcpy(res.NDA06_Data, res.encap_PMm, 8);//未知，填补数据用
            memcpy(res.NDA06_IDm, res.encap_IDm, 8);
            res.NDA06_code[0] = 0x00;
            res.NDA06_code[1] = 0x00;
            res.NDA06_code[2] = 0x01;//未知
            break;
        case FELICA_CMD_NDA_08:
            sg_res_init(0x0C);
            res.felica_payload[0] = 0x00;
            res.felica_payload[1] = 0x00;
            break;
        default:
            sg_res_init(0);
            res.status = 1;
    }
    res.encap_len = res.payload_len;
}
