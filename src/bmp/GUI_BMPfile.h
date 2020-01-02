/*****************************************************************************
* | File      	:   GUI_BMPfile.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V2.1
* | Date        :   2019-10-10
* | Info        :   
* -----------------------------------------------------------------------------
* V2.1(2019-10-10):
* 1.Add GUI_ReadBmp_4Gray()
* V2.0(2018-11-12):
* 1.Change file name: GUI_BMP.h -> GUI_BMPfile.h
* 2.fix: GUI_ReadBmp()
*   Now Xstart and Xstart can control the position of the picture normally, 
*   and support the display of images of any size. If it is larger than 
*   the actual display range, it will not be displayed.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef __GUI_BMPFILE_H
#define __GUI_BMPFILE_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>




#ifdef __cplusplus
extern "C"
{
#endif  



/*Bitmap file header   14bit*/
typedef struct BMP_FILE_HEADER {
    uint16_t bType;        //File identifier
    uint32_t bSize;      //The size of the file
    uint16_t bReserved1;   //Reserved value, must be set to 0
    uint16_t bReserved2;   //Reserved value, must be set to 0
    uint32_t bOffset;    //The offset from the beginning of the file header to the beginning of the image data bit
} __attribute__ ((packed)) BMPFILEHEADER;    // 14bit

/*Bitmap information header  40bit*/
typedef struct BMP_INFO {
    uint32_t biInfoSize;      //The size of the header
    uint32_t biWidth;         //The width of the image
    uint32_t biHeight;        //The height of the image
    uint16_t biPlanes;          //The number of planes in the image
    uint16_t biBitCount;        //The number of bits per pixel
    uint32_t biCompression;   //Compression type
    uint32_t bimpImageSize;   //The size of the image, in bytes
    uint32_t biXPelsPerMeter; //Horizontal resolution
    uint32_t biYPelsPerMeter; //Vertical resolution
    uint32_t biClrUsed;       //The number of colors used
    uint32_t biClrImportant;  //The number of important colors
} __attribute__ ((packed)) BMPINFOHEADER;

/*Color table: palette */
typedef struct RGB_QUAD {
    uint8_t rgbBlue;               //Blue intensity
    uint8_t rgbGreen;              //Green strength
    uint8_t rgbRed;                //Red intensity
    uint8_t rgbReversed;           //Reserved value
} __attribute__ ((packed)) BMPRGBQUAD;
/**************************************** end ***********************************************/

uint8_t GUI_ReadBmp(const char *path, uint16_t Xstart, uint16_t Ystart);
uint8_t GUI_ReadBmp_4Gray(const char *path, uint16_t Xstart, uint16_t Ystart);
uint8_t GUI_ReadBmp_RGB(const char *path, uint16_t Xstart, uint16_t Ystart);





#ifdef __cplusplus
}  // extern "C"
#endif

#endif
