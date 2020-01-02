#include <wiringPi.h>
#include <stdio.h>
#include <Display.h>
#include <Image.h>

int main(void)
{
	wiringPiSetup();

    // just incase there's somehting attached to these pins
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);


    DisplayConfigruation config;
    config.spiSpeed = 65000000;


    Display d1 = Display(config);
    d1.printConfiguration();

    d1.clear(WHITE);
    delay(10);
    d1.clear(RED);
    delay(10);
    d1.clear(BLUE);
    delay(10);
    d1.clear(GREEN);
    delay(10);
    d1.clear(BLACK);


    Image image = Image(config.width, config.height, BLACK);

    //              x   y
    for (int i = 4; i < 100; ++i) {
        image.drawPixel(i, 30, WHITE);
        image.drawPixel(i, 31, WHITE);
        image.drawPixel(i, 40, RED);
        image.drawPixel(i, 41, RED);
        image.drawPixel(i, 50, GREEN);
        image.drawPixel(i, 51, GREEN);
        image.drawPixel(i, 60, BLUE);
        image.drawPixel(i, 61, BLUE);
    }

    d1.showImage(image);


    image.close();

    return 0;
}