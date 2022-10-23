/*
NeoPixel matrix display demo with auto brightness
* requires ads1115 && photo resistor
Wade Ryan
2020/01/07

Compiling:

$ g++ -lwiringPi -lwiringPiUDDrpi 1display.cpp -o demo1

I'm using a custom neopixel matrix that is basically 11x11.  
Some of the pixels are "missing" on purpose, 
so that the display is shaped like a stop sign.

Here is the layout:

      First Pixel
              |
              V  --->
              * * * * * -------\
   /------- * * * * * * * <----/
   \----> * * * * * * * * * -------\
        * * * * * * * * * * * <----/
        * * * * * * * * * * *
        * * * * * * * * * * *
        * * * * * * * * * * *
        * * * * * * * * * * *
          * * * * * * * * *  
            * * * * * * *    
              * * * * * 
*/

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include <wiringPi.h>

#include <udd.h>
#include <ads1115rpi.h>
#include <wiringPiI2C.h>

#include <pthread.h>

using namespace udd;

DisplayConfiguration d1Config;

DisplayNeoPixel d1 = DisplayNeoPixel();

unsigned long long currentTimeMillis();
long long lastTime = currentTimeMillis();
long long lastSecond;
int fps, lastFPS = 0;

int ADS1115_ADDRESS=0x48;
float vRef = 3.3;

int minBrightness=8;
int nomBrightness=32;
int maxBrightness=128;

float minVBrightness=0.5;
float maxVBrightness=2.0;

unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return
        (unsigned long long)(currentTime.tv_sec) * 1000 +
        (unsigned long long)(currentTime.tv_usec) / 1000;
}


void drawSine(Image &image, float offset, float speed, int maxX, int maxY, float waveHeight, Color color, int width) {
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


bool demoSineWave(int frameCount, long long start, Image &image) {
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

    while (elapsed<10*1000) {
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


void gyro() {
    long long start = currentTimeMillis();
    long long elapsed = 0;

    Image bmp = Image(11, 11, BLACK);
    Image tilt0 = Image(11, 11, BLACK);
    Image tilt1 = Image(11, 11, BLACK);

    tilt0.loadBMP("images/gyro/tilt0.bmp", 0, 0);
    tilt1.loadBMP("images/gyro/tilt1.bmp", 0, 0);

    int mirrorScreen=0;

    while (elapsed<12*1000) {
        elapsed = currentTimeMillis() - start;

        Rotation degree=DEGREE_0;

        ScreenMirror mirror=(mirrorScreen++%2==0)?NORMAL:HORIZONTAL; 


        bmp.loadBMP("images/gyro/gyro-0.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(2000);

        bmp.loadBMP("images/gyro/gyro-1.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(500);

        bmp.loadBMP("images/gyro/gyro-0.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(250);

        bmp.loadBMP("images/gyro/gyro-1.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(250);

        bmp.loadBMP("images/gyro/gyro-2.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(250);

        bmp.loadBMP("images/gyro/gyro-3.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(500);

        bmp.loadBMP("images/gyro/gyro-4.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(750);

        bmp.loadBMP("images/gyro/gyro-3.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(500);
    
        bmp.loadBMP("images/gyro/gyro-4.bmp", 0, 0);
        d1.showImage(bmp,degree,mirror);
        delay(250);


        for (int i=0;i<10;++i) {

            d1.showImage(tilt0,DEGREE_0,NORMAL);
            delay(200); 

            d1.showImage(tilt1,DEGREE_0,NORMAL);
            delay(200);
        }
    }
}


void display1Demo() {

//    Image bmp = Image(320, 240, BLACK);
//    bmp.loadBMP("images/BlueAngle4-320x240.bmp", 0, 0);

    Image bmp = Image(11, 11, DARK_BLUE);

    Image chart = Image(d1Config.height, d1Config.width, BLACK);


    long count = 0;
    while (true) {
        d1.clearScreen(WHITE);
        delay(250);

        d1.clearScreen(RED);
        delay(250);

        d1.clearScreen(GREEN);
        delay(250);

        d1.clearScreen(BLUE);
        delay(250);


//        long long start = currentTimeMillis();
//        while (demoSineWave(++count, start, chart));


        printf("Graduated Colors\n");
        int c=0;
        for (int y=0;y<d1.config.height;++y) {
            Color color=Color(c); c+=24;
            for (int x=0;x<d1.config.width;++x) {
                d1.setPixel(Pixel(x,y,color));
            }
        }

        rainbow();


        d1.render(d1Config.screenRotation);
        delay(1000);

        gyro();

        int radius=5;

        bmp.drawCircle(5,5,radius,YELLOW,FILL,SOLID,1);
        bmp.drawCircle(5,5,radius-1,DARK_BLUE,FILL,SOLID,1);
        bmp.drawText(3,2,"1",&Font8,DARK_BLUE,WHITE);

        d1.showImage(bmp);
        delay(3000);

        bmp.drawText(3,2,"2",&Font8,DARK_BLUE,WHITE);
        d1.showImage(bmp);
        delay(2500);

        bmp.drawText(3,2,"3",&Font8,DARK_BLUE,WHITE);
        d1.showImage(bmp);
        delay(5000);

    }
}



void configureDisplay1() {
    printf("--------------------------------------------\n");
    printf("-----d1 config------------------------------\n");
    d1Config.width =            11;
    d1Config.height =           11;

    d1Config.stripType =        WS2811_STRIP_GRB; 
    d1Config.targetFreq =       WS2811_TARGET_FREQ;
    d1Config.dmaChannel =       10;
    d1Config.gpioPin =          18;
    d1Config.brightness =       16;
    d1Config.screenRotation  =  udd::DEGREE_0;

    d1.openDisplay(d1Config);
    d1.printConfiguration();


// bottom row (0)
//  These are physicall there right now
    d1.addGhostPixel(Point( 0, 0));
    d1.addGhostPixel(Point( 1, 0));
    d1.addGhostPixel(Point( 2, 0));

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

pthread_t threadCreate(void *(*method)(void *), const char *description) {
	pthread_t threadId;
	int status= pthread_create(&threadId, NULL, method, NULL);
	if (status != 0) {
		printf("%s::thread create failed %d--%s\n", description, status, strerror(errno));
		exit(9);
	}
	pthread_detach(threadId);
	return threadId;
}

void* setBrightness(void *) {
  int handle = wiringPiI2CSetup(ADS1115_ADDRESS);
  int brightness;

  while (true) {
    float volts[4];

    int startTime=currentTimeMillis();
    for (int i=0;i<4;++i) {
      float v=readVoltage(handle);
      if (v>6) {
        v=0;
      }
      volts[i]=v;
    }

    long long cTime=currentTimeMillis();
    int elapsed = cTime - startTime;

    if (volts[0]<1.0)  {
        brightness=maxBrightness;
    } else if (volts[0]<2.0) {
        brightness=nomBrightness;
    } else {
        brightness=minBrightness;
    }


    d1.setBrightness(brightness);

    printf("%lld %7d %8.2f %8.2f brightness=%d\n", 
           cTime, elapsed, volts[0], volts[1], brightness);
    fflush(stdout);

    delay(1000);
  }
}

int main(int argc, char **argv)
{
    wiringPiSetup();  // use wiring pi numbers


    configureDisplay1();
    
    printf("press control-c to quit\n");

    threadCreate(setBrightness, "setBrightness");

    if (argc>1) {
        Image bmp = Image(11, 11, BLACK);
        bmp.loadBMP(argv[1], 0, 0);
        d1.showImage(bmp);
        exit(0);
    }


    display1Demo();


    return 0;
}
