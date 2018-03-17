void setup() {
  setup_PixWrt();
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
