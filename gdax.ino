/*
sample coordinates
pt_coord a = {10, 10};
pt_coord b = {200, 280};
pt_coord c = {100,10};
pt_coord d = {120,10};
pt_coord e = {120,300};
pt_coord box_a = {50, 10};
pt_coord box_b = {100, 150};
*/

void setup() {
  setup_PixWrt();

  //arduino powered up first
  //establish connection through serial to python
  Serial.begin(9600);
  //wait for python to respond
  while (!Serial) {
  }
  Serial.println("Waiting for input...");
}

void loop() {
  char buffer_str[65];
  
  while(!Serial.available()); //wait for non-empty buffer
  empty_buffer(buffer_str);
  parse_data(buffer_str);
}

void empty_buffer(char *buffer_str) {
  char t = 1;
  for (int i = 0; i < 64 && t; i++) {
    t = Serial.read();
    buffer_str[i] = t;
    delay(2);
  }
}

void parse_data(char *buffer_str) {
  char* str_index;
  char pair[8];
  char price[10];
  char time_pst[6];
  char date[11];
  
  str_index = strtok(buffer_str, ",");
  strcpy(pair,str_index);
  graphics_write_text(pair, {10, 10}, {11, 200}, white, black, 2);
  
  str_index = strtok(NULL, ",");
  strcpy(price, str_index);
  graphics_write_text(price, {28, 10}, {29, 200}, red, black, 3);
  
  str_index = strtok(NULL, ",");
  strcpy(time_pst, str_index);
  graphics_write_text(time_pst, {60, 10}, {61, 200}, blue, black, 1);
  
  str_index = strtok(NULL, ",");
  strcpy(date, str_index);
  graphics_write_text(date, {70, 10}, {71, 200}, green, black, 1);  
}
