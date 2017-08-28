#include "main.h"
#include <xc.h>
#include <stdbool.h>

#define ADR 0x10  //アドレス

void init(void);
void SendToMotor(uint16_t speed,uint8_t stat);

void main(void) {
    uint16_t speed;
    uint8_t mode;
    uint8_t com_flg = 0;

    init();

    I2C_init(ADR);//アドレス
    PWMInit();

    mode=0;

    __delay_ms(100);

    while(true){

        if(I2C_ReceiveCheck()){
            if (com_flg == 0) com_flg = 1;
            speed = (rcv_data[0]&0x03)<<8;//STM仕様に変更
            speed |= rcv_data[1];
            mode = (rcv_data[0] >> 2) & 0x03;
            CLRWDT();
        }
        else if (com_flg == 0){
            CLRWDT();
        }
        PWMSet(speed,mode);
    }
}

void init(void){
  // Set oscilation
  OSCCON = 0xF0; //PLL　Enable

  // Set pin mode
  ANSELA = 0x00;
  ANSELB = 0x00;

  // Set watch dog
  WDTCON = 0x13;
}

void interrupt  HAND(void){
    Slave_Interrupt();
}