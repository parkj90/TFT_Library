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
  char *str_index;
  
  char pair[8];
  char price[10];
  char time_pst[6];
  char date[11];
  
  //loops until buffer is not empty
  while(!Serial.available());
  //read bytes until null char or 64 byte buffer limit is reached
  char t = 1;
  for (int i = 0; i < 64 && t; i++) {
    t = Serial.read();
    buffer_str[i] = t;
    delay(2);
  }

  //parse data
  str_index = strtok(buffer_str, ",");
  strcpy(pair,str_index);
  graphics_write_text(pair, {10, 10}, {11,100}, white, black);
  
  str_index = strtok(NULL, ",");
  strcpy(price, str_index);
  graphics_write_text(price, {19, 10}, {20,100}, red, black);
  
  str_index = strtok(NULL, ",");
  strcpy(time_pst, str_index);
  graphics_write_text(time_pst, {30, 10}, {31,100}, blue, black);
  
  str_index = strtok(NULL, ",");
  strcpy(date, str_index);
  graphics_write_text(date, {39, 10}, {40,100}, green, black);  
}
