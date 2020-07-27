/* 
Dual display demo
Wade Ryan
2020/01/07

Compiling:

$ g++ -lwiringPi -lpthread 2displays.cpp /usr/local/lib/libwiringPiUDDrpi.a -o demo2

*/


#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <wiringPi.h>

#include <udd.h>

using namespace udd;

DisplayConfigruation d1Config;
DisplayConfigruation d2Config;

DisplayST7789R d1 = DisplayST7789R();
DisplayST7735R d2 = DisplayST7735R();

unsigned long long currentTimeMillis();
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


void drawSine(Image image, float offset, float speed, int maxX, int maxY, float waveHeight, Color color, int width) {
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


bool demoSineWave(int frameCount, long long start, Image image) {
    long long now = currentTimeMillis();
    float refVoltage = 5;

    long long second = now / 1000;
    if (lastSecond < 0) lastSecond = second;

    if (lastSecond != second) {
        lastFPS = fps;
        fps = 0;
        lastSecond = second;
    }
    ++fps;

    int imageWidth  = d1Config.height;
    int imageHeight = d1Config.width;
  
    int minX=0, minY = 0;
    int maxX = imageWidth-1;
    int maxY = imageHeight-1;
    int midY = minY + (maxY - minY) / 2;

    image.clear(BLACK);
    image.drawRectangle(minX, minY, maxX, maxY, WHITE, NONE, DOTTED, 1);

    // line markers
    for (int v = 1; v < refVoltage; ++v) {
        int y = maxY - ((v / refVoltage) * maxY);
        image.drawLine(1, y, maxX, y, BROWN, SOLID, 1);
    }

    
    image.drawCircle(maxX / 2, maxY / 2, maxY / 4, GRAY,        NONE,  DOTTED, 3);
    image.drawCircle(maxX / 2, maxY / 2, maxY / 4, LIGHT_GRAY,  NONE,  SOLID,  2);
    image.drawCircle(maxX / 2, maxY / 2, maxY / 4, WHITE,       NONE,  SOLID,  1);

    Color lineColor[9] = {
        //1      2     3        4      5         6        7       8    
        YELLOW, GREEN, MAGENTA, CYAN,  ORANGE,   RED, LIGHT_BLUE, LIGHT_GRAY
    };

    for (int line = 0; line < 4; ++line) {
        drawSine(image, frameCount + (4 - line) * (PI), .25, maxX, maxY, .65, lineColor[line], (line == 0) ? 3 : 2);
    }


    char message[32];
    sprintf(message, "%4.2f", refVoltage);
    image.drawText(minX + 1, minY + 1, message, &Font24, DARK_GRAY_BLUE, WHITE);

    sprintf(message, "%d-fps", lastFPS);
    image.drawText(maxX - (17 * strlen(message)) - 1, minY + 1, message, &Font24, BLACK, WHITE);

    d1.showImage(image,DEGREE_270);

    return ((now - start) < 8000);
}

void* display1Demo(void*) {

    Image bmp = Image(320, 240, BLACK);
    bmp.loadBMP("../images/BlueAngle4-320x240.bmp", 0, 0);

    Image chart = Image(d1Config.height, d1Config.width, BLACK);


    long count = 0;
    while (true) {
        d1.clearScreen(WHITE);
        delay(10);
        d1.clearScreen(RED);
        delay(10);
        d1.clearScreen(BLUE);
        delay(10);
        d1.clearScreen(GREEN);
        delay(10);
        d1.clearScreen(BLACK);

        d1.showImage(bmp, DEGREE_270);

        delay(1000);

        long long start = currentTimeMillis();

        while (demoSineWave(++count, start, chart));
    }
}


void configureDisplay1() {
    printf("--------------------------------------------\n");
    printf("-----d1 config------------------------------\n");
    d1Config.width = 240;
    d1Config.height = 320;
    d1Config.spiSpeed = 90000000;

    d1Config.CS = 21;
    d1Config.DC = 22;
    d1Config.RST = 23;
    d1Config.BLK = 7;

    d1.openDisplay(d1Config);
    d1.printConfiguration();
}

void configureDisplay2() {
    printf("--------------------------------------------\n");
    printf("-----d2 config------------------------------\n");
    d2Config.width  = 128;
    d2Config.height = 128;

    d2Config.CS = 24;
    d2Config.DC = 25;
    d2Config.RST = 26;
    d2Config.BLK = 7;

    d2Config.xOffset = 2;
    d2Config.yOffset = 1;
    d2Config.spiSpeed = 35000000;

    d2.openDisplay(d2Config);
    d2.printConfiguration();
}


void *display2Demo(void *) {
    while (true) {
        d2.clearScreen(WHITE);
        delay(100);
        d2.clearScreen(RED);
        delay(100);
        d2.clearScreen(GREEN);
        delay(100);
        d2.clearScreen(BLUE);
        delay(100);
        d2.clearScreen(BLACK);

        Image d2Image = Image(d2Config.width, d2Config.height, BLACK);

        Color pallet[10] = {
            RED, YELLOW, GREEN, BLUE, MAGENTA, CYAN
        };

        for (int i = 0; i < 6; ++i) {
            d2Image.drawRectangle(20 + i * 10, 0, i * 10 + 30, 128, pallet[i], FILL, SOLID, 1);
        }
        d2Image.drawText(2, 2, "Hello World!!!", &Font12, BLACK, WHITE);

        d2Image.drawRectangle(0, 0, d2Config.width - 1, d2Config.height - 1, WHITE, NONE, SOLID, 1);

        Color rings[4]{
            RED, WHITE, RED, WHITE
        };

        for (int r = 0; r < 40; ++r) {
            d2Image.drawCircle(d2Config.height - 45, d2Config.width - 45, r,  rings[r / 10], NONE, SOLID, 3);
        }   d2Image.drawCircle(d2Config.height - 45, d2Config.width - 45, 40, BLACK, NONE, SOLID, 1);


        d2.showImage(d2Image, DEGREE_90);

        delay(2000);
        d2Image.loadBMP("../images/MotorBike-128x128.bmp", 0, 0);
        d2.showImage(d2Image, DEGREE_90);

        delay(4000);
        d2Image.close();
    }
}

int main(void)
{
	wiringPiSetup();  // use wiring pi numbers

    // just incase there's somehting attached to the standard spi cs pins
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    digitalWrite(10, HIGH);
    digitalWrite(11, HIGH);

    int         demos = 3;
    pthread_t   threads[demos];
    char        message[demos][256];

    for (int i = 0; i < demos; ++i) {
        sprintf(message[i], "demo thread %d", i);
    }

    configureDisplay1();
    configureDisplay2();

    pthread_create(&threads[0], NULL, display1Demo, (void*)message[0]);
    pthread_create(&threads[1], NULL, display2Demo, (void*)message[1]);

    printf("press control-c to quit\n"); 

    while (true) {
        delay(1000);
    }

    return 0;
}