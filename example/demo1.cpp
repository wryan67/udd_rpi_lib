/*
Single display demo
Wade Ryan
2020/01/07

Compiling:

$ g++ -lwiringPi -lwiringPiUDDrpi demo1.cpp -o demo1

*/

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <wiringPi.h>

#include <udd.h>

using namespace udd;

DisplayConfigruation d1Config;

DisplayST7789R d1 = DisplayST7789R();

unsigned long long currentTimeMillis();
long long lastTime = currentTimeMillis();
long long lastSecond;
int fps, lastFPS = 0;

int spiSpeed = 90000000;


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

void rotationDemo() {

    d1.clearScreen(DARK_BLUE);
    int tmpWidth = 100;
    int tmpHeight = 51;

    Image tmpimg = Image(tmpWidth, tmpHeight, BLACK);
    
    tmpimg.drawLine(00, 00, 99, 00, WHITE, SOLID, 1);  // top 
    tmpimg.drawLine(00, 00, 00, 50, WHITE, SOLID, 1);  // left
    tmpimg.drawLine(99, 00, 99, 50, WHITE, SOLID, 1);  // right
    tmpimg.drawLine(00, 50, 99, 50, WHITE, SOLID, 1);  // bottom

    printf("--- DEGREE_0 --------------------------------------------------\n");
    tmpimg.drawText(10, 10, "D_0", &Font24, BLACK, RED);
    d1.showImage(tmpimg, Point(30, 10), Point(30 + tmpWidth - 1, 10 + tmpHeight - 1), DEGREE_0); // RED

    printf("--- DEGREE_90 --------------------------------------------------\n");
    tmpimg.drawText(10, 10, "D_90", &Font24, BLACK, YELLOW);
    d1.showImage(tmpimg, Point(30, 10), Point(30 + tmpWidth - 1, 10 + tmpHeight - 1), DEGREE_90); // YELLOW

    printf("--- DEGREE_180 --------------------------------------------------\n");
    tmpimg.drawText(10, 10, "D_180", &Font24, BLACK, GREEN);
    d1.showImage(tmpimg, Point(30, 10), Point(30 + tmpWidth - 1, 10 + tmpHeight - 1), DEGREE_180); // GREEN

    printf("--- DEGREE_270 --------------------------------------------------\n");
    tmpimg.drawText(10, 10, "D_270", &Font24, BLACK, CYAN);
    d1.showImage(tmpimg, Point(30, 10), Point(30 + tmpWidth - 1, 10 + tmpHeight - 1), DEGREE_270); // CYAN


}

void display1Demo() {
    printf("demo1\n"); fflush(stdout);

    Image bmp = Image(320, 240, BLACK);
    bmp.loadBMP("images/BlueAngle4-320x240.bmp", 0, 0);

    Image chart = Image(d1Config.height, d1Config.width, BLACK);


    long count = 0;
    while (true) {
        int solidsDelay = 200;
        d1.clearScreen(WHITE);
        delay(solidsDelay);
        d1.clearScreen(RED);
        delay(solidsDelay);
        d1.clearScreen(GREEN);
        delay(solidsDelay);
        d1.clearScreen(BLUE);
        delay(solidsDelay);
        d1.clearScreen(BLACK);

        d1.showImage(bmp, DEGREE_270);
        delay(2000);

        rotationDemo();
        delay(4000);

        long long start = currentTimeMillis();

        while (demoSineWave(++count, start, chart));
    }
}


void configureDisplay1() {
    printf("--------------------------------------------\n");
    printf("-----d1 config------------------------------\n");
    d1Config.width = 240;
    d1Config.height = 320;
    d1Config.spiSpeed = spiSpeed;

    d1Config.CS = 21;
    d1Config.DC = 22;
    d1Config.RST = 23;
    d1Config.BLK = 7;

    d1.openDisplay(d1Config);
    d1.printConfiguration();
}



int main(int argc, char **argv)
{
	wiringPiSetup();  // use wiring pi numbers

    if (argc > 0) {
        sscanf(argv[argc - 1], "%d", &spiSpeed);
    }

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

    printf("press control-c to quit\n");

    display1Demo();


    return 0;
}
