#include <SPI.h>
#include <homespun_font.h>

#define TEXT_HEIGHT 8
#define TEXT_WIDTH  7

//data/command pin
const uint8_t DCpin = 3;
//chip select pin
const uint8_t SSpin = 4;
//refresh pin
const uint8_t RFpin = 5;

const uint16_t MAXWIDTH = 240;
const uint16_t MAXLENGTH = 320;

//color_code examples
//COLOR_RED   = {63, 0, 0};
//COLOR_GREEN = {0, 63, 0};
//COLOR_BLUE  = {0, 0, 63};
//COLOR_BLACK = {0, 0, 0};
//COLOR_WHITE = {63, 63, 63};

struct color_code {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

  color_code white = {63, 63, 63};
  color_code black = {0, 0, 0};
  color_code red   = {63, 0, 0};
  color_code green = {0, 63, 0};
  color_code blue  = {0, 0, 63};


struct pt_coord {
  uint16_t x;
  uint16_t y;
};

struct ln_coord {
  int x;
  int y;
};

void setup() {

  /*
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  Serial.println("Serial Connection Established.");
  Serial.println("Waiting for input...");
  Serial.read();
  while(!Serial.available());
  */
  
  pinMode(DCpin, OUTPUT);
  pinMode(SSpin, OUTPUT);
  pinMode(RFpin, OUTPUT);

  digitalWrite(DCpin, HIGH);
  digitalWrite(SSpin, HIGH);
  digitalWrite(RFpin, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

  lcd_init();
  delay(100);
  lcd_clear();
  delay(100);
}


void lcd_write_cmd(uint8_t cmd) {
  digitalWrite(DCpin, LOW);
  digitalWrite(SSpin, LOW);
  SPI.transfer(cmd);
  digitalWrite(SSpin, HIGH);
}

void lcd_write_data(uint8_t dat) {
  digitalWrite(DCpin, HIGH);
  digitalWrite(SSpin, LOW);
  SPI.transfer(dat);
  digitalWrite(SSpin, HIGH);
}

void lcd_init() {
  delay(100);
  lcd_write_cmd(0x11);
  delay(100);
  lcd_write_cmd(0x29);
  delay(100);
  lcd_write_cmd(0x20);
}

void lcd_clear() {
  //caset
  lcd_write_cmd(0x2A);
  lcd_write_data(0);
  lcd_write_data(0);
  lcd_write_data((MAXWIDTH - 1) >> 8);
  lcd_write_data((MAXWIDTH - 1) & 0xFF);

  //paset
  lcd_write_cmd(0x2B);
  lcd_write_data(0);
  lcd_write_data(0);
  lcd_write_data((MAXLENGTH - 1) >> 8);
  lcd_write_data((MAXLENGTH - 1) & 0xFF);

  //memory write
  lcd_write_cmd(0x2C);
  for (uint32_t i = 0; i < static_cast<uint32_t>(MAXWIDTH) * static_cast<uint32_t>(MAXLENGTH); i++) {
    lcd_write_data(0);
    lcd_write_data(0);
    lcd_write_data(0);
  }
}

void color_change_gradient(color_code *p) {
  if (p->b > 0 && p->r == 63)
    p->b--;
  else if (p->g < 63 && p->r == 63)
    p->g++;
  else if (p->r > 0 && p->b == 0)
    p->r--;
  else if (p->b < 63)
    p->b++;
  else if (p->g > 0)
    p->g--;
  else if (p->r < 63)
    p->r++;
}

void lcd_set_pixel(pt_coord p, color_code c) {
  //caset
  lcd_write_cmd(0x2A);
  lcd_write_data(p.x >> 8);
  lcd_write_data(p.x);
  lcd_write_data(p.x >> 8);
  lcd_write_data(p.x);

  //paset
  lcd_write_cmd(0x2B);
  lcd_write_data(p.y >> 8);
  lcd_write_data(p.y);
  lcd_write_data(p.y >> 8);
  lcd_write_data(p.y);

  //memory write
  lcd_write_cmd(0x2C);
  lcd_write_data(c.b << 2);
  lcd_write_data(c.g << 2);
  lcd_write_data(c.r << 2);
}

void graphics_draw_line(pt_coord a_pt, pt_coord b_pt, color_code c) {
  ln_coord a = {static_cast<int>(a_pt.x), static_cast<int>(a_pt.y)};
  ln_coord b = {static_cast<int>(b_pt.x), static_cast<int>(b_pt.y)};

  //determine octant
  uint8_t oct = line_find_octant(&a, &b);
  
  
  //convert to zero octant via pointers
  line_Zoctant_convert(oct, &a, &b);

  //use zero octant
  float d_x = b.x - a.x;
  float d_y = b.y - a.y;
  float d_err = abs(d_y / d_x);
  float err = d_err - 0.5;
   
  for (; a.x <= b.x; a.x++) {
    //reconvert to original octant for set pixel
    lcd_set_pixel(line_Zoctant_switchfrom(oct, a), c);

    err = err + d_err;
    if (err > 0.5) {
      a.y++;
      err--;
    }
  }  
}

uint8_t line_find_octant(ln_coord *a, ln_coord *b) {
  float slp;
  
  if (b->x - a->x) 
    slp = static_cast<float>(b->y - a->y) / static_cast<float>(b->x - a->x);
  else {
    if (b->y - a->y > 0) {
      return 1; 
    }
    else
      return 6;
  }
  
  if (a->x < b->x) {
    if  (a->y < b->y) {
      if (slp < 1) {
        return 0;
      }
      else {
        return 1;
      }
    }
    else {
      if (slp > -1) {
        return 7;
      }
      else {
        return 6;
      }
    }
  }
  else {
    if  (a->y < b->y) {
      if (slp > -1) {
        return 3;
      }
      else {
        return 2;
      }
    }
    else {
      if (slp < 1) {
        return 4;
      }
      else {
        return 5;
      }
    }
  }
}

void line_Zoctant_convert(uint8_t oct, ln_coord *a, ln_coord *b) {
  int t;
  switch(oct) {
    case 0:
      break;
    case 1:
      t = a->x;
      a->x = a->y;
      a->y = t;
      t = b->x;
      b->x = b->y;
      b->y = t;
      break;
    case 2:
      t = -a->x;
      a->x = a->y;
      a->y = t;
      t = -b->x;
      b->x = b->y;
      b->y = t;
      break;
    case 3:
      a->x = -a->x;
      b->x = -b->x;
      break;
    case 4:
      a->x = -a->x;
      a->y = -a->y;
      b->x = -b->x;
      b->y = -b->y;
      break;
    case 5:
      t = -(a->x);
      a->x = -(a->y);
      a->y = t;
      t = -(b->x);
      b->x = -(b->y);
      b->y = t;
      break;
    case 6:
      t = a->x;
      a->x = -a->y;
      a->y = t;
      t = b->x;
      b->x = -b->y;
      b->y = t;
      break;
    case 7:
      a->y = -a->y;
      b->y = -b->y;
      break; 
  }
}

pt_coord line_Zoctant_switchfrom(uint8_t oct, ln_coord a) {
  pt_coord pt_a;
  int t;
  switch(oct) {
    case 0:
      break;
    case 1:
      t = a.x;
      a.x = a.y;
      a.y = t;
      break;
    case 2:
      t = a.x;
      a.x = -a.y;
      a.y = t;
      break;
    case 3:
      a.x = -a.x;
      break;
    case 4:
      a.x = -a.x;
      a.y = -a.y;
      break;
    case 5:
      t = -a.x;
      a.x = -a.y;
      a.y = t;
      break;
    case 6:
      t = -a.x;
      a.x = a.y;
      a.y = t;
      break;
    case 7:
      a.y = -a.y;
      break;
  }
  pt_a = {static_cast<uint16_t>(a.x), static_cast<uint16_t>(a.y)};
  return pt_a;
}

void graphics_draw_rect(pt_coord a, uint16_t w, uint16_t l, color_code c) {
    for (uint16_t j = 0; j < l; j++) {
      for (uint16_t i = 0; i < w; i++) {
        lcd_set_pixel(a, c);
        a.x++;
      }
      a.x = a.x - w;
      a.y++;
    }
}

void graphics_draw_circ(pt_coord m, uint16_t r, color_code c) {
  pt_coord a;
  pt_coord b;
  int16_t x = r - 1;
  int16_t y = 0;  
  int16_t dx = 1;
  int16_t dy = 1;
  int16_t err = dx - (r << 1);

  while (x >= y) {
    //0 and 3 octants
    if (static_cast<int16_t>(m.x) - x < 0) {
      a.x = 0;
    }
    else {
      a.x = static_cast<int16_t>(m.x) - x;
    }
    a.y = static_cast<int16_t>(m.y) + y;
    if (static_cast<int16_t>(m.x) + x >= MAXWIDTH) {
      b.x = MAXWIDTH - 1;
    }
    else {
      b.x = static_cast<int16_t>(m.x) + x;
    }
    b.y = static_cast<int16_t>(m.y) + y;
    graphics_draw_line(a, b, c);

    //4 and 7 octants
    if (static_cast<int16_t>(m.y) - y < 0) {
      a.y = 0;
      b.y = 0;
    }
    else {
      a.y = static_cast<int16_t>(m.y) - y;
      b.y = static_cast<int16_t>(m.y) - y;
    }
    graphics_draw_line(a, b, c);

    //1 and 2 octants
    if (static_cast<int16_t>(m.x) - y < 0)  {
      a.x = 0;
    }
    else {
      a.x = static_cast<int16_t>(m.x) - y;
    }
    a.y = static_cast<int16_t>(m.y) + x;
    if (static_cast<int16_t>(m.x) + y >= MAXWIDTH) {
      b.x = MAXWIDTH - 1;
    }
    else {
      b.x = static_cast<int16_t>(m.x) + y;
    }
    b.y = static_cast<int16_t>(m.y) + x;
    graphics_draw_line(a, b, c);

    // 5 and 6 octants
    if (static_cast<int16_t>(m.y) - x < 0) {
      a.y = 0;
      b.y = 0;
    }
    else {
      a.y = static_cast<int16_t>(m.y) - x;
      b.y = static_cast<int16_t>(m.y) - x;
    }
    graphics_draw_line(a, b, c);

    if (err <= 0) {
      y++;
      err += dy;
      dy +=2;
    }
    if (err > 0) {
      x--;
      dx += 2;
      err += (-r << 1) + dx;
    }
  }
  
}

//writes text in a "box" who's top left corner is pt_coord a
//and bottom right corner is pt_coord b
void graphics_write_text(char *x, pt_coord a, pt_coord b, color_code c) {
  uint16_t start_pos = a.y;
  uint16_t str_ind = 0;
  char t;
  while (x[str_ind] != '\0') {
    //check if end of box is reached
    if (a.y >= b.y) {
      if (a.x >= b.x) {
        return;
      }
      a.x += TEXT_HEIGHT;
      a.y = start_pos;
    }
    
    //line feed
    if (x[str_ind] == '\n') {
      a.x += TEXT_HEIGHT;
      a.y = start_pos;
      str_ind++;
      continue;
    }
    
    //copy character from string and change to font index
    t = x[str_ind] & 0x7F;
    if (t < ' ')
      t = 0;
    else
      t -= ' ';

    //copy font bitmap
    const uint8_t *ch = font[t];

    //set pixels while incrementing through pixels
    for (uint8_t j = 0; j < TEXT_WIDTH; j++) {
      for (uint8_t i = 0; i < TEXT_HEIGHT; i++) {
        if (ch[j] & (1 << i))
          lcd_set_pixel(a, c);
        a.x++;
      }
      a.y++;
      a.x -= TEXT_HEIGHT;
    }
    str_ind++;
  }
}

void loop() {

/*
  pt_coord pixel;
  static color_code gradient = {63, 0, 31};

  for (int j = 0; j < MAXLENGTH; j++) {
    pixel.y = j;
    for (int i = 0; i < MAXWIDTH; i++) {
      pixel.x = i;
      lcd_set_pixel(pixel, gradient);
    }
    color_change_gradient(&gradient);
  }
*/
  pt_coord a = {10, 10};
  pt_coord b = {200, 280};
  pt_coord c = {100,10};
  pt_coord d = {120,10};
  pt_coord e = {120,300};
  pt_coord box_a = {50, 10};
  pt_coord box_b = {100, 150};

  char test_str[] = "This is just a test string to see if the textbox\n drawing function is properly\0 functioning.";

  //graphics_draw_line(a, b, green);

  graphics_draw_circ(b, 60, green);

  graphics_draw_rect(c, 20, 40, red);

  for (int i = 0; i < 10; i++) {
    graphics_draw_line(d, e, blue);
    e.x += 10;
  }
  
  graphics_write_text(test_str,box_a, box_b, white);

  delay(10000);

  

}
