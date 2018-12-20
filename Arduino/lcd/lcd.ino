# include <LiquidCrystal.h>
# include <DHT.h>
# include <String.h>
# include <Math.h>

const int stagePin = 7;

const int DHTPIN = 2;
const int DHTTYPE = DHT11;

const int lcdDataPin[8] = {22, 23, 24, 25, 26, 27, 28, 29};
const int lcdRs = 30;
const int lcdEn = 31;
const int lcdPinOut[8] = {32, 33, 34, 35, 36, 37, 38, 39};
const int lcdRsOut = 40;
const int lcdEnOut = 41;
const int nCol = 16;
const int nRow = 2;

const int relayInt1 = 42;
const int relayInt2 = 43;

const int DCPin1[2] = {3, 4};
const int DCPin2 [2] = {5, 6};

DHT dht (DHTPIN, DHTTYPE);
LiquidCrystal lcd(lcdRsOut, lcdEnOut, lcdPinOut[4],
                  lcdPinOut[5], lcdPinOut[6], lcdPinOut[7]);

byte deree[8] = {
  0b01110,
  0b01010,
  0b01110,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

int stage = 1;
unsigned long time1 = 0;
unsigned long timeSet = 1000;

class sensor {
  private:
    float humidity;
    float temperature;
  public:
    void Init();
    void Output();
};

void sensor::Init() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}

void sensor::Output() {
  unsigned long start = 0;
  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0, 0);
    lcd.print ("Khong the doc duoc du lieu tu cam bien DHT11");
  } else {
    if ((unsigned long) (millis() - start) > timeSet)
    {
      start = millis();
      lcd.setCursor(0, 0);
      lcd.print("TEMPERATURE: ");
      lcd.print(round(temperature));
      lcd.print(" ");
      lcd.write(1);
      lcd.print ("C");
      lcd.setCursor(0, 1);
      lcd.print("HUMIDITY: ");
      lcd.print(round(humidity));
      lcd.print("%");
    }
  }
  if (temperature >= 28) {
    if ((unsigned long) (millis() - time1) > 4 * timeSet)
    {
      start = millis();
      analogWrite(DCPin1[0], 255);
      analogWrite(DCPin1[1], 0);
      digitalWrite(relayInt1, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Bat DC va Relay");
    }
  }
  else if (temperature <= 26) {
    if ((unsigned long) (millis() - time1) > 4 * timeSet)
    {
      start = millis();
      digitalWrite(DCPin1[0], LOW);
      digitalWrite(relayInt1, HIGH);
      Serial.print("Relay 2 chay");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tat DC va Relay");
    }
  }
}

void readStage() {
  if (digitalRead(stagePin) == 1) {
    stage++;
    delay(400);
  }
  if (Serial.read() == "MODE") {
    stage = Serial.read();
  }
  if ((stage > 4) || (stage < 1)) {
    stage = 1;
  }
}

void modeBinaryToChar() {
  char check;
  int translate;
  lcd.setCursor(0, 5);
  lcd.print("Mode 1");
  Serial.print("Mode 1");
  lcd.setCursor(1, 1);
  lcd.print("Binary to char");
  delay(1000);
  lcd.clear();
  lcd.blink();
  Serial.begin(9600);
  while (stage == 1) {
    readStage();
    translate = 0;
    check = "";
    if (digitalRead(lcdEn) == HIGH) {
      for (int i = 0; i < 8; i++) {
        translate += pow(digitalRead(lcdDataPin[i]) * 2, i);
      }
      if (digitalRead(lcdRs) == LOW) {
        switch (translate) {
          case 0:
            lcd.noBlink();
            break;
          case 1:
            lcd.blink();
            break;
          case 2:
            lcd.noDisplay();
            break;
          case 3:
            lcd.display();
            break;
          case 4:
            lcd.scrollDisplayLeft();
            break;
          case 5:
            lcd.scrollDisplayRight();
            break;
          case 6:
            lcd.autoscroll();
            break;
          case 7:
            lcd.noAutoscroll();
            break;
          case 8:
            lcd.rightToLeft();
            break;
          case 9:
            lcd.leftToRight();
            break;
          case 10:
            lcd.home();
            break;
          default:
            lcd.clear();
            break;
        }
      } else {
        check = (char)(translate);
        lcd.print(check);
        delay(200);
      }
    }
    Serial.print(translate);
    Serial.print("  ");
    Serial.print(digitalRead(lcdEn));
    Serial.print(digitalRead(lcdRs));
    Serial.print(digitalRead(stage));
    Serial.print(" : ");
    Serial.print(check);
    Serial.print("\n");
  }
  lcd.clear();
}

int fowardPingPong() {
  int i = 0;
  unsigned long start = millis();
  while (i < 20) {
    readStage();
    if (stage != 2) return (4);
    if ((unsigned long) (millis()-start) > timeSet)
    {
      start = millis();
      Serial.print(i);
      lcd.print("-");
      i++;
      lcd.noBlink();
    }
    if (i < 21) {
      if (digitalRead(lcdDataPin[0]) == 1) return (0);
      else if (digitalRead(lcdDataPin[7]) == 1) return (1);
    }else {
      start = millis();
      if ((unsigned long) (millis()-start) > timeSet) {
        if (digitalRead(lcdDataPin[7]) == 1) return (2);
      }
    }
  }
  Serial.print(i);
  return (0);
}

int backPingPong() {
  for (int i = 0; i < 15; i++) {
    if ((unsigned long) (millis() - time1) > timeSet)
    {
      lcd.setCursor(1, 15 - i);
      lcd.blink();
    }
    if (i < 15) {
      if (digitalRead(lcdDataPin[0]) == 1) return (0);
      else if (digitalRead(lcdDataPin[7]) == 1) return (1);
    }
    if ((unsigned long) (millis() - time1) > timeSet * 15)
    {
      lcd.setCursor(1, 0);
      lcd.blink();
      if (digitalRead(lcdDataPin[0]) == 1) return (2);
    }
    return (1);
  }
}

void pingPongRule() {
  int result = 2;
  while (result == 2) {
    result = fowardPingPong();
    if (result != 2) backPingPong();
    if (result == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Player 2 Win");
      delay(3000);
      lcd.clear();
    }
    else if (result == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Player 1 Win");
      delay(3000);
      lcd.clear();
    }
    timeSet -= 50;
  }
}

void modePingPong() {
  lcd.clear();
  Serial.print("Mode 2");
  lcd.setCursor(0, 5);
  lcd.print("Mode 2");
  lcd.setCursor(1, 1);
  lcd.print("Ping Pong game");
  delay(1000);
  lcd.clear();
  while (stage == 2) {
    lcd.setCursor(0, 7);
    lcd.print("3");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 7);
    lcd.print("2");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 7);
    lcd.print("1");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 7);
    lcd.print("GO");
    delay(100);
    lcd.clear();
    lcd.blink();
    readStage();
    pingPongRule();
  }
  lcd.begin(nCol, nRow);
}

void modeReadWriteDHT() {
  sensor DHTSensor;
  lcd.clear();
  lcd.setCursor(0, 5);
  lcd.print("Mode 3");
  Serial.print("Mode 3");
  lcd.setCursor(1, 1);
  lcd.print("DHT read&ctrl");
  delay(3000);
  lcd.clear();
  while (stage == 3) {
    readStage();
    DHTSensor.Init();
    DHTSensor.Output();
  }
}

void modeControlBLE() {
  char variable;
  int value;
  lcd.clear();
  lcd.setCursor(0, 5);
  lcd.print("Mode 4");
  Serial.print("Mode 4");
  lcd.setCursor(1, 2);
  lcd.print("Ctrl relay&DC");
  delay(1000);
  Serial.print("\n");
  Serial.print(stage);
  lcd.clear();
  while (stage == 4) {
    readStage();
    variable = Serial.read();
    value = Serial.read();
    if (variable == "Relay1") {
      digitalWrite(relayInt1, value);
    }
    if (variable == "Relay2") {
      digitalWrite(relayInt2, value);
    }
    if (variable == "DC1") {
      analogWrite(DCPin1[0], value);
      analogWrite(DCPin1[1], value);
    }
    if (variable == "DC2") {
      analogWrite(DCPin2[0], value);
      analogWrite(DCPin2[1], value);
    }
  }
}

void setup() {
  dht.begin();
  Serial.begin(9600);
  lcd.begin(nCol, nRow);
  lcd.clear();
  lcd.createChar(1, deree);

  pinMode(stagePin, INPUT);

  pinMode(relayInt1, OUTPUT);
  pinMode(relayInt2, OUTPUT);
  digitalWrite(relayInt1, HIGH);
  digitalWrite(relayInt2, HIGH);

  pinMode(DCPin1[0], OUTPUT);
  pinMode(DCPin1[1], OUTPUT);
  pinMode(DCPin2[0], OUTPUT);
  pinMode(DCPin2[1], OUTPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(lcdDataPin[i], INPUT);
  }
  pinMode(lcdEn, INPUT);
  pinMode(lcdRs, INPUT);
}

void loop() {
  readStage();
  switch (stage) {
    case 1:
      modeBinaryToChar();
      break;
    case 2:
      modePingPong();
      break;
    case 3:
      modeReadWriteDHT();
      break;
    case 4:
      modeControlBLE();
      break;
    default:
      modeBinaryToChar();
      break;
  }
}
