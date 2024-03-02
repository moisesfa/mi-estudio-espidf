#ifndef EXAMPLE_DHT_H
#define EXAMPLE_DHT_H

// Declaración Variables 
#define DHT11_PIN 17
#define BUZZER_PIN 18
#define BUZZER_PIN_SEL (1ULL << BUZZER_PIN)

#define HUM_THRESHOLD 800
#define TEMP_THRESHOLD 250

// Declaración Funciones 
void init_hw(void);
void beep(void *arg);
void check_alarm(void);
void example_dht_loop(void);


#endif // EXAMPLE_DHT_H

