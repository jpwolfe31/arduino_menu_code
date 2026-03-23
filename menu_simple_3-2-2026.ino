#include <Wire.h>

// #include <LiquidCrystal_I2C.h>  // if lcd
// ===== LCD CONFIGURATION =====
//#define LCD_ADDR 0x27
//#define LCD_COLS 20
//#define LCD_ROWS 6   // Change to match your LCD height (6 for 20x6)
// LCD object
//LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

#include <Adafruit_GFX.h> // if oled
#include <Adafruit_SSD1306.h>
#define CUSTOM_SDA D3
#define CUSTOM_SCL D2
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels  was 32!!
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
// even though the display has no reset pin, it must be initialized 
//   as follows for the library to work - 4 is the reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DISPLAY_ROWS 6   // Change to match number of rows
int displayActiveFlag = 1;  // used to blank screen when not in use
char oled_line[6][32];  // strings for oled lines
char oled_text_str[32]; // string for manipulations, if needed

// Button pins
#define BTN_UP     A1
#define BTN_DOWN   A0
#define BTN_BACK   A2
#define BTN_SELECT A3

// ===== DEBOUNCE =====
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 250;

// ===== MENU STRUCTURE =====
// strucutres must be listed first to compile
struct MenuItem { 
  const char* name;
  MenuItem* parent;  // parent is populated dynamically by the menu system.  Always starts as nullptr
  MenuItem* children;
  int childCount;
  void (*action)();
};

// ===== ACTIONS =====
// must be above menu items to compile
void actionStart() { display.clearDisplay(); strcpy(oled_line[0], "Starting..."); updateMenuDisplay(1); delay(1000); }
void actionBrightnessHigh() { display.clearDisplay(); strcpy(oled_line[0], "Brightness High"); updateMenuDisplay(1); delay(1000); }
void actionBrightnessLow() { display.clearDisplay(); strcpy(oled_line[0], "Brightness Low"); updateMenuDisplay(1); delay(1000); }
void actionVolume() { display.clearDisplay(); strcpy(oled_line[0], "Volume set"); updateMenuDisplay(1); delay(1000); }
void actionVersion() { display.clearDisplay(); strcpy(oled_line[0], "Version 1.0"); updateMenuDisplay(1); delay(1000); }
void actionAuthor() { display.clearDisplay(); strcpy(oled_line[0], "By ArduinoDev"); updateMenuDisplay(1); delay(1000); }

// ===== MENU ITEMS =====
// lowest menu levels must be listed first to compile as they need to be known to higher menu levels

MenuItem brightnessSetL3[] = { // 3rd level down
  {"High", nullptr, nullptr, 0, actionBrightnessHigh},
  {"Low",  nullptr, nullptr, 0, actionBrightnessLow}
};

MenuItem settingsL2[] = { // 2nd level down
  {"Brightness", nullptr, brightnessSetL3, 2, nullptr},
  {"Volume", nullptr, nullptr, 0, actionVolume}
};

MenuItem infoL2[] = { // 2nd level down
  {"Version", nullptr, nullptr, 0, actionVersion},
  {"Author", nullptr, nullptr, 0, actionAuthor}
};

MenuItem MenuL1[] = {  // 1st level down
  {"Settings", nullptr, settingsL2, 2, nullptr},
  {"Info", nullptr, infoL2, 2, nullptr},
  {"Start", nullptr, nullptr, 0, actionStart} //,  // for testing
//{"Settings1", nullptr, settingsL2, 2, nullptr},  
//  {"Info1", nullptr, infoL2, 2, nullptr},
//  {"Start1", nullptr, nullptr, 0, actionStart},
//{"Settings2", nullptr, settingsL2, 2, nullptr},
//  {"Info2", nullptr, infoL2, 2, nullptr},
//  {"Start2", nullptr, nullptr, 0, actionStart}
};

// Root menu - not displayed
MenuItem rootMenu = {"Root", nullptr, MenuL1, 3, nullptr};  // 3 to 9 for testing

// ===== NAVIGATION STATE =====
MenuItem* currentMenu = &rootMenu;
int cursorPos = 0;       // Position in menu
int topIndex = 0;        // First visible item index


// ===== MENU FUNCTIONS =====
void displayMenu() {
  //lcd.clear();
  display.clearDisplay();
  if (currentMenu->childCount == 0) return;
  for (int i = 0; i < DISPLAY_ROWS; i++) {
    int itemIndex = topIndex + i;
    if (itemIndex >= currentMenu->childCount) {break;}
    //lcd.setCursor(0, i);
    if (itemIndex == cursorPos) {
      strcpy(oled_line[i], "> ");
//      lcd.print(">"); // Cursor
    } else {
      //lcd.print("  ");
      strcpy(oled_line[i], "  ");
    }
    strcat(oled_line[i], currentMenu->children[itemIndex].name);
    if (itemIndex >= currentMenu->childCount) {
      strcpy(oled_line[i], "                 ");  // jw added - fill blank lines
      }
    // lcd.print(currentMenu->children[itemIndex].name);
  }
updateMenuDisplay(currentMenu->childCount);
}

void updateMenuDisplay(int lines)
{
display.clearDisplay();
for (int i = 0; i < 6; i++){ 
  if (i < lines){ // load text lines
    if (i == 0) {oledText(oled_line[0], 8, 3, 1, false);}
    if (i == 1) {oledText(oled_line[1], 8, 13, 1, false);}
    if (i == 2) {oledText(oled_line[2], 8, 23, 1, false);}
    if (i == 3) {oledText(oled_line[3], 8, 33, 1, false);}
    if (i == 4) {oledText(oled_line[4], 8, 43, 1, false);}
    if (i == 5) {oledText(oled_line[5], 8, 53, 1, false);}
    }
  }
oledRect(); // also displays display
}

// oledText(char *text, int x, int y,int size, boolean d)
//   text is the text string to be printed
//   x is the integer x position of text
//   y is the integer y position of text
//   z is the text size - supports sizes from 1 to 8
//   d is either "true" or "false". True draws the display - not used here -display.display used instead
void oledText(char *text, int x, int y, int size, boolean d)
{
display.setTextSize(size);
display.setTextColor(WHITE);
display.setCursor(x,y);
display.println(text);
// display.display();  // draws display by flushing all changes to the display hardware
// we only draw display after the rectangle is drawn below 
// oledDisplay_display();  // includes display.display - use for blanking display
}

// draws oled rectangle around text lines
void oledRect() 
{
display.drawRect(0, 0, 128, 64, WHITE);  // 128 by 64 is maximum - one pixel line
// display.display(); // draws display by flushing all changes to the display hardware
oledDisplay_display();  // includes display.display - use for blanking display
//other OLED display commands
//display.setTextColor(BLACK, WHITE); //reverse
//display.drawPixel(pixelX, pixelY, color) //single pixel
//display.width() //returns value
//display.height()
//display.drawRect(upperLeftX, upperLeftY, width, height, color)
//display.drawLine(startX, startY, endX, endY, color)
//display.fillRect(upperLeftX, upperLeftY, width, height, WHITE)
//display.drawRoundRect(upperLeftX, upperLeftY, width, height, cornerRadius, color)
//display.drawFillRoundRect(upperLeftX, upperLeftY, width, height, cornerRadius, color)
//display.drawCircle(centerX, centerY, radius, color)
//display.fillCircle(centerX, centerY, radius, color)
//display.drawTriangle(poin1X, point1Y, point2X, point2Y, point3X, point3Y, color)
//display.fillTriangle(poin1X, point1Y, point2X, point2Y, point3X, point3Y, color)
//display.write() function to display a single character
//display.drawBitmap(topLeftX, topLeftY, imageData, width, height, color) // for custom graphic
//display.setRotation(2);  // rotate display 180 choices 0, 1 (90), 2 (180) and 3 (270)
// display.ssd1306_command(SSD1306_DISPLAYOFF); // To switch display off
// display.ssd1306_command(SSD1306_DISPLAYON); // To switch display back on
}

// draws oled display
// replacement for display.display to allow for turning display off to preserve oled life
void oledDisplay_display() {  
   if (displayActiveFlag != 1){
     display.clearDisplay(); // clear oled display 
     }
   display.display(); // draws display by flushing all changes to the display hardware
   }


void navigateUp() {
  if (cursorPos == 0) { // added loop back to bottom
    cursorPos = currentMenu->childCount - 1;  // childCount of 3 means positions 0, 1, 2
    topIndex = max(((currentMenu->childCount) - DISPLAY_ROWS), 0); // cannot go negative if DISPLAY_ROWS < 6
    return;
  }
  if (cursorPos > 0) {
    cursorPos--;
    if (cursorPos < topIndex) {
      topIndex--;
    }
  }
}

void navigateDown() {
  if (cursorPos == currentMenu->childCount - 1) { // added loop back to top
    cursorPos = 0;  
    topIndex = 0;
    return;
  }
  if (cursorPos < currentMenu->childCount - 1) {
    cursorPos++;
    if (cursorPos >= topIndex + DISPLAY_ROWS) {
      topIndex++;
    }
  }
}

void selectItem() {
  MenuItem* selected = &currentMenu->children[cursorPos];
  if (selected->childCount > 0) {
    selected->parent = currentMenu;
    currentMenu = selected;
    cursorPos = 0;
    topIndex = 0;
  } else if (selected->action != nullptr) {
    selected->action();
  }
}

void goBack() {
  if (currentMenu->parent != nullptr) {
    currentMenu = currentMenu->parent;
    cursorPos = 0;
    topIndex = 0;
  }
}

// ===== BUTTON HANDLING =====
bool isButtonPressed(int pin) {
  return digitalRead(pin) == LOW;
}

void handleButtonPress() {
  if (millis() - lastDebounceTime < debounceDelay) return;

  if (isButtonPressed(BTN_UP)) {
    navigateUp();
    lastDebounceTime = millis();
  }
  else if (isButtonPressed(BTN_DOWN)) {
    navigateDown();
    lastDebounceTime = millis();
  }
  else if (isButtonPressed(BTN_SELECT)) {
    selectItem();
    lastDebounceTime = millis();
  }
  else if (isButtonPressed(BTN_BACK)) {
    goBack();
    lastDebounceTime = millis();
  }

  displayMenu();
}

// ===== SETUP & LOOP =====
void setup() {
  // start i2c
  //i2c1.begin(D3,D2,100000); // SDA pin D3, SCL pin D2, 400k baud  **** for two wire i2c
  // default std frequency on eps32 s3 is acutally 100k
  Wire.begin(CUSTOM_SDA, CUSTOM_SCL, 100000); // Initializes I2C with custom pins and rate
  // Wire.begin(); default pins are SDA A4, SCL A5
  delay(50);
  
  //lcd.init(); // set up lcd
  //lcd.backlight();

  // set up oled
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
     Serial.println(F("SSD1306 allocation failed"));
    }
  delay(50);
  //else Serial.println(F("SSD1306 allocation completed"));
  display.clearDisplay(); // using this before display.display gets rid of the oled Adafruit logo
  //delay(50);
  //oledRect();
  delay(50);
  display.display(); 
  delay(50);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  displayMenu();
}

void loop() {
  handleButtonPress();
}
