
// attiny85(digispark) , доработка распылителя Glade
// логика работы - если свет не включается(не превышен порог light_change), то впрыск происходит раз в 5 часов
// при включении света(превышен порог light_change) происходит впрыск, если включить свет повторно в течении 30 минут, то впрыска не будет, а будет подан звуковой сигнал
// если свет будет продолжать гореть, то по ситечении 30 минут произойдет впрыс, а затем снова через 30 минут и т.д.
// если свет больше не включается, то следующий впрыс будет через 5 часов после последнего впрыска

#define buzzerPin 0
#define buttonPin 1
#define sensorPin A2 // GPIO 4

#include <avr/wdt.h>
#include <avr/sleep.h>

int count = 122;
int val = 0;
int buzzer_ = 0;
int light_change = 450;

void setup() {
    // Serial.begin(9600);
    pinMode(buttonPin, OUTPUT);        // управление кнопкой, активный - низкий
    digitalWrite(buttonPin, HIGH);
    pinMode(buzzerPin, OUTPUT);        // управление пищалкой, активный - высокий
    digitalWrite(buzzerPin, LOW);
    ADCSRA = 0;                        // отключаем АЦП
    ACSR |= (1 << ACD);                // отключаем компаратор
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void loop() {
    ADCSRA |= _BV(ADEN);      // включаем АЦП
    wdt_enable(WDTO_8S);
    WDTCR |= (1 << WDIE);     // прерывание по вотчдогу , в attiny85 регистр называется не WDTCSR , а WDTCR !!!
    if (count >= 1875) {      // реально вотчдог срабатывает примерно через 9.4 секунды, поэтому 1500 это около 4 часов, 1875 - около 5 часов
        glade_on();
    }                 
    val = analogRead(sensorPin);
    ADCSRA = 0; // отключаем АЦП
    if (val <= light_change) {
        buzzer_ = 1;          // разрешение включения пищалки
    } 
    if (val > light_change && count > 200) {      // впрыск при включении света, не раньше чем через 30(200 count) минут
        glade_on();
    } else if (val > light_change && buzzer_ == 1 && count <= 200) {      //после последнего впрыска и сигнализирует пищалкой если раньше 200(примерно 30 минут)
        buzzer();
    }
    /*  Serial.print("val = "); 
      Serial.println(val);
      Serial.print("count = "); 
      Serial.println(count);
      Serial.print("buzzer_ = ");
      Serial.println(buzzer_);
      Serial.println(""); */
    wdt_reset();
    sleep_enable();
    sleep_cpu();
}

ISR(WDT_vect) {
    count++;
}

void glade_on() {
    digitalWrite(buttonPin, LOW);
    delay(50);
    digitalWrite(buttonPin, HIGH);
    buzzer_ = 0; // запрет включения пищалки
    count = 0;
}

void buzzer() {
    for (byte n = 0; n <= 3; n++) {
        digitalWrite(buzzerPin, HIGH);
        delay(50);
        digitalWrite(buzzerPin, LOW);
        delay(50);
    }
    buzzer_ = 0;
}
