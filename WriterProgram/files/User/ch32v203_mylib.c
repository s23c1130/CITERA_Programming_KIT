/*
 * ch32v203_mylib.c
 *
 *  Created on: Jun 30, 2024
 *      CITERA Programming KIT 
 */

#include "ch32v20x.h"
#include "ch32v203_mylib.h"



// 数値保持用,ドット[3][2][1][0]の順/カンマ/コロン保持用/表示するか(1)で点灯
volatile static char seg_num[5];
volatile static int seg_dot = 0, seg_val;

volatile int32_t readData = -1;
volatile int res_16bit = 0;

volatile int timeout = 0;

volatile int motor_l_before = 0;
volatile int motor_r_before = 0;

//GPIOAの有効化
void GPIOA_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
}

//GPIOBの有効化
void GPIOB_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOB;
}

//GPIOCの有効化
void GPIOC_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;
}

//GPIODの有効化
void GPIOD_init(void) {
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
}

//GPIO OUTPUT時の速度指定
//引数:speed:速度指定 mylib.h参照
//戻り値：入力されている値を返す。
//speedに0を指定すると、値が書き換わらない
int GPIO_output_speed(int speed) {
    static int mode_speed = OUTPUT_2MHz;
    if (speed) {
        mode_speed = speed;
    }
    return mode_speed;
}

//GPIO入出力設定
//pin:ピン番号(mylib.h参照)
//mode:モード(OUTPUTなど mylib.h参照)
void pinMode(int Pin, int mode) {
    //GPIOがA(0x1y)だったら
    if ((Pin & 0xF0) == 0x10) {
        //OUTPUTだったら
        if ((mode & 0b1000) == 0) {
            //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOA->CFGHR = bit_replace(GPIOA->CFGHR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOA->CFGHR = bit_replace(GPIOA->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            }
        } else {                //INPUT
                                //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOA->CFGLR = bit_replace(GPIOA->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOA->CFGHR = bit_replace(GPIOA->CFGHR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOA->CFGHR = bit_replace(GPIOA->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);

            }
            //プルアップもしくはプルダウンを選択している場合
            if ((mode & 0b0011) == 0b10) {
                GPIOA->OUTDR = bit_replace(GPIOA->OUTDR, ((mode & 0b0100) >> 2),
                        1, (Pin & 0x0F));
            }
        }
    }
    //GPIOがB(0x2y)だったら
    if ((Pin & 0xF0) == 0x20) {
        //OUTPUTだったら
        if ((mode & 0b1000) == 0) {
            //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOB->CFGLR = bit_replace(GPIOB->CFGLR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOB->CFGLR = bit_replace(GPIOB->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOB->CFGHR = bit_replace(GPIOB->CFGHR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOB->CFGHR = bit_replace(GPIOB->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            }
        } else {                //INPUT
                                //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOB->CFGLR = bit_replace(GPIOB->CFGLR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOB->CFGLR = bit_replace(GPIOB->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOB->CFGHR = bit_replace(GPIOB->CFGHR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOB->CFGHR = bit_replace(GPIOB->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            }
            //プルアップもしくはプルダウンを選択している場合
            if ((mode & 0b0011) == 0b10) {
                GPIOB->OUTDR = bit_replace(GPIOB->OUTDR, ((mode & 0b0100) >> 2),
                        1, (Pin & 0x0F));
            }
        }
    }
    //GPIOがC(0x3y)だったら
    if ((Pin & 0xF0) == 0x30) {
        //OUTPUTだったら
        if ((mode & 0b1000) == 0) {
            //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOC->CFGHR = bit_replace(GPIOC->CFGHR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOC->CFGHR = bit_replace(GPIOC->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            }
        } else {                //INPUT
                                //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOC->CFGLR = bit_replace(GPIOC->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOC->CFGHR = bit_replace(GPIOC->CFGHR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOC->CFGHR = bit_replace(GPIOC->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            }
            //プルアップもしくはプルダウンを選択している場合
            if ((mode & 0b0011) == 0b10) {
                GPIOC->OUTDR = bit_replace(GPIOC->OUTDR, ((mode & 0b0100) >> 2),
                        1, (Pin & 0x0F));
            }
        }
    }
    //GPIOがD(0x4y)だったら
    if ((Pin & 0xF0) == 0x40) {
        //OUTPUTだったら
        if ((mode & 0b1000) == 0) {
            //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOD->CFGHR = bit_replace(GPIOD->CFGHR, GPIO_output_speed(0),
                        2, ((Pin & 0b0111) * 4));
                //CNF
                GPIOD->CFGHR = bit_replace(GPIOD->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            }
        } else {                //INPUT
                                //レジスタがHigh側かLow側か
            if ((Pin & 0b1000) == 0) {
                //LOW側
                //MODE
                GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOD->CFGLR = bit_replace(GPIOD->CFGLR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            } else {
                //HIGH側
                //MODE
                GPIOD->CFGHR = bit_replace(GPIOD->CFGHR, 0b00, 2,
                        ((Pin & 0b0111) * 4));
                //CNF
                GPIOD->CFGHR = bit_replace(GPIOD->CFGHR, (mode & 0b0011), 2,
                        (Pin & 0b0111) * 4 + 2);
            }
            //プルアップもしくはプルダウンを選択している場合
            if ((mode & 0b0011) == 0b10) {
                GPIOD->OUTDR = bit_replace(GPIOD->OUTDR, ((mode & 0b0100) >> 2),
                        1, (Pin & 0x0F));
            }
        }
    }
}

// GPIOを取得する関数
// 戻り値：ピンがHIGH(1)かLOW(0)か
// 0xFFが返却されたらエラー
unsigned char digitalRead(int Pin) {
    // GPIOxがA(0x1y)だったら
    if ((Pin & 0xF0) == 0x10) {
        if (GPIOA->INDR & (1 << (Pin & 0x0F))) {
            return 1;
        } else {
            return 0;
        }
    }
    // GPIOxがB(0x2y)だったら
    if ((Pin & 0xF0) == 0x20) {
        if (GPIOB->INDR & (1 << (Pin & 0x0F))) {
            return 1;
        } else {
            return 0;
        }
    }
    // GPIOxがC(0x3y)だったら
    if ((Pin & 0xF0) == 0x30) {
        if (GPIOC->INDR & (1 << (Pin & 0x0F))) {
            return 1;
        } else {
            return 0;
        }
    }
    // GPIOxがD(0x4y)だったら
    if ((Pin & 0xF0) == 0x40) {
        if (GPIOD->INDR & (1 << (Pin & 0x0F))) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0xFF; // エラー
}

// デジタル値出力
// Pin:ピン 名付け 0xYZ Y:GPIOx(A:1 B:2 C:3 ...)
//     Z:ポート番号 GPIOA-10 →　0x1Aとなる。
// val:出力値 (1or0)
void digitalWrite(int Pin, int val) {
    // GPIOxがA(0x1y)だったら
    if ((Pin & 0xF0) == 0x10) {
        if (val == 0) {
            GPIOA->OUTDR &= ~(1 << (Pin & 0x0F));
        } else {
            GPIOA->OUTDR |= (1 << (Pin & 0x0F));
        }
    }

    // GPIOxがB(0x2y)だったら
    if ((Pin & 0xF0) == 0x20) {
        if (val == 0) {
            GPIOB->OUTDR &= ~(1 << (Pin & 0x0F));
        } else {
            GPIOB->OUTDR |= (1 << (Pin & 0x0F));
        }
    }
    // GPIOxがC(0x3y)だったら
    if ((Pin & 0xF0) == 0x30) {
        if (val == 0) {
            GPIOC->OUTDR &= ~(1 << (Pin & 0x0F));
        } else {
            GPIOC->OUTDR |= (1 << (Pin & 0x0F));
        }
    }

    // GPIOxがD(0x4y)だったら
    if ((Pin & 0xF0) == 0x40) {
        if (val == 0) {
            GPIOD->OUTDR &= ~(1 << (Pin & 0x0F));
        } else {
            GPIOD->OUTDR |= (1 << (Pin & 0x0F));
        }
    }
}

//  ビット置き換え関数
//  引数 data:置き換え前のビット列 | byte:置き換えるビット |
//  len:置き換えるビットの長さ | shift:シフト数
uint32_t bit_replace(uint32_t data, uint32_t byte, uint8_t len, uint8_t shift) {
    uint32_t mask = ~(((1 << len) - 1) << shift);
    data &= mask;
    data |= byte << shift;
    return data;
}

/////////////////////////////////////////////////
//////////////ここからCITERA変更//////////////////
/////////////////////////////////////////////////


//HSI(8MHz)を使用
//PLL x18により
//SYSCLK　144MHz
//HCLK 144MHz
//APB1 144MHz
//APB2 72MHz
//ADC 12MHz
//(状況により変更あり)
void Clock_init(void){
    //PLL x18に設定
    RCC->CFGR0 |= (1 << 21);
    RCC->CFGR0 |= (1 << 20);
    RCC->CFGR0 |= (1 << 19);
    RCC->CFGR0 |= (1 << 18);

    //APB2クロック 2分周に
    RCC->CFGR0 &= ~RCC_PPRE2_0;
    RCC->CFGR0 &= ~RCC_PPRE2_1;
    RCC->CFGR0 |= RCC_PPRE2_2;

    //ADCクロック　6分周に　APB2クロック/6 = 12MHz
    RCC->CFGR0 &= ~RCC_ADCPRE_0;
    RCC->CFGR0 |= RCC_ADCPRE_1;

    //PLL HSI->そのまま入力 (/2しない)
    EXTEN->EXTEN_CTR |= (1 << 4);

    //PLL有効　(input clock x 18)
    RCC->CTLR |= (1 << 24);

    //PLLがReadyになるまで待機
    while((RCC->CTLR & (1 << 25)) == 0);

    //システムクロックをPLL入力にセット
    RCC->CFGR0 |= (1 << 1);
}

void pinSetting(void){

    GPIOA_init();
    GPIOB_init();
    GPIOD_init();

    RCC->APB2PCENR |= RCC_AFIOEN;          // AFIO有効

    AFIO->PCFR1 |= AFIO_PCFR1_PD01_REMAP;  // PD0/PD1をGPIOとして使う

    // SWDを無効にしてGPIOを有効にする
    AFIO->PCFR1 &= ~AFIO_PCFR1_SWJ_CFG_0;
    AFIO->PCFR1 &= ~AFIO_PCFR1_SWJ_CFG_1;
    AFIO->PCFR1 |= AFIO_PCFR1_SWJ_CFG_2;

    //AFIO->PCFR2 &= ~(1 << 17);
    //AFIO->PCFR2 |= (1 << 16); //PB0 TX PB1 RXに割当

    GPIO_output_speed (OUTPUT_50MHz);

    pinMode(SW_LEFT, INPUT_PULLUP);
    pinMode(SW_RIGHT, INPUT_PULLUP);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(TEMP, INPUT_ANALOG);
    pinMode(BUZZER, OUTPUT_FUNC);
    pinMode(SEG_A, OUTPUT_OPENDRAIN);
    pinMode(SEG_B, OUTPUT_OPENDRAIN);
    pinMode(SEG_C, OUTPUT_OPENDRAIN);
    pinMode(SEG_D, OUTPUT_OPENDRAIN);
    pinMode(SEG_E, OUTPUT_OPENDRAIN);
    pinMode(SEG_F, OUTPUT_OPENDRAIN);
    pinMode(SEG_G, OUTPUT_OPENDRAIN);
    pinMode(SEG_DOT, OUTPUT_OPENDRAIN);
    pinMode(DIG1, OUTPUT_OPENDRAIN);
    pinMode(DIG2, OUTPUT_OPENDRAIN);
    pinMode(DIG3, OUTPUT_OPENDRAIN);
    pinMode(DIG4, OUTPUT_OPENDRAIN);
    pinMode(CAL_S, OUTPUT);
    pinMode(UART4_TX, OUTPUT);
    pinMode(UART4_RX, INPUT_PULLDOWN);
}

//ADC設定
void ADC_Setting(void){
    RCC->APB2PCENR |= RCC_ADC1EN; //ADC1有効
    

    ADC1->CTLR2 &= ~ADC_ALIGN; //右詰め
}

//UART設定
void UART_Setting(void){
    //なぜかUART4は使えないので、ソフトウェアUARTに変更

    // RCC->APB1PCENR |= RCC_APB1Periph_UART4; //UART4有効

    // UART4->BRR = (1 << 4) | 0b1000; //9600bps(@144MHz)

    // UART4->CTLR1 = USART_CTLR1_TE; //送信有効
    // UART4->CTLR1 |= USART_CTLR1_RE; //受信有効

    // UART4->CTLR1 |= USART_CTLR1_UE; //UART4有効

    digitalWrite(UART4_TX, 1); //1状態で待機

    //割り込み設定
    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB; //PB1を割り込み対象にする
    EXTI->INTENR |= EXTI_INTENR_MR1; //PB1の割り込み有効
    EXTI->FTENR |= EXTI_FTENR_TR1; //たち下がりで割り込みさせる
    NVIC_SetPriority(EXTI1_IRQn, 0b11100000); //優先度をTIM2の次に
    NVIC_EnableIRQ(EXTI1_IRQn);
}

//Tone関数用タイマー設定
void ToneSetting(void){
    //TIM3_CH2 PWM設定
    RCC->APB1PCENR |= RCC_APB1Periph_TIM3;

    TIM3->CTLR1 |= TIM_ARPE; //Auto-reload and Preload 有効

    TIM3->CTLR1 |= TIM_CEN; //Count開始

    TIM3->PSC = 143; //1MHz (@144MHz)

    //TIM3 CH2 PWM1モード
    TIM3->CHCTLR1 &= ~TIM_OC2M_0;
    TIM3->CHCTLR1 |= TIM_OC2M_1;
    TIM3->CHCTLR1 |= TIM_OC2M_2;

    TIM3->CHCTLR1 |= TIM_OC2PE; //OC2 preload 有効
}

//TIM2 割り込み用設定
void TIM2_InterruptSetting(void){
    //TIM2有効化
    RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
    TIM2->CTLR1 |= TIM_ARPE; //Auto-reload and Preload 有効

    TIM2->PSC = 143; //(@144MHz)
    TIM2->ATRLR = 1000; //1MHzを1kHzで割り込み

    TIM2->DMAINTENR |= TIM_UIE; //Update Interrupt 有効

    NVIC_SetPriority(TIM2_IRQn, 0b00000000); //再優先度に設定
    NVIC_EnableIRQ(TIM2_IRQn); //interrupt TIM2_IRQn 有効

    TIM2->CTLR1 |= TIM_CEN; //Count開始
}

void tone(int freq){
    //TIM3　周波数指定
    TIM3->ATRLR = 1000000 / freq; //TIM3 周波数指定
    TIM3->CH2CVR = 1000000 / freq / 2; //TIM3_CH2 Duty比を50%に

    TIM3->CCER |= TIM_CC2E; //TIM3_CH2 出力有効
}

void noTone(void){
    //TIM3_CH2 出力無効
    TIM3->CCER &= ~TIM_CC2E;
}

void SerialWrite(uint8_t data){
    // UART4->DATAR = data; //データを送信

    // //TCフラグを下ろして、送信完了するフラグが立つまで待機
    // UART4->STATR &= ~USART_FLAG_TC;
    // while((UART4->STATR &= ~USART_FLAG_TC) == 0);

    digitalWrite(UART4_TX, 0); //スタートビット
    delay_cnt(SOFT_UART_DELAY); //9600bps

    for(int i = 0; i < 8; i++){
        if(data & (1 << i)){
            digitalWrite(UART4_TX, 1);
        }else{
            digitalWrite(UART4_TX, 0);
        }
        delay_cnt(SOFT_UART_DELAY);
    }

    digitalWrite(UART4_TX, 1);
    delay_cnt(SOFT_UART_DELAY*2); //ストップビット
}

int SerialRead(void){
    int tmp = readData;
    readData = -1; //-1を代入（次読み取ったとき-1なら新規データなしと分かるため）
    return tmp;
}

//シリアル受信
void EXTI1_IRQHandler (void){
    NVIC_DisableIRQ(EXTI1_IRQn);

    //8bit受信モード
    if(res_16bit == 0){

        uint8_t res_data = 0;
        delay_cnt(SOFT_UART_DELAY); //スタートビット分待つ
        for(int i = 0 ; i < 8 ; i++){
            uint32_t high_cnt = 0;
            uint32_t low_cnt = 0;
            SysTick->CNT = 0;
            while(SysTick->CNT < SOFT_UART_DELAY){
                if(digitalRead(UART4_RX) == 0){
                    low_cnt++;
                }else{
                    high_cnt++;
                }
            }

            if(high_cnt > low_cnt){
                res_data |= (1 << i);
            }
        }

        delay_cnt(SOFT_UART_DELAY); //ストップビットまで待って終了

        readData = res_data;
    }else{
        //16bit受信モード（連続受信する場合に使用）
        uint16_t res_data = 0;
        delay_cnt(SOFT_UART_DELAY); //スタートビット分待つ
        for(int i = 8 ; i < 16 ; i++){
            uint32_t high_cnt = 0;
            uint32_t low_cnt = 0;
            SysTick->CNT = 0;
            while(SysTick->CNT < SOFT_UART_DELAY){
                if(digitalRead(UART4_RX) == 0){
                    low_cnt++;
                }else{
                    high_cnt++;
                }
            }

            if(high_cnt > low_cnt){
                res_data |= (1 << i);
            }
        }
        delay_cnt(SOFT_UART_DELAY); //ストップビット
        delay_cnt(SOFT_UART_DELAY); //スタートビット
        
        for(int i = 0 ; i < 8 ; i++){
            uint32_t high_cnt = 0;
            uint32_t low_cnt = 0;
            SysTick->CNT = 0;
            while(SysTick->CNT < SOFT_UART_DELAY){
                if(digitalRead(UART4_RX) == 0){
                    low_cnt++;
                }else{
                    high_cnt++;
                }
            }

            if(high_cnt > low_cnt){
                res_data |= (1 << i);
            }
        }

        readData = res_data;
    }

    delay_cnt(SOFT_UART_DELAY); //ストップビット

    EXTI->INTFR |= EXTI_INTF_INTF1; //クリア
    NVIC_EnableIRQ(EXTI1_IRQn);
}

void TIM2_IRQHandler (void) {
    TIM2->INTFR &= ~TIM_UIF;  // interrupt解除

    timeout++; //シリアル通信タイムアウト用

    volatile static int seg_digit = 0;
    seg_digit++;
    if (seg_digit >= 5) {
        seg_digit = 0;
    }

    // セグメント制御部分
    if (seg_val == 1) {

        // 切り替える瞬間に見えることを防止
        digitalWrite (DIG1, 1);
        digitalWrite (DIG2, 1);
        digitalWrite (DIG3, 1);
        digitalWrite (DIG4, 1);
        digitalWrite (CAL_S, 0);

        if (seg_digit == 4) {
            if (seg_dot & __CORON) {
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
            } else {
                digitalWrite (SEG_A, 1);
                digitalWrite (SEG_B, 1);
            }

            if (seg_dot & __DASH) {
                digitalWrite (SEG_C, 0);
            } else {
                digitalWrite (SEG_C, 1);
            }
        } else {
            switch (seg_num[seg_digit]) {
            case 0:
            case '0':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 1);
                break;
            case 1:
            case '1':
                digitalWrite (SEG_A, 1);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 1);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 1);
                digitalWrite (SEG_G, 1);
                break;
            case 2:
            case '2':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 1);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 1);
                digitalWrite (SEG_G, 0);
                break;
            case 3:
            case '3':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 1);
                digitalWrite (SEG_G, 0);
                break;
            case 4:
            case '4':
                digitalWrite (SEG_A, 1);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 1);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 5:
            case '5':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 6:
            case '6':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 7:
            case '7':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 1);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 1);
                break;
            case 8:
            case '8':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 9:
            case '9':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 10:
            case 'A':
            case 'a':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 1);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 11:
            case 'B':
            case 'b':
                digitalWrite (SEG_A, 1);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 12:
            case 'C':
            case 'c':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 1);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 1);
                break;
            case 13:
            case 'D':
            case 'd':
                digitalWrite (SEG_A, 1);
                digitalWrite (SEG_B, 0);
                digitalWrite (SEG_C, 0);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 1);
                digitalWrite (SEG_G, 0);
                break;
            case 14:
            case 'E':
            case 'e':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 1);
                digitalWrite (SEG_D, 0);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;
            case 15:
            case 'F':
            case 'f':
                digitalWrite (SEG_A, 0);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 1);
                digitalWrite (SEG_D, 1);
                digitalWrite (SEG_E, 0);
                digitalWrite (SEG_F, 0);
                digitalWrite (SEG_G, 0);
                break;     
            case '-':
                digitalWrite (SEG_A, 1);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 1);
                digitalWrite (SEG_D, 1);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 1);
                digitalWrite (SEG_G, 0);
                break;
            default:
                digitalWrite (SEG_A, 1);
                digitalWrite (SEG_B, 1);
                digitalWrite (SEG_C, 1);
                digitalWrite (SEG_D, 1);
                digitalWrite (SEG_E, 1);
                digitalWrite (SEG_F, 1);
                digitalWrite (SEG_G, 1);
                break;
            }
        }
        switch (seg_digit) {
        case 0:
            if (seg_dot & __DOT1) {
                digitalWrite (SEG_DOT, 0);
            } else {
                digitalWrite (SEG_DOT, 1);
            }

            digitalWrite (DIG1, 1);
            digitalWrite (DIG2, 1);
            digitalWrite (DIG3, 1);
            digitalWrite (DIG4, 0);
            digitalWrite (CAL_S, 0);
            break;
        case 1:
            if (seg_dot & __DOT2) {
                digitalWrite (SEG_DOT, 0);
            } else {
                digitalWrite (SEG_DOT, 1);
            }
            digitalWrite (DIG1, 1);
            digitalWrite (DIG2, 1);
            digitalWrite (DIG3, 0);
            digitalWrite (DIG4, 1);
            digitalWrite (CAL_S, 0);
            break;
        case 2:
            if (seg_dot & __DOT3) {
                digitalWrite (SEG_DOT, 0);
            } else {
                digitalWrite (SEG_DOT, 1);
            }
            digitalWrite (DIG1, 1);
            digitalWrite (DIG2, 0);
            digitalWrite (DIG3, 1);
            digitalWrite (DIG4, 1);
            digitalWrite (CAL_S, 0);
            break;
        case 3:
            if (seg_dot & __DOT4) {
                digitalWrite (SEG_DOT, 0);
            } else {
                digitalWrite (SEG_DOT, 1);
            }
            digitalWrite (DIG1, 0);
            digitalWrite (DIG2, 1);
            digitalWrite (DIG3, 1);
            digitalWrite (DIG4, 1);
            digitalWrite (CAL_S, 0);
            break;
        case 4:
            digitalWrite (DIG1, 1);
            digitalWrite (DIG2, 1);
            digitalWrite (DIG3, 1);
            digitalWrite (DIG4, 1);
            digitalWrite (CAL_S, 1);
            break;
        }
    } else {
        digitalWrite (SEG_A, 1);
        digitalWrite (SEG_B, 1);
        digitalWrite (SEG_C, 1);
        digitalWrite (SEG_D, 1);
        digitalWrite (SEG_E, 1);
        digitalWrite (SEG_F, 1);
        digitalWrite (SEG_G, 1);
        digitalWrite (SEG_DOT, 1);
        digitalWrite (DIG1, 1);
        digitalWrite (DIG2, 1);
        digitalWrite (DIG3, 1);
        digitalWrite (DIG4, 1);
        digitalWrite (CAL_S, 0);
    }
}

// void delay (int time) {
//     delay_cnt = 0;
//     while (delay_cnt < time);
// }

//Systick初期化（delay_ms/delay_us）
void SysTick_init(void){
    //STRE オートリロード有効 (最大値になると0にもどる）
    SysTick->CTLR |= (1 << 3);

    //HCLKをそのまま入力として使う
    //HCLK/8 0にするとHCLK/8になる
    SysTick->CTLR |= (1 << 2);

    //64bit分入れておく
    SysTick->CMP = 0xFFFFFFFFFF;

    //SysTickクロック開始
    SysTick->CTLR |= (1 << 0);
}

//SysTickによるdelay
void delay(int time){
    SysTick->CNT = 0; //0にする
    volatile uint64_t delay_cnt = (uint64_t)time * 144000;
    while(SysTick->CNT < delay_cnt); //待機
}

void delay_us(int time_us){
    SysTick->CNT = 0; //0にする
    volatile uint64_t delay_cnt = (uint64_t)time_us * 144;
    while(SysTick->CNT < delay_cnt); //待機
}

void delay_cnt(int cnt){
    SysTick->CNT = 0; //0にする
    while(SysTick->CNT < cnt); //待機
}

//-999 ~ 9999までを受け入れ
// それより大きい値or小さい値が入った場合はクリップ処理
void SegmentFlush_int (int num) {
    seg_val = 0;  // 一時的に無効化

    // クリップ処理
    if (num >= 10000) {
        num = 9999;
    }
    if (num <= -1000) {
        num = -999;
    }
    // 負の値だった場合
    if (num < 0) {
        num *= -1;
        seg_num[3] = '-';
        seg_num[2] = (num % 1000) / 100;
        seg_num[1] = (num % 100) / 10;
        seg_num[0] = (num % 10) / 1;
    } else {
        seg_num[3] = num / 1000;
        seg_num[2] = (num % 1000) / 100;
        seg_num[1] = (num % 100) / 10;
        seg_num[0] = (num % 10) / 1;
    }

    seg_val = 1;  // 有効化
}

// 桁数により表示を変更
// 9999 ~ 1000
// 999.9 ~ 100.0
// 99.99 ~ 10.00
// 9.999 ~ 0.000
// もし9.35が入力されたら9.350を出力（必ず4桁で出力）
void SegmentFlush_Float (float num) {
    if (num >= 9999) {
        num = 9999;
    }
    if (num < -999) {
        num = -999;
    }
    seg_val = 0;
    seg_dot = 0;

    //正の値
    if (num > 0) {
        // 1000 ~ 9999
        if (num > 1000) {
            int int_num = (int)num;
            seg_num[3] = int_num / 1000;
            seg_num[2] = (int_num % 1000) / 100;
            seg_num[1] = (int_num % 100) / 10;
            seg_num[0] = (int_num % 10) / 1;
        }

        // 100.0 ~ 999.9
        if (100.0 <= num && num <= 999.9) {
            int int_num = (int)(num * 10);
            seg_num[3] = int_num / 1000;
            seg_num[2] = (int_num % 1000) / 100;
            seg_num[1] = (int_num % 100) / 10;
            seg_num[0] = (int_num % 10) / 1;
            seg_dot = __DOT2;
        }

        // 10.00 ~ 99.99
        if (10.00 <= num && num <= 99.99) {
            int int_num = (int)(num * 100);
            seg_num[3] = int_num / 1000;
            seg_num[2] = (int_num % 1000) / 100;
            seg_num[1] = (int_num % 100) / 10;
            seg_num[0] = (int_num % 10) / 1;
            seg_dot = __DOT3;
        }

        // 0.000 ~ 9.999
        if (num <= 9.999) {
            int int_num = (int)(num * 1000);
            seg_num[3] = int_num / 1000;
            seg_num[2] = (int_num % 1000) / 100;
            seg_num[1] = (int_num % 100) / 10;
            seg_num[0] = (int_num % 10) / 1;
            seg_dot = __DOT4;
        }
    }else{
        //-999 ~ -100
        if(-999 <= num && num <= -100){
            int int_num = (int)(num * -1);
            seg_num[3] = '-';
            seg_num[2] = (int_num % 1000) / 100;
            seg_num[1] = (int_num % 100) / 10;
            seg_num[0] = (int_num % 10) / 1;
        }

        //-99.9 ~ -10.0
        if(-99.9 <= num && num <= -10.0){
            int int_num = (int)(num * -10);
            seg_num[3] = '-';
            seg_num[2] = (int_num % 1000) / 100;
            seg_num[1] = (int_num % 100) / 10;
            seg_num[0] = (int_num % 10) / 1;
            seg_dot = __DOT2;
        }
        //-9.99 ~ -0.00
        if(-9.99 <= num && num < 0){
            int int_num = (int)(num * -100);
            seg_num[3] = '-';
            seg_num[2] = (int_num % 1000) / 100;
            seg_num[1] = (int_num % 100) / 10;
            seg_num[0] = (int_num % 10) / 1;
            seg_dot = __DOT3;
        }
    }

    seg_val = 1;
}

//16bit分のデータをHEX形式で出力
void SegmentFlush_HEX(uint16_t num){
    seg_val = 0;

    if(num >= 0xFFFF){
        num = 0xFFFF;
    }

    seg_num[3] = (num & 0xF000) >> 12;
    seg_num[2] = (num & 0x0F00) >> 8;
    seg_num[1] = (num & 0x00F0) >> 4;
    seg_num[0] = (num & 0x000F);

    seg_val = 1;
}

// セグメントを単独で点灯?消灯させる場合に使用
// Float関数を使うと無効になるので注意
//
//        ↓Dash
// 8.8.:8.'8.
//  ↑ ↑↑ ↑  ↑
//  4 3| 2  1 Dot
//     |
//     Coron
void SegmentDot1 (char Val) {
    if (Val) {
        seg_dot |= __DOT1;
    } else {
        seg_dot &= ~__DOT1;
    }
}

void SegmentDot2 (char Val) {
    if (Val) {
        seg_dot |= __DOT2;
    } else {
        seg_dot &= ~__DOT2;
    }
}

void SegmentDot3 (char Val) {
    if (Val) {
        seg_dot |= __DOT3;
    } else {
        seg_dot &= ~__DOT3;
    }
}

void SegmentDot4 (char Val) {
    if (Val) {
        seg_dot |= __DOT4;
    } else {
        seg_dot &= ~__DOT4;
    }
}

void SegmentDash (char Val) {
    if (Val) {
        seg_dot |= __DASH;
    } else {
        seg_dot &= ~__DASH;
    }
}

void SegmentCoron (char Val) {
    if (Val) {
        seg_dot |= __CORON;
    } else {
        seg_dot &= ~__CORON;
    }
}

// セグメントのデータを一括で消すための関数
// これを実行するとすべて消えるので注意
void SegmentClear (void) {
    seg_val = 0;
    seg_dot = 0x00;
    seg_num[0] = 0;
    seg_num[1] = 0;
    seg_num[2] = 0;
    seg_num[3] = 0;
}

int analogRead(int pin){
    //キャリブレーション
    ADC1->CTLR2 |= ADC_ADON; //ADC起動

    //ADCが安定状態になるまで(1us)待機
    delay_us(100);

    ADC1->CTLR2 |= ADC_CAL; //キャリブレーション開始
    while(ADC1->CTLR2 & ADC_CAL); //終了するまで待機
    
    ADC1->RSQR1 = 0; //L = 0
    ADC1->SAMPTR2 = 0b101; //55.5cycles;
    switch(pin)
    {
        case PA0:
            ADC1->RSQR3 = 0;
            break;
        case PA1:
            ADC1->RSQR3 = 1;
            break;
        case PA2:
            ADC1->RSQR3 = 2;
            break;
        case PA3:
            ADC1->RSQR3 = 3;
            break;
        case PA4:
            ADC1->RSQR3 = 4;
            break;
        case PA5:
            ADC1->RSQR3 = 5;
            break;
        case PA6:
            ADC1->RSQR3 = 6;
            break;
        case PA7:
            ADC1->RSQR3 = 7;
            break;
        case PB0:
            ADC1->RSQR3 = 8;
            break;
        case PB1:
            ADC1->RSQR3 = 9;
            break;
        default:
            return -1;
            break;
    }

    delay_us(500);

    ADC1->CTLR2 |= ADC_ADON; //変換開始
    ADC1->STATR &= ~ADC_EOC; //フラグを降ろす
    while((ADC1->STATR & ADC_EOC) == 0); //変換が終わるまで待機

    
    return (ADC1->RDATAR & 0xFFFF);
}

int Serial_LSensor(){
    res_16bit = 1;
    SerialWrite(0x91);
    timeout = 0;
    int data;
    while(timeout < UART_TIMEOUT){
        data = SerialRead();
        if(data != -1){
            break;
        }
    }

    if(((data & 0xFF00) >> 8) != 0x11){
        //エラー
        return -1;
    }

    return (0xFF & data);
}

int Serial_RSensor(){
    res_16bit = 1;
    SerialWrite(0x92);
    timeout = 0;
    int data;
    while(timeout < UART_TIMEOUT){
        data = SerialRead();
        if(data != -1){
            break;
        }
    }

    if(((data & 0xFF00) >> 8) != 0x12){
        //エラー
        return -1;
    }

    return (0xFF & data);
}

void Serial_MotorDrive_L(int power){
    res_16bit = 0;
    if(power >= 0){
        if(power >= 255) power = 255;
        SerialWrite(0x94);
        timeout = 0;
        int data;
        while(timeout < UART_TIMEOUT){
            data = SerialRead();
            if(data != -1){
                break;
            }
        }

        if(data != 0x14){
            return; //エラー
        }

        SerialWrite(power);
    }else{
        power *= -1;
        if(power >= 255) power = 255;
        SerialWrite(0xB4);
        timeout = 0;
        int data;
        while(timeout < UART_TIMEOUT){
            data = SerialRead();
            if(data != -1){
                break;
            }
        }

        if(data != 0x16){
            return; //エラー
        }

        SerialWrite(power);
    }
}

void Serial_MotorDrive_R(int power){
    res_16bit = 0;
    if(power >= 0){
        if(power >= 255) power = 255;
        SerialWrite(0x98);
        timeout = 0;
        int data;
        while(timeout < UART_TIMEOUT){
            data = SerialRead();
            if(data != -1){
                break;
            }
        }

        if(data != 0x18){
            return; //エラー
        }

        SerialWrite(power);
    }else{
        power *= -1;
        if(power >= 255) power = 255;
        SerialWrite(0xB8);
        timeout = 0;
        int data;
        while(timeout < UART_TIMEOUT){
            data = SerialRead();
            if(data != -1){
                break;
            }
        }

        if(data != 0x1A){
            return; //エラー
        }

        SerialWrite(power);
    }
}

void Serial_MotorDrive(int l, int r){
    if(motor_l_before != l){
        motor_l_before = l;
        Serial_MotorDrive_L(l);
        delay(MOTOR_DELAY);
    }

    if(motor_r_before != r){
        motor_r_before = r;
        Serial_MotorDrive_R(r);
        delay(MOTOR_DELAY);
    }
}

int Serial_DotMatrix_Write(DotMatrix_t data){

    //以前送信したデータと同じ→なにもせず終了
    if(DotMatrix_Data_Compare(data) != 0){
        return 2;
    }

    SerialWrite(0xC1);
    delay(UART_MATRIX_DELAY);
    int buf;
    timeout = 0;
    while(timeout < UART_TIMEOUT){
        buf = SerialRead();
        if (buf != -1){
            break;
        }
    }
    if(buf == 0x01){
        for(int i = 0; i < 8; i++){
            SerialWrite(data.red_data[i]);
            delay(UART_MATRIX_DELAY);
            SerialWrite(data.green_data[i]);
            delay(UART_MATRIX_DELAY);
        }
    }else{
        //エラー
        return -1;
    }

    return 0;
}

int DotMatrix_Data_Compare(DotMatrix_t data){
    static DotMatrix_t save_dot;
    int result = -1;
    for(int i = 0; i < 8; i++){
        if(save_dot.red_data[i] != data.red_data[i]){
            result = 0;
            save_dot.red_data[i] = data.red_data[i];
        }
        if(save_dot.green_data[i] != data.green_data[i]){
            result = 0;
            save_dot.green_data[i] = data.green_data[i];
        }
    }
    return result;
}