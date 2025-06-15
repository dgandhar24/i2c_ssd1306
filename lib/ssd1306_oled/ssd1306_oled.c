#include "ssd1306_oled.h"


uint8_t display_buffer[8][128];
uint32_t reg32 __attribute__((unused));
uint32_t I2C_OLED = I2C1;
uint8_t OLED_ADDRESS = DEFAULT_7bit_OLED_SLAVE_ADDRESS;
uint8_t WIDTH = 128;
uint8_t HEIGHT = 64;

const uint8_t InitLen = 26;

const uint8_t Init[] = {
  0xAE,         // Display OFF
  0xA8, 0x3F,   // set multiplex (HEIGHT-1): 0x1F for 128x32, 0x3F for 128x64
  0xD3, 0x00,   // Display offset to 0
  0x40,         // Set display start line to 0
  0x8D, 0x14,   // Charge pump enabled
  0x20, 0x00,      // Memory addressing mode 0x00 Horizontal 0x01 Vertical
  0xDA, 0x12,   // Set COM Pins hardware configuration to sequential
  0x81, 0x80,   // Set contrast
  0xD9, 0xF1,   // Set pre-charge period
  0xDB, 0x40,   // Set vcom detect  
  
  0x22, 0x00, 7, // Page min to max
  0x21, 0x00, 127, // Column min to max

  0xAF  // Display on
};

const uint8_t ssd1306_init_sequence [] = {	// Initialization Sequence
	0xAE,			// Set Display ON/OFF - AE=OFF, AF=ON
	0xD5, 0xF0,		// Set display clock divide ratio/oscillator frequency, set divide ratio
	0xA8, 0x3F,		// Set multiplex ratio (1 to 64) ... (height - 1)
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0x40 | 0x00,	// Set start line address, at 0.
	0x8D, 0x14,		// Charge Pump Setting, 14h = Enable Charge Pump
	0x20, 0x00,		// Set Memory Addressing Mode - 00=Horizontal, 01=Vertical, 10=Page, 11=Invalid
	0xA0 | 0x01,	// Set Segment Re-map
	0xC8,			// Set COM Output Scan Direction
	0xDA, 0x12,		// Set COM Pins Hardware Configuration - 128x32:0x02, 128x64:0x12
	0x81, 0x3F,		// Set contrast control register - 0x01 to 0xFF - Default: 0x3F
	0xD9, 0x22,		// Set pre-charge period (0x22 or 0xF1)
	0xDB, 0x20,		// Set Vcomh Deselect Level - 0x00: 0.65 x VCC, 0x20: 0.77 x VCC (RESET), 0x30: 0.83 x VCC
	0xA4,			// Entire Display ON (resume) - output RAM to display
	0xA6,			// Set Normal/Inverse Display mode. A6=Normal; A7=Inverse
	0x2E,			// Deactivate Scroll command
	0xAF,			// Set Display ON/OFF - AE=OFF, AF=ON
	0x22, 0x00, 0x3F,	// Set Page Address (start,end) 0 - 63
	0x21, 0x00,	0x7f,	// Set Column Address (start,end) 0 - 127
};

#define DISPLAY_ON (0xAF)
#define DISPLAY_ON (0xAE)

#define SET_ADDR_MODE (0x20)
#define HZ_ADDR_MODE (0x00)
#define VT_ADDR_MODE (0x01)
#define PAGE_ADDR_MODE (0x02)

#define SET_START_ADDR (0xB0) // for page addressing mode

#define SET_VT_MIRROR (0xC0) // Mirror vertically

#define SET_SCAN_DIR (0xC8) // Set COM o/p scan direction

#define SET_LOW_COL_ADDR (0x00)
#define SET_HIGH_COL_ADDR (0x10)

#define SET_START_LINE_ADDR (0x40)




// const uint8_t Init[] = {
//   0xAE,
//   0x20,
//   0x01,
//   0xB0,
//   0xC8,//C0
//   0x00,
//   0x10,
//   0x40,
//   0xFF,
//   0xA8, //A7
//   0xFF, //0xA8
//   0x3F,
//   0xA4,
//   0xD3,
//   0x00,
//   0xD5,
//   0xF0,
//   0xD9,
//   0x22,
//   0xDA,
//   0x12,
//   0xDB,
//   0x20,
//   0x8D,
//   0x14,
//   0xAF,
// };


void ssd1306_send_data(uint8_t spec, uint8_t data) {
  I2C_Start(OLED_ADDRESS, WRITE);
  // I2C_Address(OLED_ADDRESS);
  I2C_Write(spec);
  I2C_Write(data);
  I2C_Stop();
}

void drawPixel(int x, int y, int color) {
  if(x >= 128 || y >= 64 || x < 0 || y < 0) {
    return;
  }
  // odd
  if(y&1) y = y/2 + 32;
  else y = y/2;
  int row_byte = y/8;
  int row_bit = y%8;

  if(color)
    display_buffer[row_byte][x] |= (1 << row_bit);
  else
    display_buffer[row_byte][x] &= ~(1 << row_bit);
}

void drawLine() {
  for(int x=0, y=0; x<64; x++, y++) {
    drawPixel(x, y, 1);
  }
  for(int x=64, y=64; x<128; x++, y--) {
    drawPixel(x, y, 1);
  }
}

void OLED_init(void) {
    I2C_Config();
    I2C_Start(OLED_ADDRESS, WRITE);
    // I2C_Address(OLED_ADDRESS);
    I2C_Write(I2C_COMMAND);
    for (uint8_t i = 0; i < sizeof(ssd1306_init_sequence); i++)
    {
        I2C_Write(ssd1306_init_sequence[i]);
    }
    I2C_Stop();
}

void OLED_Clear(void) {
    I2C_Start(OLED_ADDRESS, WRITE);
    // I2C_Address(OLED_ADDRESS);
    I2C_Write(I2C_DATA);
    for (uint16_t i = 0; i < 1024; i++)
    {
        I2C_Write(0x00);
    }
    I2C_Stop();
}

void setBuffer() {
  uint8_t x = 0xFF;
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 128; j++) {
      display_buffer[i][j] = 0x00;
      // x = x^0xFF;
    }
  }
}

void writeBuffer(int i, int j, uint8_t data) {
  display_buffer[i][j] = data;
  ssd1306_send_data(I2C_DATA, display_buffer[i][j]);
}

void display(void) {
    for (uint8_t j = 0; j < 128; j++) {
      for(uint8_t i = 0; i < 8; i++) {
        I2C_Start(OLED_ADDRESS, WRITE);
        // I2C_Address(OLED_ADDRESS);
        I2C_Write(I2C_DATA);
        I2C_Write(display_buffer[i][j]);
        I2C_Stop();
        // delay(2);
      }
    }
}

void drawFlatLine(int x, int y, int h, uint8_t horizontal) {
  if(horizontal) {
    if(h < 0) x = x - h;
    for(int i = x; i < x+h; i++) {
      drawPixel(i, y, 1);
    }
  }
  else {
    if(h < 0) y = y - h;
    for(int i = y; i < y+h; i++) {
      drawPixel(x, i, 1);
    }
  } 
}

void draw_rectangle(int x0, int y0, int x1, int y1) {
  if(x1 < x0) {
    int temp = x0;
    x0 = x1;
    x1 = temp;
    temp = y0;
    y0 = y1;
    y1 = temp;
  }

  if(y0 < y1) {
    drawFlatLine(x0, y0, x1-x0, true); // ---------->
    drawFlatLine(x0, y1, x1-x0, true); // <----------
    drawFlatLine(x0, y0, y1-y0, false); // V
    drawFlatLine(x1, y0, y1-y0, false); // ^
  }
}

void drawCircle(int xc, int yc, int x, int y) {
    drawPixel(xc+x, yc+y, 1);
    drawPixel(xc-x, yc+y, 1);
    drawPixel(xc+x, yc-y, 1);
    drawPixel(xc-x, yc-y, 1);
    drawPixel(xc+y, yc+x, 1);
    drawPixel(xc-y, yc+x, 1);
    drawPixel(xc+y, yc-x, 1);
    drawPixel(xc-y, yc-x, 1);
}

// Function for circle-generation
// using Bresenham's algorithm
void draw_circle(int xc, int yc, int r) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    drawCircle(xc, yc, x, y);
    while (y >= x)
    {
        // for each pixel we will
        // draw all eight pixels
        
        x++;

        // check for decision parameter
        // and correspondingly 
        // update d, x, y
        if (d > 0)
        {
            y--; 
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;
        drawCircle(xc, yc, x, y);
    }
}
