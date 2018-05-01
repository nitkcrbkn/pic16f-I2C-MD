///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//木更津高専ロボット研究同好会
//I2C通信用関数
//Use device microchip PIC16F1938
//MPLAB X IDE(ver.2.30)
//HI-TECH C Compiler for PIC10/12/16 MCUs Version 9.80 in Lite mode
//Last updata 2015/5/13/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include<htc.h>
#include"I2C.h"
uint8_t rcv_data[RCV_DATA_LEN]; // 受信データバッファ
uint8_t snd_data[SND_DATA_LEN]; // 送信データバッファ


int8_t rcv_flg; // データ受信回数を保存するフラグ
uint8_t *Sdtp; // ���M�f�[�^�o�b�t�@�̃A�h���X�|�C���^�[
uint8_t *Rdtp; // ��M�f�[�^�o�b�t�@�̃A�h���X�|�C���^�[

int8_t AckCheck;
int8_t success;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//I2Cの初期化を行う関数
//�E����@		�E�L�q
//  Master  void I2C_init(unsigned char speed); spped = 通信速度(default 0x4f)(Fosc = 32MHz)
//  Slave   void I2C_init(unsigned char add);   add = スレーブ側のアドレス, サイズは1Byte
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  Master

void I2C_init(uint8_t speed)
#endif
#ifdef  Slave
void I2C_init(uint8_t add)
#endif
{
#ifdef Master
    SSPSTAT = 0x80;
    SSPCON1 = 0x28;
    SSPADD = speed;
    SSPIE = 1; // SSP(I2C)���荞�݂�������
    BCLIE = 1; // MSSP(I2C)�o�X�Փˊ��荞�݂�������
    PEIE = 1; // ���ӑ��u���荞�݂�������
    GIE = 1; // �S���荞�ݏ�����������
    SSPIF = 0; // SSP(I2C)���荞�݃t���O���N���A����
    BCLIF = 0; // MSSP(I2C)�o�X�Փˊ��荞�݃t���O���N���A����
    pinModeSCK = 1;
    pinModeSDA = 1;

#endif
#ifdef Slave
    SSPSTAT = 0x00;
    SSPCON1 = 0x26;
    SEN = 1;
    SSPADD = add << 1;
    SSPMSK = 0xfe;
    SSPIE = 1; // SSP(I2C)���荞�݂�������
    BCLIE = 1; // MSSP(I2C)�o�X�Փˊ��荞�݂�������
    PEIE = 1; // ���ӑ��u���荞�݂�������
    GIE = 1; // �S���荞�ݏ�����������
    SSPIF = 0; // SSP(I2C)���荞�݃t���O���N���A����
    BCLIF = 0;
    pinModeSCK = 1;
    pinModeSDA = 1;
#endif
}
/**************************************Master_Mode********************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Masterの割り込み関数
//  void interrupt **(void)�̒��ɓ���Ă��������B
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef Master

void Master_Interrupt(void) {
    if (SSPIF == 1) { // SSP(I2C)���荞�݂��������������f
        if (AckCheck == 1) AckCheck = 0;
        SSPIF = 0; // �t���O�N���A
    } else if (BCLIF == 1) { // MSSP(I2C)�o�X�Փˊ��荞�݂��������������f
        BCLIF = 0; // ����̓t���O�̂݃N���A����(������)
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ans = I2C_Send(adrs,len,*buf) : I2C送信関数
//
//  adrs = 送信先アドレス
//  len  = 送信データの長さ
//  buf  = 送信データのバッファポインタ
//  ans  = 0:正常終了, 1:応答無し, 2:受信拒否
//  ���M�������ƁAans�ŏ�Ԃ��m�F
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t I2C_Send(uint8_t adrs, uint8_t len, uint8_t *buf) {
    uint8_t i;
    int8_t ans;

    // �X�^�[�g(START CONDITION)
    I2C_IdleCheck(0x5);
    SSPCON2bits.SEN = 1;
    // [�X���[�u�̃A�h���X+�X���[�u�͎�M��v��]�𑗐M����
    I2C_IdleCheck(0x5);
    AckCheck = 1;
    SSPBUF = (uint8_t) (adrs << 1); // �A�h���X�𑗐M R/W=0
    while (AckCheck); // ���肩���ACK�ԓ���҂�
    ans = SSPCON2bits.ACKSTAT;
    if (ans == 0) {
        for (i = 0; i < len; i++) { // [�f�[�^]�𑗐M����
            I2C_IdleCheck(0x5);
            AckCheck = 1;
            SSPBUF = (unsigned char) *buf; // �f�[�^�𑗐M
            buf++;
            while (AckCheck); // ���肩���ACK�ԓ���҂�
            ans = SSPCON2bits.ACKSTAT;
            if (ans != 0) {
                ans = 2; // ���肪NOACK��Ԃ���
                success = 1; //�m�F�p
                break;
            }
        }
    }
    // �X�g�b�v(STOP CONDITION)
    I2C_IdleCheck(0x5);
    SSPCON2bits.PEN = 1;
    return ans;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//int I2C_Receive(adrs,len,*buf) : I2C受信関数
//  data = ��M�f�[�^�ۊǗp�ϐ�
//  adrs = 受信先アドレス
//  len  = 受信データの長さ
//  ans  = 0:正常終了, 1:応答無し, 2:送信拒否
//  *buf = 受信データを入れるバッファポインタ
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t I2C_Receive(uint8_t adrs, uint8_t len, uint8_t *buf) {
    //	 uint8_t data = 0 ;

    uint8_t i, ans;

    // �X�^�[�g(START CONDITION)
    I2C_IdleCheck(0x5);
    SSPCON2bits.SEN = 1;
    // [�X���[�u�̃A�h���X+�X���[�u�փf�[�^�v��]�𑗐M����
    I2C_IdleCheck(0x5);
    AckCheck = 1;
    SSPBUF = (char) ((adrs << 1) + 1); // �A�h���X�𑗐M R/W=1
    while (AckCheck); // ���肩���ACK�ԓ���҂�
    ans = SSPCON2bits.ACKSTAT;
    if (ans == 0) {
        for (i = 0; i < len; i++) { // [�f�[�^]����M����
            I2C_IdleCheck(0x5);
            SSPCON2bits.RCEN = 1; // ��M��������
            I2C_IdleCheck(0x4);
            *(buf + i) = SSPBUF; // ��M
            //data = SSPBUF; //��Ma
            //buf++ ;
            I2C_IdleCheck(0x5);
            if (i = len) SSPCON2bits.ACKDT = 1; // ACK�f�[�^��NOACK
            else SSPCON2bits.ACKDT = 0; // ACK�f�[�^��ACK
            SSPCON2bits.ACKEN = 1; // ACK�f�[�^��Ԃ�
        }
    }
    // �X�g�b�v(STOP CONDITION)
    I2C_IdleCheck(0x5);
    SSPCON2bits.PEN = 1;
    return ans;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//I2C_IdleCheck(char mask) : I2Cの待機関数
//  ACKEN RCEN PEN RSEN SEN R/W BF ���S�ĂO�Ȃ�n�j
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void I2C_IdleCheck(uint8_t mask) {
    while ((SSPCON2 & 0x1F) | (SSPSTAT & mask));
}
#endif
/***************************************Slave_Mode*********************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Slaveの割り込み関数
//  void interrupt **(void)�̒��ɓ���Ă��������B
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef Slave

void Slave_Interrupt(void) {
    char x;

    /*** SSP(I2C)���荞�ݔ������̏���***/
    if (SSPIF == 1) { //
        if (SSPSTATbits.R_nW == 0) {
            /******* �}�X�^�[����̏������ݗv��(�X���[�u�͎�M) *******/
            if (SSPSTATbits.D_nA == 0) {
                // ��M�o�C�g�̓A�h���X
                Rdtp = (char *) rcv_data;
                x = SSPBUF; // �A�h���X�f�[�^����ǂ݂���
                rcv_flg = 0;
            } else {
                // ��M�o�C�g�̓f�[�^
                *Rdtp = SSPBUF; // �f�[�^��Ǎ���(ACK��PIC�������I�ɕԂ�)
                Rdtp++;
                rcv_flg++;
            }
            SSPIF = 0; // �����݃t���O�N���A
            SSPCON1bits.CKP = 1; // SCL���C�����J������(�ʐM�̍ĊJ)
        } else {
            /******* �}�X�^�[����̓ǂݏo���v��(�X���[�u�͑��M) *******/
            if (SSPSTATbits.BF == 1) {
                // �A�h���X��M��̊��荞�݂Ɣ��f����
                Sdtp = (char *) snd_data;
                x = SSPBUF; // �A�h���X�f�[�^����ǂ݂���
                while ((SSPCON1bits.CKP) | (SSPSTATbits.BF));
                SSPBUF = *Sdtp; // ���M�f�[�^�̃Z�b�g
                Sdtp++;
                SSPIF = 0; // �����݃t���O�N���A
                SSPCON1bits.CKP = 1; // SCL���C�����J������(�ʐM�̍ĊJ)
            } else {
                // �f�[�^�̑��M���ACK�󂯎��ɂ�銄�荞�݂Ɣ��f����
                if (SSPCON2bits.ACKSTAT == 0) {
                    // �}�X�^�[����ACK�����Ȃ玟�̃f�[�^�𑗐M����
                    while ((SSPCON1bits.CKP) | (SSPSTATbits.BF));
                    SSPBUF = *Sdtp; // ���M�f�[�^�̃Z�b�g
                    Sdtp++;
                    SSPIF = 0; // �����݃t���O�N���A
                    SSPCON1bits.CKP = 1; // SCL���C�����J������(�ʐM�̍ĊJ)
                } else {
                    // �}�X�^�[�����NOACK�ŉ������ꂽ��
                    SSPIF = 0; // �����݃t���O�N���A
                }
            }
        }
    }

    /* MSSP(I2C)�o�X�Փˊ��荞�������̏���*/
    if (BCLIF == 1) {
        BCLIF = 0; // ����̓t���O�̂݃N���A����(������)
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//int I2C_ReceiveCheck() : データ受信回数をチェックしてrcv_flgをクリアする関数
//  ans = データ受信回数
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t I2C_ReceiveCheck() {
    int ans;

    ans = 0;
    if (rcv_flg != 0) { // 受信したデータがあったら
        if ((SSPSTATbits.S == 0)&&(SSPSTATbits.P == 1)) { // �į�ߺ��ި��ݔ��s���ꂽ����
            ans = rcv_flg;
            rcv_flg = 0;
        }
    }
    return (ans);
}
#endif
