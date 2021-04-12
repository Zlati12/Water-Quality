#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define BASE 1.87
#define offset 0
#define ENOUGHT 30

//#define DEBUG

byte bar_1_up[8] = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000
};

byte bar_2_up[8] = {
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b11000
};


byte bar_3_up[8] = {
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100,
  0b11100
};

byte bar_4_up[8] = {
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110,
  0b11110
};
byte bar_1_down[8] = {
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b00001
};

int avrg[ENOUGHT];

void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT);
  pinMode(6, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, bar_1_up);
  lcd.createChar(1, bar_2_up);
  lcd.createChar(2, bar_3_up);
  lcd.createChar(3, bar_4_up);
  lcd.createChar(4, bar_1_down);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water quality  ");
  lcd.setCursor(0, 1);
  lcd.print("meter v.1.0");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press button 1");
  lcd.setCursor(0, 1);
  lcd.print("to measure ..");

}

void loop() {
  if (digitalRead(8) == HIGH)
  {
    while (1)
    {
      while (digitalRead(8) == HIGH);
      float tmp;
      tmp = get_temperature();
      //////////////////////////////////////////////
      float phvle;
      phvle = get_ph();
      //////////////////////////////////////////////
      float tds;
      tds = get_conductivity(tmp);
      /////////////////////////////////////////////
      float ntv;
      ntv = get_leght();
      /////////////////////////////////////////////
      // Serial.print("  tmp: ");
      // Serial.print(tmp);
      while (1)
      {
        out_ph(phvle);
        delay(500);
        if (read_buttons() == 2)break;
        out_tds(tds, tmp);
        delay(500);
        if (read_buttons() == 2)break;
        out_ntv(ntv);
        delay(500);
        if (read_buttons() == 2)break;
        delay(500);
      }
    }
  }
}
/////////////////////////////////////////////////
void out_ph(float ph) {
#ifdef DEBUG
  Serial.print("  ph:");
  Serial.println(ph);
#endif
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ph:");
  lcd.print(ph);
  if(ph>14)ph=14;
  scroll_bar_s(ph);
}
void out_tds(float tds , float tmp) {
#ifdef DEBUG
  Serial.print("   tds:");
  Serial.println(tds);
  Serial.print("   tmp:");
  Serial.println(tmp);
#endif
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cond.:");
  lcd.print(tds);
  scr_per(tds, 500);
}
void out_ntv(float ntv ) {
#ifdef DEBUG
  Serial.print("  ntv:");
  Serial.println(ntv);
#endif
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turb.:");
  lcd.print(ntv);
  scr_per(ntv, 3000);
}

void scroll_bar_n(int num)
{
  int i;
  lcd.setCursor(0, 1);
  for (int j = 0; j < 17; j++)
  {
    if (num > 5)
    {
      lcd.write(byte(255));
      num = num - 5;
    }
    else
    {
      if (num == 0)break;
      num=num-2;
      lcd.write(byte(num));
      break;
    }
  }
}


int scr_per(float num, int mx)
{
  float x;
  x = 100 * num / mx;
  x = x * 0.8;
  scroll_bar_n(x);
}

void scroll_bar_s(int num)
{
  int pos;
  pos = 7;
  lcd.setCursor(0, 1);
  lcd.print("{");
  lcd.setCursor(15, 1);
  lcd.print("}");
  if (num == 7)
  {
    lcd.setCursor(7, 1);
    lcd.write(byte(4));
    lcd.setCursor(8, 1);
    lcd.write(byte(0));
    return;
  }
  if (num > 7)
  {
    num = num - 7;
    lcd.setCursor(8, 1);
    while (num > 0)
    {
      lcd.write(byte(255));
      num--;
    }
    return;
  }
  num = 7 - num;
  lcd.setCursor(8, 1);
  while (num > 0)
  {
    lcd.setCursor(pos, 1);
    lcd.write(byte(255));
    num--;
    pos--;
  }
  return;
}

int read_buttons()
{
  while (1)
  {
    if (digitalRead(7) == HIGH)return 1;
    if (digitalRead(8) == HIGH)return 2;
    delay(100);
  }
}

float avrg_func(int port)
{
  float V;
  for (int i = 0; i < ENOUGHT; i++)
  {
    V = analogRead(port);
    avrg[i] = V;
  }
  float avr_V;
  avr_V = 0;
  for (int i = 0; i < ENOUGHT; i++)
  {
    avr_V = avr_V + avrg[i];
  }
  avr_V = avr_V / ENOUGHT;
  return avr_V;
}


float avrg_volt(int port)
{
  float v;
  v = avrg_func(port);
  return v = v * (5.0 / 1024);

}
float get_temperature() {
  float TV = avrg_func(A0);
  float tmp , atv , btv , ctv;
  atv = 0.000277;
  btv = -0.114;
  ctv = 0.346;
  tmp = (atv * TV + btv) * TV + ctv;
  return tmp;
}

float get_ph() {
  float PHvoltage = avrg_volt(A3);
  float phvle;
  PHvoltage = PHvoltage - BASE;
  phvle = ( PHvoltage / 0.059 ) + 7;
  return phvle;
}
float get_conductivity(float tmp) {
  float C_tds;
  C_tds = 1 + 0.02 * (tmp - 25);
  float V_tds;
  V_tds = avrg_volt(A1);
  V_tds = V_tds / C_tds;
  float Y_tds;
  Y_tds = (133.42 * V_tds * V_tds * V_tds) - (255.86 * V_tds * V_tds) + (857.39 * V_tds);
  Y_tds = Y_tds / 2;
  return Y_tds;
}
float get_leght() {
  float V_ntv;
  V_ntv = avrg_volt(A2)-0.25;
  float Y_ntv;
  Y_ntv = (-1120.4 * V_ntv + 5742.3) * V_ntv - 4352.9;
  return Y_ntv;
}
