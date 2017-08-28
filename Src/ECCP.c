#include<pic16f1938.h>
#include"ECCP.h"

void PWMInit(void){
  ANSELAbits.ANSA3=0;//ディジタル入出力にセット
  ANSELAbits.ANSA4=0;
  TRISAbits.TRISA3=0;//出力として設定、CCP1の出力を無効
  TRISAbits.TRISA4=0;

  PR2 = 255;//PWM周期の値を設定
  CCP1CON = 0b01001111; //正転フルブリッジ出力　全出力アクティブLow

  /* タイマ２を起動、設定 */
  TMR2IF=0;//TMR2IF割り込みフラグビットをクリア
  CCPTMRS0 = 0b11111100;//CCP1はPWMモードでTimer2を使用
  CCPTMRS1 = 0b00000011;//予約
  T2CON = 0b00000100;//プリスケーラを1:1に設定、Timer2を有効化

  PWM1CON = 0b00000000; //自動シャットダウンの設定
  //PSTR1CON = 0b00011111;ステアリング制御

  while(!TMR2IF);//タイマがオーバーフローしてTMR2Fビットがセットされるのをまつ

  /* 対応するビットをクリアしてCCP1ピン出力ドライバを有効化 */
  TRISCbits.TRISC2=0;//CCP1/P1A
  TRISBbits.TRISB2=0;//P1B
  TRISBbits.TRISB1=0;//P1C
  TRISBbits.TRISB4=0;//P2D
}

void PWMSet(uint16_t duty, drive_mode MODE) {
  if (duty > MAX_DUTY)
    duty = MAX_DUTY;

  switch(MODE){

  /* フリー動作 */
  case FREE_MODE:
      LED1 = 0; //LEDで回転方向を示す
      LED2 = 0;
      CCP1CON = 0b01000000; //正転　PWMはオフ（ECCPxモジュールをリセット）
      CCPR1L = 0; //デューティ値を0とする
      P1A = 0;//出力をクリア
      P1B = 0;
      P1C = 0;
      P1D = 0;
      break;

    /* 正転 */
  case FORWARD_MODE:
      LED1 = 1;
      LED2 = 0;
      CCP1CON = 0b01001100; //正転フルブリッジ出力　全出力アクティブHigh
      CCPR1L = duty >> 2; //デューティ値の上位８ビットを読み込む
      DC1B0 = duty & 0b00000001;//デューティ値の下位２ビットを読み込む
      DC1B1 = (duty & 0b00000010) >> 1;
      break;

    /* 逆転 */
  case BACK_MODE:
      LED1 = 0;
      LED2 = 1;
      CCP1CON = 0b11001100;//逆転フルブリッジ出力　全出力アクティブHigh
      CCPR1L = duty >> 2;
      DC1B0 = duty & 0b00000001;
      DC1B1 = (duty & 0b00000010) >> 1;
      break;


    /* ブレーキ */
  case BRAKE_MODE:
      LED1 = 1;
      LED2 = 1;
      CCP1CON = 0b01000000; //正転　PWMはオフ（ECCPxモジュールをリセット）
      CCPR1L = 0;
      P1A = 0;
      P1B = 0;
      P1C = 0;
      P1D = 0;

      P1A = 0;//P1A,P1CをLow P1B,P1DをHighにセット
      P1B = 1;
      P1C = 0;
      P1D = 1;

      break;
  }

}


