#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TM1637TinyDisplay.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define UP_BUTTON_PIN 9
#define DOWN_BUTTON_PIN 10
#define SELECT_BUTTON_PIN 11
#define LED_CAN_1 3
#define LED_CAN_2 4
#define LED_CAN_3 5
#define POT_PIN A0
#define BUZZER_PIN 12
#define BRICK_WIDTH 12
#define BRICK_HEIGHT 8
#define BRICK_GAP 2
#define BRICK_ROWS 3
#define BRICK_COLS 6
#define CLK_PIN 2
#define DIO_PIN 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TM1637TinyDisplay tm1637(CLK_PIN, DIO_PIN);

int brick[BRICK_ROWS][BRICK_COLS];
int canHakki = 3;
int gemiX = 0;
int menuSecimi = 0;
bool oyunBasladi = false;
boolean levelStarted = false;
int upButtonState;
int downButtonState;
int selectButtonState;
const int numOptions = 2;
const char *menuOptions[numOptions] = {"START", "QUIT"};
int currentSelection = 0;
int score = 0;
int lvlScore = 0;
float topX = SCREEN_WIDTH / 2;
float topY = SCREEN_HEIGHT - 8;
int topSpeedX = 1;
float topSpeedY = -1;
int topSize = 3;
int brickCount;
int lvlNum = 0;
bool canActive = false;
int canX;
int canY;
const int CAN_RADIUS = 2;
const int CAN_SPEED = 2;
const int photoresistorPin = A1;
int photoresistorValue;
uint16_t backgroundColor;
uint16_t ballColor;
uint16_t brickColor;
uint16_t paddleColor;
uint16_t canColor;

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Menu:");
  display.setTextSize(1);
  display.println("-------------------");

  for (int i = 0; i < numOptions; i++) {
    if (i == currentSelection) {
      display.print("-> ");
    } else {
      display.print("   ");
    }
    display.print(menuOptions[i]);
    display.println();
  }

  display.println("-------------------");
  display.display();
}

void setup() {
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  tm1637.setBrightness(0x0f);
  pinMode(photoresistorPin, INPUT);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SELECT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT);
  pinMode(LED_CAN_1, OUTPUT);
  pinMode(LED_CAN_2, OUTPUT);
  pinMode(LED_CAN_3, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_CAN_1, HIGH);
  digitalWrite(LED_CAN_2, HIGH);
  digitalWrite(LED_CAN_3, HIGH);
}

void clearBricks() {
  for (int i = 0; i < BRICK_ROWS; i++) {
    for (int j = 0; j < BRICK_COLS; j++) {
      brick[i][j] = 0;
    }
  }
}

void generateLevel() {
  brickCount = 0;
  for (int i = 0; i < BRICK_ROWS; i++) {
    for (int j = 0; j < BRICK_COLS; j++) {
      brick[i][j] = random(0, 4);
      if (brick[i][j] == 1) {
        brickCount++;
      }
    }
  }
}

void drawBricks() {
  for (int i = 0; i < BRICK_ROWS; i++) {
    for (int j = 0; j < BRICK_COLS; j++) {
      if (brick[i][j] == 1) {
        int x = j * (BRICK_WIDTH + BRICK_GAP);
        int y = i * (BRICK_HEIGHT + BRICK_GAP);
        display.fillRect(x, y, BRICK_WIDTH, BRICK_HEIGHT, brickColor);
      }
    }
  }
}

void checkGameOver() {
  if (canHakki <= 0) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("GAME OVER!");
    display.print("Score: ");
    display.println(score);
    display.display();
    delay(4000);
    resetGame();
  }
}

void createNewCan(int col, int row) {
  canActive = true;
  canX = col * (BRICK_WIDTH + BRICK_GAP) + BRICK_WIDTH / 2;
  canY = row * (BRICK_HEIGHT + BRICK_GAP) + BRICK_HEIGHT / 2;
}

void moveCan() {

  if (canActive) {
    display.fillCircle(canX, canY, CAN_RADIUS, ~canColor);
    canY += CAN_SPEED;

    if (canY + CAN_RADIUS >= SCREEN_HEIGHT - 8 && canX >= gemiX && canX <= gemiX + BRICK_WIDTH) {
      canActive = false;
      canY = 0;
      canHakki++;

      if (canHakki > 3)
        canHakki = 3;

      if (canHakki <= 3) {

        if (canHakki == 2) {
          digitalWrite(LED_CAN_1, LOW);
          digitalWrite(LED_CAN_2, HIGH);
          digitalWrite(LED_CAN_3, HIGH);
        } else if (canHakki == 1) {
          digitalWrite(LED_CAN_1, LOW);
          digitalWrite(LED_CAN_2, LOW);
          digitalWrite(LED_CAN_3, HIGH);
        } else if (canHakki == 0) {
          digitalWrite(LED_CAN_1, LOW);
          digitalWrite(LED_CAN_2, LOW);
          digitalWrite(LED_CAN_3, LOW);
        }
        else {
          digitalWrite(LED_CAN_1, HIGH);
          digitalWrite(LED_CAN_2, HIGH);
          digitalWrite(LED_CAN_3, HIGH);
        }

      }
    }
    drawCan();
  }
}

void drawCan() {
  if (canActive) {
    display.fillCircle(canX, canY, CAN_RADIUS, canColor);
  }
}

void resetGame() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Returning to Main Menu...");
  display.display();
  delay(2000);
  clearBricks();
  generateLevel();
  canHakki = 3;
  lvlNum = 0;
  score = 0;
  oyunBasladi = false;
  updateDisplay();
}

void loop() {
  upButtonState = digitalRead(UP_BUTTON_PIN);
  downButtonState = digitalRead(DOWN_BUTTON_PIN);
  selectButtonState = digitalRead(SELECT_BUTTON_PIN);
  photoresistorValue = analogRead(photoresistorPin);

  backgroundColor = photoresistorValue > 500 ? WHITE : BLACK;
  ballColor = photoresistorValue > 500 ? BLACK : WHITE;
  brickColor = photoresistorValue > 500 ? BLACK : WHITE;
  paddleColor = photoresistorValue > 500 ? BLACK : WHITE;
  canColor = photoresistorValue > 500 ? BLACK : WHITE;

  if (upButtonState == LOW) {
    currentSelection--;

    if (currentSelection < 0) {
      currentSelection = numOptions - 1;
    }

    updateDisplay();
    delay(200);
  }

  if (downButtonState == LOW) {
    currentSelection++;

    if (currentSelection >= numOptions) {
      currentSelection = 0;
    }

    updateDisplay();
    delay(200);
  }

  if (selectButtonState == LOW) {
    if (currentSelection == 0) {
      oyunBasladi = true;
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Selected:");
      display.println(menuOptions[currentSelection]);
      display.display();
      delay(3000);
      display.clearDisplay();
      updateDisplay();
    } else if (currentSelection == 1) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Oyunumuza");
      display.println("gosterdiginiz");
      display.println("ilgi icin");
      display.println("tesekkurler");
      display.display();
      delay(5000);
      display.clearDisplay();
      display.ssd1306_command(SSD1306_DISPLAYOFF);
      return;
    }

    if (oyunBasladi) {
      gemiX = map(analogRead(POT_PIN), 0, 1023, 0, SCREEN_WIDTH - 8);
      display.clearDisplay();
      display.fillScreen(backgroundColor);
      display.fillRect(gemiX, 62, BRICK_WIDTH, 2, paddleColor);

      topX += topSpeedX;
      topY += topSpeedY;

      if (topX <= 0 || topX >= SCREEN_WIDTH - topSize) {
        topSpeedX *= -1;
      }

      if (topY <= 0) {
        topSpeedY *= -1;
      }

      if (topY >= SCREEN_HEIGHT - 8 && topX >= gemiX && topX <= gemiX + BRICK_WIDTH) {
        topSpeedY *= -1;
      }

      if (topY >= SCREEN_HEIGHT) {
        delay(300);
        topX = gemiX + 6;
        topY = SCREEN_HEIGHT - 10;
        topSpeedY *= -1;
        canHakki--;
        checkGameOver();

        if (canHakki > 3) {
          canHakki = 3;
        }

        if (canHakki == 2) {
          tone(BUZZER_PIN, 500, 100);
          digitalWrite(LED_CAN_1, LOW);
          digitalWrite(LED_CAN_2, HIGH);
          digitalWrite(LED_CAN_3, HIGH);
        } else if (canHakki == 1) {
          tone(BUZZER_PIN, 500, 100);
          digitalWrite(LED_CAN_1, LOW);
          digitalWrite(LED_CAN_2, LOW);
          digitalWrite(LED_CAN_3, HIGH);
        } else if (canHakki == 0) {
          tone(BUZZER_PIN, 500, 100);
          digitalWrite(LED_CAN_1, LOW);
          digitalWrite(LED_CAN_2, LOW);
          digitalWrite(LED_CAN_3, LOW);
        }

      }

      int hitBrickRow = -1;
      int hitBrickCol = -1;

      for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
          if (brick[i][j] == 1 &&
              topX >= j * (BRICK_WIDTH + BRICK_GAP) && topX <= j * (BRICK_WIDTH + BRICK_GAP) + BRICK_WIDTH &&
              topY >= i * (BRICK_HEIGHT + BRICK_GAP) && topY <= i * (BRICK_HEIGHT + BRICK_GAP) + BRICK_HEIGHT) {
            hitBrickRow = i;
            hitBrickCol = j;
            break;
          }
        }

        if (hitBrickRow != -1) {
          break;
        }

      }

      if (hitBrickRow != -1 && hitBrickCol != -1) {
        brick[hitBrickRow][hitBrickCol] = 0;
        topSpeedY *= -1;
        score += 1;
        lvlScore += 1;
        tm1637.showNumberDec(score, true);

        if (random(0, 6) == 0) {
          createNewCan(hitBrickCol, hitBrickRow);
        }
      }

      moveCan();

      if (lvlScore == brickCount) {
        lvlScore = 0;
        lvlNum++;
        display.clearDisplay();
        display.setCursor(40, 28);
        display.print("L E V E L ");
        display.print(lvlNum);
        display.display();
        delay(3000);
        clearBricks();
        generateLevel();
        topX = gemiX + 6;
        topY = SCREEN_HEIGHT - 10;
        topSpeedY = -(topSpeedY + topSpeedY * 0.2) ;
        updateDisplay();
      }

      if (canHakki > 0) {
        display.fillCircle(topX, topY, topSize, ballColor);
        drawBricks();
        display.display();
      }

    }
  }
}