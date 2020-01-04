#include "Image.h"
#include "fonts.h"
#include <math.h>

#include <GUI_BMPfile.h>

using namespace udd;





Image::Image(int width, int height, Color backgroundColor) {
    this->width = width;
    this->height = height;

    this->backgroundColor = backgroundColor;

    uint32_t imageSize = width * height;
    uint32_t memorySize = imageSize * sizeof(ColorType);

    canvas = (ColorType*)malloc(memorySize);

    clear(backgroundColor);
}

void Image::clear(Color backgroundColor) {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            ColorType* xp = getPixel(x, y);
            xp->red = backgroundColor.color.red;
            xp->green = backgroundColor.color.green;
            xp->blue = backgroundColor.color.blue;
            xp->opacity = backgroundColor.color.opacity;
        }
    }
}

void Image::close() {
    free(canvas);
}

void Image::drawPixel(int x, int y, Color color) {
    if (x < 0) return;
    if (y < 0) return;
    if (x >= width) return;
    if (y >= height) return;

    ColorType* xp = getPixel(x, y);

    xp->red = color.color.red;
    xp->green = color.color.green;
    xp->blue = color.color.blue;
    xp->opacity = color.color.opacity;


    if (color.color.red != 0 ||
        color.color.green != 0 ||
        color.color.blue != 0
        ) {
    }
}



void Image::printPixel(int x, int y) {
    ColorType* xp = getPixel(x, y);

    _word cx = color2word(xp);

    printf("pixel(%d, %d)='#%2x%2x%2x  displayWord=%04x\n", x,y, xp->red, xp->green, xp->blue, cx);
}

ColorType* Image::getPixel(int x, int y) {

    if (x<0 || x > width || y<0 || y>height) {
        return NULL;
    }

    long offset = (y * width) + x;
    ColorType* xp = canvas + offset;


    return xp;
}


ColorType* Image::getPixel(int x, int y, Rotation rotation) {
    switch (rotation) {
    case DEGREE_0:    return getPixel(x, y); 
    case DEGREE_90:   return getPixel(y, height - x -1);
    case DEGREE_180:  return getPixel(width - x  -1, height - y-1);
    case DEGREE_270:  return getPixel(width -y -1, x);

    default:
        fprintf(stderr, "not implemented yet, rotation degree=%d", rotation);
        return getPixel(x, y);
    }
}


_word Image::color2word(ColorType* xp) {
    return ((0x1f & (xp->blue)) << 11) | ((0x3f & (xp->red)) << 5) | ((0x1f & (xp->green)));
}

void Image::drawPoint(int x, int y, Color color, int width) {

    switch (width) {
    case 0:
        return;
    case 1:
        drawPixel(x, y, color);
        break;
    case 2:
        drawPixel(x, y, color);
        drawPixel(x + 1, y, color);
        drawPixel(x, y + 1, color);
        drawPixel(x + 1, y + 1, color);
        break;
    case 3:
        drawPixel(x, y, color);
        drawPixel(x - 1, y, color);
        drawPixel(x + 1, y, color);

        drawPixel(x, y - 1, color);
        drawPixel(x, y + 1, color);
        break;
    default:
        fprintf(stderr, "not yet implemented (width=%d)\n", width);
        break;
    }

}


void Image::drawLine(int x1, int y1, int x2, int y2, Color color, LineStyle style, int width) {
    float x, y, dx, dy;
    int i, longestLeg, rx, ry;

    dx = x2 - x1;
    dy = y2 - y1;

    longestLeg = (abs(dx) >= abs(dy)) ? abs(dx) : abs(dy);

    dx = dx / longestLeg;
    dy = dy / longestLeg;
    rx = x1;
    ry = y1;
    x = x1;
    y = y1;

    for (i = 0; i <= longestLeg; ++i) {
        bool visible = true;

        if (style == DOTTED && i%2==0) {
            visible = false;
        }

        if (visible) drawPoint(rx, ry, color, width);


        if (abs(x2 - rx) == 0 && abs(y2 - ry) == 0) {
            break;
        }

        x += dx;
        y += dy;

        rx = round(x);
        ry = round(y);
    }
}


void Image::drawText(int Xstart, int Ystart, const char* pString,
    sFONT* Font, Color background, Color foreground) {


    int Xpoint = Xstart;
    int Ypoint = Ystart;


    while (*pString != '\0') {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + Font->Width) > width) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint + Font->Height) > height) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        drawChar(Xpoint, Ypoint, *pString, Font, background, foreground);

        //The next character of the address
        pString++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}

void Image::drawChar(int Xpoint, int Ypoint, const char Acsii_Char,
    sFONT* Font, Color background, Color foreground) {
    int Page, Column;


    uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    const unsigned char* ptr = &Font->table[Char_Offset];

    for (Page = 0; Page < Font->Height; Page++) {
        for (Column = 0; Column < Font->Width; Column++) {

            if (*ptr & (0x80 >> (Column % 8))) {
                drawPixel(Xpoint + Column, Ypoint + Page, foreground);
            }
            else {
                drawPixel(Xpoint + Column, Ypoint + Page, background);
            }
            //One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        }// Write a line
        if (Font->Width % 8 != 0)
            ptr++;
    }// Write all
}


/****************************************************************************
* | Method :   loadBMP
* | Author : Waveshare team
* | Function : Hardware underlying interface
* | Info :
    *Used to shield the underlying layers of each master
    * andenhance portability
    * ----------------
    * | This version : V2.1
    * | Date : 2019 - 10 - 10
    * | Info :
    *---------------------------------------------------------------------------- -
    *V2.1(2019 - 10 - 10) :
    *1.Add GUI_ReadBmp_4Gray()
    * V2.0(2018 - 11 - 12) :
    *1.Change file name : GUI_BMP.h->GUI_BMPfile.h
    * 2.fix : GUI_ReadBmp()
    * Now Xstart and Xstart can control the position of the picture normally,
    *and support the display of images of any size.If it is larger than
    * the actual display range, it will not be displayed.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this softwareand associated documnetation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions :
#
# The above copyright noticeand this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/

void Image::loadBMP(const char* filename, int Xstart, int Ystart) {
    FILE* fp;                     //Define a file pointer
    BMPFILEHEADER bmpFileHeader;  //Define a bmp file header structure
    BMPINFOHEADER bmpInfoHeader;  //Define a bmp info header structure

    // Binary file open
    if ((fp = fopen(filename, "rb")) == NULL) {
        fprintf(stderr,"Cann't open image the file: %s\n",filename);
        exit(0);
    }

    // Set the file pointer from the beginning
    fseek(fp, 0, SEEK_SET);
    fread(&bmpFileHeader, sizeof(BMPFILEHEADER), 1, fp);    //sizeof(BMPFILEHEADER) must be 14
    fread(&bmpInfoHeader, sizeof(BMPINFOHEADER), 1, fp);    //sizeof(BMPFILEHEADER) must be 50

    uint32_t Image_Width_Byte = bmpInfoHeader.biWidth * 3;
    uint32_t Bmp_Width_Byte = Image_Width_Byte;//Raw data RGB888
    uint8_t bmpImage[bmpInfoHeader.biWidth * bmpInfoHeader.biHeight * 3 ];
    memset(bmpImage, 0x00, sizeof(bmpImage));

    // Determine if it is a monochrome bitmap
    int readbyte = bmpInfoHeader.biBitCount;

    if (readbyte != 24) {
        fprintf(stderr, "Bmp image is not a 4-color bitmap!\n");
        exit(0);
    }
    // Read image data into the cache
    uint16_t x, y;
    uint8_t Rdata;
    fseek(fp, bmpFileHeader.bOffset, SEEK_SET);
    for (y = 0; y < bmpInfoHeader.biHeight; y++) {//Total display column
        for (x = 0; x < Bmp_Width_Byte; x++) {//Show a line in the line
            if (fread((char*)&Rdata, 1, 1, fp) != 1) {
                perror("get bmpdata:\r\n");
                break;
            }
            unsigned int imageOffset = x + (bmpInfoHeader.biHeight - y - 1) * Image_Width_Byte;
            if (imageOffset > sizeof(bmpImage)) {
                fprintf(stderr, "image out of bounds");
                exit(9);
            }
            bmpImage[imageOffset] = Rdata;
        }
    }
    fclose(fp);

    // Refresh the image to the display buffer based on the displayed orientation

    for (y = 0; y < bmpInfoHeader.biHeight; y++) {
        for (x = 0; x < bmpInfoHeader.biWidth; x++) {
            if (x > width || y > height) {
                continue;
            }

            uint8_t blue   = bmpImage[x * 3 + y * Image_Width_Byte + 0];
            uint8_t green  = bmpImage[x * 3 + y * Image_Width_Byte + 1];
            uint8_t red    = bmpImage[x * 3 + y * Image_Width_Byte + 2];

            Color color = Color(red, green, blue);
            drawPixel(Xstart + x, Ystart + y, color);
        }
    }
    return;
}


void Image::drawRectangle(int x1, int y1, int x2, int y2, Color Color, 
        FillPattern pattern, LineStyle lineStyle, int width) {
    
    switch (pattern) {
    case NONE:
        drawLine(x1, y1, x2, y1, Color, lineStyle, width);
        drawLine(x1, y1, x1, y2, Color, lineStyle, width);
        drawLine(x2, y2, x2, y1, Color, lineStyle, width);
        drawLine(x2, y2, x1, y2, Color, lineStyle, width);
        break;
    case FILL:
        for (int y = y1; y < y2; y++) {
            drawLine(x1, y, x2, y, Color, lineStyle, width);
        }
        break;
    case MASK:
        fprintf(stderr, "drawRectangle::mask not yet implemented\n");
        break;
    default:
        fprintf(stderr, "drawRectangle::unknown pattern\n");
        break;
    }
}


void Image::arcPoint(int x, int y, int radius, double degree, int* xPoint, int* yPoint) {
    double px = radius * cos(degree * PI / 180.0);
    double py = radius * sin(degree * PI / 180.0);

    int ix = x + round(px);
    int iy = y + round(py);

    *xPoint = ix;
    *yPoint = iy;
}

void Image::drawCircle(int x, int y, int radius, Color color, FillPattern pattern, LineStyle lineStyle, int width) {

    double xPoint, yPoint;

    int lx=-1, ly=-1;
    int ct = 0;

    for (int degree = 0; degree < 360; degree += 1) {
        
        xPoint = radius * cos(degree * PI / 180.0);
        yPoint = radius * sin(degree * PI / 180.0);

        switch (lineStyle) {
        case SOLID:
            if (lx != xPoint || ly != yPoint) {
                drawPoint(x + round(xPoint), y + round(yPoint), color, width);
                lx = xPoint;
                ly = yPoint;
            }
            break;
        case DOTTED:
            if (lx != xPoint || ly != yPoint) {
                if (++ct % 2 == 1) {
                    drawPoint(x + round(xPoint), y + round(yPoint), color, width);
                }
                lx = xPoint;
                ly = yPoint;
            }
        }
    }
}