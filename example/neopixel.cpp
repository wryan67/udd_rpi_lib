/*
NeoPixel matrix display demo
Wade Ryan
2020/01/07

Compiling:

$ g++ -lwiringPi -lwiringPiUDDrpi 1display.cpp -o demo1

*/

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <wiringPi.h>

#include <udd.h>

using namespace udd;

DisplayConfigruation d1Config;

DisplayNeoPixel d1 = DisplayNeoPixel();

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
    bool first = true;
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

long rainbowColor=0;

void rainbow() {
    long long start = currentTimeMillis();
    long long elapsed = 0;

    while (elapsed<30*1000) {
        int color=++rainbowColor%256;
        elapsed = currentTimeMillis() - start;

        for (int x=0; x<d1Config.width; ++x) {
            for (int y=0; y<d1Config.height; ++y) {
                Color clr=Color(++color%256);
                // printf("wheel=%3d r=%3d g=%3d b=%3d\n",color%256,clr.color.red, clr.color.green, clr.color.blue);
                Pixel px=Pixel(x,y,clr);
                d1.setPixel(px);
            }
        }
        d1.render(d1Config.screenRotation);
        usleep(66);
    }
}


long gryoPosition=0;
void gyro() {
    long long start = currentTimeMillis();
    long long elapsed = 0;

    Image bmp = Image(11, 11, BLACK);


    while (elapsed<30*1000) {
        int color=++gryoPosition%2;
        elapsed = currentTimeMillis() - start;

        bmp.loadBMP("images/gyro/gyro-1a.bmp", 0, 0);

        d1.showImage(bmp);
        //delay(5000);

        bmp.printPixel(0,0);
        bmp.printPixel(0,1);
        bmp.printPixel(0,2);
        bmp.printPixel(0,3);

        exit(0);
    }
}


void display1Demo() {

//    Image bmp = Image(320, 240, BLACK);
//    bmp.loadBMP("images/BlueAngle4-320x240.bmp", 0, 0);

//    Image chart = Image(d1Config.height, d1Config.width, BLACK);


    long count = 0;
    while (true) {
        printf("WHITE\n");
        d1.clear(WHITE);
        delay(250);

        printf("RED\n");
        d1.clear(RED);
        delay(250);

        printf("GREEN\n");
        d1.clear(GREEN);
        delay(250);

        printf("BLUE\n");
        d1.clear(BLUE);
        delay(250);

//        rainbow();


        // long long start = currentTimeMillis();
        //  while (demoSineWave(++count, start, chart));


        printf("Graduated Colors\n");
        int c=0;
        for (int y=0;y<d1.config.height;++y) {
            Color color=Color(c); c+=24;
            for (int x=0;x<d1.config.width;++x) {
                d1.setPixel(Pixel(x,y,color));
            }
        }
        d1.render(d1Config.screenRotation);


        gyro();

        exit(0);

    }
}



void configureDisplay1() {
    printf("--------------------------------------------\n");
    printf("-----d1 config------------------------------\n");
    d1Config.width =            11;
    d1Config.height =           11;

    d1Config.stripType =        WS2812_STRIP; 
    d1Config.targetFreq =       WS2811_TARGET_FREQ;
    d1Config.dmaChannel =       10;
    d1Config.gpioPin =          18;
    d1Config.brightness =       10;
    d1Config.screenRotation  =  udd::DEGREE_0;

    d1.openDisplay(d1Config);
    d1.printConfiguration();


// bottom row (0)
//  These are physicall there right now
//  d1.addGhostPixel(Point( 0, 0));
//  d1.addGhostPixel(Point( 1, 0));
//  d1.addGhostPixel(Point( 2, 0));

    d1.addGhostPixel(Point( 8, 0));
    d1.addGhostPixel(Point( 9, 0));
    d1.addGhostPixel(Point(10, 0));

// row 1
    d1.addGhostPixel(Point( 0, 1));
    d1.addGhostPixel(Point( 1, 1));

    d1.addGhostPixel(Point( 9, 1));
    d1.addGhostPixel(Point(10, 1));

// row 2
    d1.addGhostPixel(Point( 0, 2));
    d1.addGhostPixel(Point(10, 2));

// row 8
    d1.addGhostPixel(Point( 0, 8));
    d1.addGhostPixel(Point(10, 8));


// row 9
    d1.addGhostPixel(Point( 0, 9));
    d1.addGhostPixel(Point( 1, 9));

    d1.addGhostPixel(Point( 9, 9));
    d1.addGhostPixel(Point(10, 9));

// top row (10)
    d1.addGhostPixel(Point( 0,10));
    d1.addGhostPixel(Point( 1,10));
    d1.addGhostPixel(Point( 2,10));

    d1.addGhostPixel(Point( 8,10));
    d1.addGhostPixel(Point( 9,10));
    d1.addGhostPixel(Point(10,10));
}



int main(void)
{
	wiringPiSetup();  // use wiring pi numbers

    configureDisplay1();
    
    printf("press control-c to quit\n");

    display1Demo();


    return 0;
}
