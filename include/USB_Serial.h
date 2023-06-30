#ifndef __USB_SERIAL_H__
#define __USB_SERIAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "tusb.h"

void USB_Serial_Init();
uint8_t USB_Serial_Read(uint8_t *buffer);
void USB_Serial_Write(uint8_t byte);
void USB_Serial_WriteHead();
uint8_t USB_Serial_Available();
void USB_Serial_Flush();

#endif // !__USB_SERIAL