#include "USB_Serial.h"

void USB_Serial_Init(){

}

uint8_t USB_Serial_Read(uint8_t *buffer){
    uint8_t byte = (uint8_t) tud_cdc_n_read_char(0);
    //printf("USB Get Data:0x%02x\r\n",byte);
    if(byte == 0xD0){
        *buffer = (uint8_t) (tud_cdc_n_read_char(0)+1);
        return true;
    }
    *buffer = byte;
    return false;
}

void USB_Serial_Write(uint8_t byte){
    if(byte == 0xE0||byte == 0xD0){
        tud_cdc_n_write_char(0,(char)0xD0);
        tud_cdc_n_write_char(0,(char)(byte -1));
    }
    else
    {
        tud_cdc_n_write_char(0,(char)byte);
    }
}

void USB_Serial_WriteHead(){
    tud_cdc_n_write_char(0,(char)0xE0);
}

uint8_t USB_Serial_Available(){
    uint8_t avail = tud_cdc_n_available(0);
    if(avail == 1){
        uint8_t peek;
        tud_cdc_n_peek(0,&peek);
        if(peek == 0xD0)return false;
        return true;
    }else if(avail > 0){
        return true;
    }
    return false;
}

uint8_t USB_Serial_Flush(){
    tud_cdc_n_write_flush(0);
}
