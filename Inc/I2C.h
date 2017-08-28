//木更津高専ロボット研究同好会　
//I2C通信用関数
//Use device microchip PIC16F1938
//MPLAB X IDE(ver.2.30)
//HI-TECH C Compiler for PIC10/12/16 MCUs Version 9.80 in Lite mode  
//Last updata 2015/5/12/

#ifndef I2C_H
#define	I2C_H

#include <stdint.h>

//#define Master
#define Slave

#define pinModeSCK TRISCbits.TRISC3
#define pinModeSDA TRISCbits.TRISC4

#define SND_DATA_LEN 8                  //!< 送信データバッファのサイズ
#define RCV_DATA_LEN 8                  //!< 受信データバッファのサイズ

/////////スレーブの受信データ、送信データ格納配列/////////////////
uint8_t rcv_data[RCV_DATA_LEN] ;  //!< 受信データバッファ
uint8_t snd_data[SND_DATA_LEN] ;  //!< 送信データバッファ

/*--------------------Master_Mode-----------------*/
#ifdef  Master
/**
マスターの割り込み用関数
 */
void Master_Interrupt(void);

/**
I2Cの送信関数
\param adrs 送信先アドレス
\param len 長さ
\param buf 送信データのはいいているunsigned char型のポインタ
\return 送信時応答 0...正常終了　1...応答なし　2...受信拒否
*/
uint8_t I2C_Send(uint8_t adrs,uint8_t len,uint8_t *buf);

/**
I2Cの受信関数
\param adrs 受信先アドレス
\param len 長さ
\param buf 受信データを入れるバッファのunsigned char型のポインタ
\return 受信時応答 0...正常終了　1...応答なし　2...送信拒否
*/
uint8_t I2C_Receive(uint8_t adrs,uint8_t len,uint8_t *buf);

/**
I2Cのwait
\param mask unknown
 */
void I2C_IdleCheck(uint8_t mask);
#endif

/*--------------------------Slave_Mode------------------------------*/
#ifdef Slave
/**
スレーブの割り込み用関数
 */
void Slave_Interrupt( void );

/**
受信チェック関数
\return 受信したデータの個数で0だったら受信していない
 */
uint8_t I2C_ReceiveCheck() ;
#endif
//--------------------------Master Mode----------------------------*/
#ifdef  Master
/**
マスター初期化関数
\param speed 速度(16MHz...26くらい)
*/
void I2C_init(uint8_t speed);

#endif
#ifdef  Slave
/**
スレーブ初期化関数
\param add アドレス
*/
void I2C_init(uint8_t add);
#endif

#endif	/* I2C_H */

