
int main (void) {
    Clock_init();

    SysTick_init();

    pinSetting();
    TIM2_InterruptSetting();

    ADC_Setting();

    UART_Setting();

    ToneSetting();

    delay(100);

    setup();

    while (1) {
        loop();
    }
}

////////////自動追加2 ここから//////////////////
////////////関数等/////////////////////////////



////////////自動追加2 ここまで/////////////////
