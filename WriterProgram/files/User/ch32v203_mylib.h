/*
 * ch32v203_mylib.h
 *
 *  Created on: Jun 30, 2024
 *      CITERA Programming KIT用システム
 *      
 */

#ifndef USER_CH32V203_MYLIB_H_
#define USER_CH32V203_MYLIB_H_

//ピン番号
#define PA0 0x10
#define PA1 0x11
#define PA2 0x12
#define PA3 0x13
#define PA4 0x14
#define PA5 0x15
#define PA6 0x16
#define PA7 0x17
#define PA8 0x18
#define PA9 0x19
#define PA10 0x1A
#define PA11 0x1B
#define PA12 0x1C
#define PA13 0x1D
#define PA14 0x1E
#define PA15 0x1F

#define PB0 0x20
#define PB1 0x21
#define PB3 0x23
#define PB4 0x24
#define PB5 0x25
#define PB6 0x26
#define PB7 0x27
#define PB8 0x28

#define PD0 0x40
#define PD1 0x41

//OUTPUT SPEED
#define OUTPUT_2MHz 0b10
#define OUTPUT_10MHz 0b01
#define OUTPUT_50MHz 0b11

//PINMODE
//0xQWER; Q:インプットかアウトプットか
//W:プルアップかプルダウンか(outputでは常に0)
//ER:CNFのビット
#define INPUT 0b1001
#define INPUT_PULLUP 0b1110
#define INPUT_PULLDOWN 0b1010
#define INPUT_ANALOG 0b1000
#define OUTPUT 0b0000
#define OUTPUT_OPENDRAIN 0b0001
#define OUTPUT_FUNC 0b0010
#define OUTPUT_OPENDRAIN_FUNC 0b0011

void GPIOA_init(void); //GPIOAのセットアップ
void GPIOB_init(void); //GPIOBのセットアップ
void GPIOC_init(void); //GPIOCのセットアップ
void GPIOD_init(void); //GPIODのセットアップ
int GPIO_output_speed(int speed); //GPIO OUTPUT時の速度指定
void pinMode(int Pin,int mode); //GPIO入出力設定
unsigned char digitalRead(int Pin); //GPIO入力
void digitalWrite(int Pin, int val); //GPIO出力
//ビット置き換え関数
uint32_t bit_replace(uint32_t data, uint32_t byte, uint8_t len, uint8_t shift);


////////////ここからCITERA編集///////////////////////////

#define SW_LEFT PA1
#define LED_GREEN PA2
#define LED_YELLOW PA3
#define LED_RED PA4
#define SW_RIGHT PA5
#define TEMP PA6
#define BUZZER PA7
#define SEG_DOT PA8
#define SEG_F PA9
#define SEG_E PA10
#define SEG_C PA13
#define CAL_S PA14
#define SEG_A PA15
#define DIG3 PD0
#define DIG4 PD1
#define UART4_TX PB0
#define UART4_RX PB1
#define SEG_D PB3
#define SEG_G PB4
#define SEG_B PB5
#define DIG1 PB6
#define DIG2 PB7

#define __DOT1 0x01
#define __DOT2 0x02
#define __DOT3 0x04
#define __DOT4 0x08
#define __CORON 0x10
#define __DASH 0x20

#define SOFT_UART_DELAY 15000 //9600bps
#define UART_TIMEOUT 5
#define MOTOR_DELAY 2
#define UART_MATRIX_DELAY 2

typedef struct{
    uint8_t red_data[8];
    uint8_t green_data[8];
}DotMatrix_t;

void Clock_init(void); //クロック設定 (144MHz)
void pinSetting(void); //ピン入出力設定
void ADC_Setting(void); //ADC設定
void UART_Setting(void); //UART設定
void ToneSetting(void); //Tone関数用タイマー設定
void TIM2_InterruptSetting(void); //TIM2 割り込み用設定

void tone(int freq);
void noTone(void);

void SerialWrite(uint8_t data);
int SerialRead(void);

void TIM2_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));
void EXTI1_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));


void SysTick_init(void); //Systick初期化（delay_ms/delay_us）
void delay (int time); //割り込みタイマによる処理
//void delay_ms(int time_ms); //SysTickによるdelay
void delay_us(int time_us);
void delay_cnt(int cnt); //直接値を代入して使う（us以上に精度が欲しい場合に使用）

//-999 ~ 9999までを受け入れ
// それより大きい値or小さい値が入った場合はクリップ処理
void SegmentFlush_int (int num);
// 桁数により表示を変更
// 9999 ~ 1000
// 999.9 ~ 100.0
// 99.99 ~ 10.00
// 9.999 ~ 0.000
// もし9.35が入力されたら9.350を出力（必ず4桁で出力）
void SegmentFlush_Float (float num);

void SegmentFlush_HEX(uint16_t num); //16bit分のデータをHEX形式で出力

// セグメントを単独で点灯?消灯させる場合に使用
// Float関数を使うと無効になるので注意
//
//        ↓Dash
// 8.8.:8.'8.
//  ↑ ↑↑ ↑  ↑
//  4 3| 2  1 Dot
//     |
//     Coron
void SegmentDot1 (char Val);
void SegmentDot2 (char Val);
void SegmentDot3 (char Val);
void SegmentDot4 (char Val);
void SegmentDash (char Val);
void SegmentCoron (char Val);
// セグメントのデータを一括で消すための関数
// これを実行するとすべて消えるので注意
void SegmentClear (void);

int analogRead(int pin);

int Serial_LSensor();
int Serial_RSensor();
void Serial_MotorDrive_L(int power);
void Serial_MotorDrive_R(int power);
void Serial_MotorDrive(int l, int r);

//DotMatrix_t構造体のデータを送信
int Serial_DotMatrix_Write(DotMatrix_t data);
//登録された構造体がすでに送信済みか確認する関数
//送信されていない場合0を返却　送信されている場合は-1を返却
int DotMatrix_Data_Compare(DotMatrix_t data);


#endif /* USER_CH32V203_MYLIB_H_ */
