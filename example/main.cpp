#include <wiringPi.h>
#include <stdio.h>
#include <Display.h>
#include <Image.h>
#include <sys/time.h>
#include <math.h>
using namespace uddDisplay;
using namespace uddImage;

DisplayConfigruation config;
Display d1;
unsigned long long currentTimeMillis();

static const double PI = 3.14159265358979;

long long lastTime = currentTimeMillis();
long long lastSecond;
int fps, lastFPS = 0;


unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return
        (unsigned long long)(currentTime.tv_sec) * 1000 +
        (unsigned long long)(currentTime.tv_usec) / 1000;
}


void drawSine(uddImage::Image image, float offset, float speed, int maxX, int maxY, float waveHeight, Color color, int width) {
    bool first = true;;
    int lx = -1, ly = -1;
    double vx = 0;
    float vmax = 2;
    for (int x = 0; x < maxX; ++x) {
        vx = (speed * offset) + (6 * PI) * x / maxX;
        int y = (maxY / 2) - sin(vx) * (maxY * waveHeight / 2);
        if (first) {
            first = false;
            continue;
        }
        else {
            image.drawLine(lx, ly, x, y, color, SOLID, width);
        }
        lx = x;
        ly = y;
    }
}


void frame(int frameCount) {
    long long now = currentTimeMillis();
    float refVoltage = 5;


    uddImage::Image image = uddImage::Image(config.height, config.width, BLACK);

    int minX, minY = 0;
    int maxX = config.height-1;
    int maxY = config.width-1;
    int midY = minY + (maxY - minY) / 2;

    image.drawLine(minX, minY, maxX, minY, WHITE, SOLID, 1);  //top
    image.drawLine(minX, maxY, maxX, maxY, WHITE, SOLID, 1);
    image.drawLine(maxX, minY, maxX, maxY, WHITE, SOLID, 1);  //right
    image.drawLine(minX, minY, minX, maxY, WHITE, SOLID, 1);  //left

    // line markers
    for (int v = 1; v < refVoltage; ++v) {
        int y = maxY - ((v / refVoltage) * maxY);
        image.drawLine(1, y, maxX, y, BROWN, SOLID, 1);
    }

    Color lineColor[9] = {
        //1      2     3        4      5         6        7       8    
        YELLOW, GREEN, MAGENTA, CYAN,  ORANGE,   RED, LIGHT_BLUE, LIGHT_GRAY
    };

    for (int line = 0; line < 4; ++line) {
        drawSine(image, frameCount + line * (PI), .25, maxX, maxY, .65, lineColor[line], (line==0)?2:1);
    }

    d1.showImage(image,DEGREE_90);


    image.close();
}

int main(void)
{
	wiringPiSetup();

    // just incase there's somehting attached to these pins
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);


    config.spiSpeed = 65000000;


    d1 = Display(config);
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

    long count = 0;
    while (true) {
        frame(++count);
    }




    return 0;
}