#define BTN_PIN_TAREE 6
#define BEEP_PIN 10  // Пин пьезо (при выборе 9 пина, 10 - недоступен из-за шим)

#define DT_PIN_TAREE 3
#define SCK_PIN_TAREE 2

#define SOFT_UART_SPEED 1000 // Скорость Soft-UART
#define SOFT_UART_PIN 13 // Пин
#define BUS_ADRESS 0 // Адрес
#define SOFT_UART_BUFFER 20 // Буфер 

#include <GyverButton.h>
#include <HX711_ADC.h>
#include <DTM1650.h>
#include <TIMECLASS.h>
#include <softUART.h>
#include <GBUS.h>

GButton button_tare(BTN_PIN_TAREE);

HX711_ADC LoadCell(DT_PIN_TAREE, SCK_PIN_TAREE); // parameters: dt pin, sck pin
DTM1650 display;

softUART<SOFT_UART_PIN> main_soft_uart(SOFT_UART_SPEED);
GBUS main_bus(&main_soft_uart, BUS_ADRESS, SOFT_UART_BUFFER);

float i;

// the setup function runs once when you press reset or power the board
void setup()
{
	pinMode(BEEP_PIN, OUTPUT);

	display.init();
	display.set_brightness(DTM1650_BRIGHTNESS_MAX);

	LoadCell.begin(); // start connection to HX711
	LoadCell.start(1000); // load cells gets 2000ms of time to stabilize
	LoadCell.setCalFactor(-392.24); // calibration factor for load cell => strongly dependent on your individual setup

	display.send_digit(DTM1650_S_MINUS, 0);
	display.send_digit(DTM1650_S_MINUS, 1);
	display.send_digit(DTM1650_S_MINUS, 2);
	display.send_digit(DTM1650_S_MINUS, 3);

	tone(BEEP_PIN, 1200, 100);
	delay(300);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	button_tare.tick();

	LoadCell.update(); // retrieves data from the load cell
	i = LoadCell.getData(); // get output value

	display.write_num(i);

	if (i >= 1000)
	{
		i = 0;
		tone(BEEP_PIN, 1600, 200);
		display.set_dot(3, true);
	}
	else
	{
		display.set_dot(3, false);
	}

	if (button_tare.isPress())
	{
		LoadCell.start(1000);
		display.set_dot(3, true);
		display.send_digit(DTM1650_S_MINUS, 0);
		display.send_digit(DTM1650_S_MINUS, 1);
		display.send_digit(DTM1650_S_MINUS, 2);
		display.send_digit(DTM1650_S_MINUS, 3);
		tone(BEEP_PIN, 1000, 300);
		delay(300);
	}

	main_bus.tick();
}
