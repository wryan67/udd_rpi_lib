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


    Image image = Image(240, 320, BLACK);


    image.drawPixel(4, 40, WHITE);
    image.drawPixel(4, 41, RED);
    image.drawPixel(4, 42, GREEN);
    image.drawPixel(4, 43, BLUE);


    for (int i = 40; i < 45; ++i) {
        image.printPixel(4, i);
    }


    DisplayConfigruation config;


    Display d1 = Display(config);

    d1.clear(WHITE);
    delay(250);
    d1.clear(RED);
    delay(250);
    d1.clear(BLUE);
    delay(250);
    d1.clear(GREEN);
    delay(250);
    d1.clear(BLACK);


    d1.showImage(image);

    image.close();

    return 0;
}