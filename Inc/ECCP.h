/* 
 * File:   ECCP.h
 * Author: seimi
 *
 * Created on 2016/06/28, 17:03
 */

#ifndef ECCP_H
#define ECCP_H

#include <stdint.h>

#define P1A LATC2	//ECCPの出力ポート
#define P1B LATB2
#define P1C LATB1
#define P1D LATB4

#define LED1 LATA3	//状態表示用LEDポート
#define LED2 LATA4

#define MAX_DUTY 999//デューティ値の上限は1023だが、回転方向変更時、危険なため制限

typedef enum{
  FREE_MODE,//フリー動作
  FORWARD_MODE,//正転
  BACK_MODE,//逆転
  BRAKE_MODE,//ブレーキ
} drive_mode;

void PWMInit(void);
//機能: PWMを使うための初期設定

void PWMSet(uint16_t duty, drive_mode MODE);
//機能: モーターの方向、回転数を制御する関数。
//引数: duty...デューティ値(0~999) dire 動作(0->フリー動作　1->正転　2->逆転　3->ブレーキ)
#endif	/* ECCP_H */