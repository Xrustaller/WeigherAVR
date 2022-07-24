#define BTN_PIN_TAREE 4
#define BEEP_PIN 9  // Пин пьезо (при выборе 9 пина, 10 - недоступен из-за шим)

#define DT_PIN_TAREE 3
#define SCK_PIN_TAREE 2

#define DISP_UPDATE_FRAN 200 // Частота обновления дисплея (мс)

#define CALIBRATION 2005.0 // 848.0

#define BUS_ID 7
#define PIN_REDE A1

#include <GyverButton.h>
#include <HX711_ADC.h>
#include <DTM1650.h>
#include <ModbusRtu.h>

GButton button_tare(BTN_PIN_TAREE);

HX711_ADC LoadCell(DT_PIN_TAREE, SCK_PIN_TAREE); // parameters: dt pin, sck pin
DTM1650 display;

Modbus bus(BUS_ID, 0, PIN_REDE);
int8_t state = 0;

uint16_t temp[1] = { 0 };

float i;

unsigned long display_update;

// the setup function runs once when you press reset or power the board
void setup(){
	pinMode(BEEP_PIN, OUTPUT);

	bus.begin(19200);

	display.init();
	display.set_brightness(DTM1650_BRIGHTNESS_MAX);

	LoadCell.begin(); // start connection to HX711
	LoadCell.start(1000); // load cells gets 2000ms of time to stabilize
	LoadCell.setCalFactor(CALIBRATION); // calibration factor for load cell => strongly dependent on your individual setup

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
	state = bus.poll(temp, 2);
	button_tare.tick();

	LoadCell.update(); // retrieves data from the load cell
	i = LoadCell.getData(); // get output value
	if (i < 0) i = 0;

	temp[0] = i;

	if (millis() - display_update > DISP_UPDATE_FRAN)
	{
		display_update = millis();
		
		if (i > 1000)
		{
			i = 0;
			tone(BEEP_PIN, 1600, 200);
			display.send_digit(DTM1650_S_MINUS, 0);
			display.send_digit(DTM1650_S_MINUS, 1);
			display.send_digit(DTM1650_S_MINUS, 2);
			display.send_digit(DTM1650_S_MINUS, 3);
		}
		else
		{
			display.write_num(i);
		}
	}

	if (button_tare.isPress())
	{
		display.send_digit(DTM1650_S_MINUS, 0);
		display.send_digit(DTM1650_S_MINUS, 1);
		display.send_digit(DTM1650_S_MINUS, 2);
		display.send_digit(DTM1650_S_MINUS, 3);
		LoadCell.start(1000);
		display.set_dot(3, true);
		tone(BEEP_PIN, 1000, 300);
		delay(300);
	}

	//main_bus.tick();
}
