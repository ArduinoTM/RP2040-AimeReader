/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "bsp/board.h"
#include "tusb.h"

#include "USB_Serial.h"
#include "aime_reader.h"

//------------- prototypes -------------//
static void cdc_task(void);

/*------------- MAIN -------------*/
int main(void)
{
  board_init();

  tusb_init();

  uart_init(uart0, 115200);
  uart_init(uart1, 115200);
  gpio_set_function(0,GPIO_FUNC_UART);
  gpio_set_function(1,GPIO_FUNC_UART);
  gpio_set_function(4,GPIO_FUNC_UART);
  gpio_set_function(5,GPIO_FUNC_UART);

  printf("Hello World!\r\n");

  while (1)
  {
    tud_task(); // tinyusb device task
    Aime_Process(); //
    //cdc_task();
  }

  return 0;
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static void cdc_task(void)
{
  static uint8_t data;
  char strBuffer[100];
  if(USB_Serial_Available()){
    USB_Serial_Read(&data);
    // sprintf(strBuffer,"USB Get Data:0x%02x\r\n",data);
    // uart_puts(uart0,strBuffer);
    printf("USB Get Data:0x%02x\r\n",data);
    USB_Serial_Write(data);
    USB_Serial_Flush();
  }
}
