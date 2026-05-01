#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <GyverButton.h>
#include <GyverEncoder.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

#define PAUSE_BUTTON 2      
#define START_BUTTON 3
#define ENC_CLK A1          
#define ENC_DT  A2            
#define ENC_SW  A5           
#define TFT_CS   10        
#define TFT_DC   A0        
#define TFT_RST  8        
#define RTC_DAT 6  // IO
#define RTC_CLK 5  // SCLK
#define RTC_RST 7  // CE
#define BUZZER_PIN 9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
GButton pauseBtn(PAUSE_BUTTON, HIGH_PULL);
GButton startBtn(START_BUTTON, HIGH_PULL);
Encoder enc(ENC_CLK, ENC_DT, ENC_SW);
ThreeWire myWire(RTC_DAT, RTC_CLK, RTC_RST); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

enum DeviceState { STATE_TIME_SETUP, STATE_CLOCK, STATE_POMODORO_ACTIVE, STATE_POMODORO_CONFIG };
enum TimeSetupState { SETUP_YEAR, SETUP_MONTH, SETUP_DAY, SETUP_HOURS, SETUP_MINUTES };
DeviceState currentState = STATE_CLOCK; 
TimeSetupState timeSetupState = SETUP_YEAR;

struct PomodoroSettings { int workTime; int shortRestTime; int longRestTime; } settings = {25, 5, 30}; 
struct PomodoroTimer { int sessionNumber; int pomodorosCompleted; bool isWorkSession; unsigned long startTime; unsigned long duration; bool isRunning; unsigned long savedRemainingTime; } timer = {1, 0, true, 0, 0, false, 0};

int inputYear = 2025, inputMonth = 12, inputDay = 21, inputHour = 12, inputMinute = 0;
enum ConfigState { CONFIG_MENU, CONFIG_WORK, CONFIG_S_REST, CONFIG_L_REST };
ConfigState configState = CONFIG_MENU;
int selectedParam = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastTimerUpdate = 0;

int currentYear, currentMonth, currentDay, currentHour, currentMinute;

#define COLOR_BG      ST7735_BLACK
#define COLOR_TEXT    ST7735_WHITE
#define COLOR_ACCENT  0x07E0 
#define COLOR_ALERT   0xF800 
#define COLOR_DIM     0x7BEF 
#define COLOR_BLUE    0x001F 

void displayClock(bool fullRedraw);
void displayPomodoro(bool fullRedraw);
void displayConfig();
void displayTimeSetup();
void startPomodoro();
void pauseTimer();
void resumeTimer();
void checkTimerFinish();
void handleTimeSetup();
void handleClockMode();
void handlePomodoroActiveMode();
void handlePomodoroConfigMode();
unsigned long getRemainingTime();
void goToMainMenu();
int getDaysInMonth(int month, int year);
void updateCurrentTime();
void soundWorkStart();
void soundShortRest();
void soundLongRest();

void setup() {
  Serial.begin(9600);
  tft.initR(INITR_BLACKTAB); 
  tft.setRotation(1); 
  tft.fillScreen(COLOR_BG);
  pinMode(START_BUTTON, INPUT_PULLUP);  
  pauseBtn.setDebounce(80);
  startBtn.setDebounce(80);
  startBtn.setTimeout(1000); 
  pinMode(BUZZER_PIN, OUTPUT);
  enc.setType(TYPE1); 
  Rtc.Begin();
  if (Rtc.GetIsWriteProtected()) {
      Serial.println("RTC was write protected, enabling writing now");
      Rtc.SetIsWriteProtected(false);
  }
  if (!Rtc.GetIsRunning()) {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }
  updateCurrentTime(); 
  bool startBtnPressed = !digitalRead(START_BUTTON); 
  if (currentYear < 2025 || currentYear > 2090 || startBtnPressed) { 
    Serial.println("ENTERING SETUP MODE");
    currentState = STATE_TIME_SETUP;
    timeSetupState = SETUP_YEAR;
    inputYear = 2025; inputMonth = 1; inputDay = 1; inputHour = 12; inputMinute = 0;
  } else {
    Serial.println("CLOCK MODE");
    currentState = STATE_CLOCK;
    displayClock(true);
  }
}
void loop() {
  pauseBtn.tick();
  startBtn.tick();
  enc.tick();
  
  if (enc.isHolded()) { goToMainMenu(); return; }
  

  switch (currentState) {
    case STATE_TIME_SETUP: handleTimeSetup(); break;
    case STATE_CLOCK: handleClockMode(); break;
    case STATE_POMODORO_ACTIVE: handlePomodoroActiveMode(); break;
    case STATE_POMODORO_CONFIG: handlePomodoroConfigMode(); break;
  }
}

void updateCurrentTime() {
    RtcDateTime now = Rtc.GetDateTime();
    currentYear = now.Year();
    currentMonth = now.Month();
    currentDay = now.Day();
    currentHour = now.Hour();
    currentMinute = now.Minute();
}

void goToMainMenu() {
    timer.isRunning = false;
    currentState = STATE_CLOCK;
    tft.fillScreen(COLOR_BG);
    displayClock(true);
    delay(500);
    enc.isClick(); 
}

void handleTimeSetup() {
  bool needUpdate = false;
  int maxDays = getDaysInMonth(inputMonth, inputYear);

  if (enc.isRight()) {
    needUpdate = true;
    switch(timeSetupState) {
        case SETUP_YEAR: inputYear++; if(inputYear > 2030) inputYear = 2025; break;
        case SETUP_MONTH: inputMonth++; if(inputMonth > 12) inputMonth = 1; break;
        case SETUP_DAY: inputDay++; if(inputDay > maxDays) inputDay = 1; break;
        case SETUP_HOURS: inputHour++; if(inputHour > 23) inputHour = 0; break;
        case SETUP_MINUTES: inputMinute++; if(inputMinute > 59) inputMinute = 0; break;
    }
  }
  
  if (enc.isLeft()) {
    needUpdate = true;
    switch(timeSetupState) {
        case SETUP_YEAR: inputYear--; if(inputYear < 2025) inputYear = 2030; break;
        case SETUP_MONTH: inputMonth--; if(inputMonth < 1) inputMonth = 12; break;
        case SETUP_DAY: inputDay--; if(inputDay < 1) inputDay = maxDays; break;
        case SETUP_HOURS: inputHour--; if(inputHour < 0) inputHour = 23; break;
        case SETUP_MINUTES: inputMinute--; if(inputMinute < 0) inputMinute = 59; break;
    }
  }

  maxDays = getDaysInMonth(inputMonth, inputYear);
  if (inputDay > maxDays) inputDay = maxDays;
  
  if (enc.isClick()) {
    tft.fillScreen(COLOR_BG); 
    needUpdate = true;
    
    if (timeSetupState == SETUP_YEAR) timeSetupState = SETUP_MONTH;
    else if (timeSetupState == SETUP_MONTH) timeSetupState = SETUP_DAY;
    else if (timeSetupState == SETUP_DAY) timeSetupState = SETUP_HOURS;
    else if (timeSetupState == SETUP_HOURS) timeSetupState = SETUP_MINUTES;
    else if (timeSetupState == SETUP_MINUTES) {
      
      tft.setCursor(10, 50);
      tft.setTextColor(COLOR_TEXT);
      tft.setTextSize(2);
      tft.print("SAVING...");
    
      RtcDateTime newTime(inputYear, inputMonth, inputDay, inputHour, inputMinute, 0);
      Rtc.SetDateTime(newTime);
      
      delay(500); 
      updateCurrentTime();
      
      bool success = (currentMinute == inputMinute && currentHour == inputHour);
      
      tft.fillScreen(COLOR_BG);
      tft.setCursor(20, 50);
      
      if (success) {
          tft.setTextColor(COLOR_ACCENT);
          tft.print("SUCCESS!");
          delay(1000);
          currentState = STATE_CLOCK;
          tft.fillScreen(COLOR_BG);
          displayClock(true);
          return;
      } else {
          tft.setTextColor(COLOR_ALERT);
          tft.print("ERROR!");
          tft.setCursor(20, 80);
          tft.setTextSize(1);
          tft.setTextColor(COLOR_TEXT);
          tft.print("CHECK WIRING");
          delay(2000);
          timeSetupState = SETUP_YEAR;
          tft.fillScreen(COLOR_BG);
      }
    }
  }

  if (needUpdate || millis() - lastDisplayUpdate > 200) {
    lastDisplayUpdate = millis();
    displayTimeSetup();
  }
}

int getDaysInMonth(int month, int year) {
  if (month == 2) return (year % 4 == 0) ? 29 : 28;
  if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
  return 31;
}

void displayTimeSetup() {
    tft.setTextSize(2);
    tft.setTextColor(COLOR_TEXT, COLOR_BG); 
    tft.setCursor(15, 10);
    tft.print("SETUP DATE:");
    
    tft.setTextSize(2);
    tft.setCursor(20, 40); 
    
    if (timeSetupState == SETUP_YEAR) tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    else tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.print(inputYear);
    
    tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.print("-");
    
    if (timeSetupState == SETUP_MONTH) tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    else tft.setTextColor(COLOR_DIM, COLOR_BG);
    if(inputMonth<10) tft.print("0"); tft.print(inputMonth);
    
    tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.print("-");
    
    if (timeSetupState == SETUP_DAY) tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    else tft.setTextColor(COLOR_DIM, COLOR_BG);
    if(inputDay<10) tft.print("0"); tft.print(inputDay);
    
    tft.setTextSize(3);
    tft.setCursor(35, 75);
    
    if (timeSetupState == SETUP_HOURS) tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    else tft.setTextColor(COLOR_DIM, COLOR_BG);
    if(inputHour<10) tft.print("0"); tft.print(inputHour);
    
    tft.setTextColor(COLOR_TEXT, COLOR_BG); tft.print(":");
    
    if (timeSetupState == SETUP_MINUTES) tft.setTextColor(COLOR_ACCENT, COLOR_BG);
    else tft.setTextColor(COLOR_DIM, COLOR_BG);
    if(inputMinute<10) tft.print("0"); tft.print(inputMinute);
}

void handleClockMode() {
  if (millis() - lastDisplayUpdate > 1000) {
    lastDisplayUpdate = millis();
    displayClock(false); 
  }
  
  if (startBtn.isSingle()) {
    startPomodoro();
  }

   if (enc.isClick()) {
    currentState = STATE_POMODORO_CONFIG;
    configState = CONFIG_MENU;
    selectedParam = 0;

    tft.fillScreen(COLOR_BG);
    displayConfig();
   }  
}

void handlePomodoroActiveMode() {
  if (timer.isRunning) {
     if (millis() - lastTimerUpdate > 200) { 
       lastTimerUpdate = millis();
       checkTimerFinish();
       displayPomodoro(false);
     }
  } else {
     if (millis() - lastDisplayUpdate > 500) {
        lastDisplayUpdate = millis();
        displayPomodoro(false);
     }
  }
  
  if (enc.isRight() || enc.isLeft() || enc.isClick()) {
    pauseTimer(); 
    currentState = STATE_POMODORO_CONFIG;
    configState = CONFIG_MENU;
    selectedParam = 0;
    tft.fillScreen(COLOR_BG);
    displayConfig();
  }
  
  if (pauseBtn.isSingle()) {
    if (timer.isRunning) pauseTimer();
    else resumeTimer();
    displayPomodoro(false);
  }
  
  if (startBtn.isHolded()) goToMainMenu();
}

void handlePomodoroConfigMode() {
  bool needUpdate = false;
  
  if (configState == CONFIG_MENU) {
    if (enc.isRight()) { selectedParam++; if (selectedParam > 3) selectedParam = 0; needUpdate = true; }
    if (enc.isLeft()) { selectedParam--; if (selectedParam < 0) selectedParam = 3; needUpdate = true; }
  } 
  else if (configState == CONFIG_WORK) {
    if (enc.isRight()) { settings.workTime++; needUpdate = true; }
    if (enc.isLeft()) { settings.workTime--; needUpdate = true; }
    settings.workTime = constrain(settings.workTime, 1, 60);
  }
  else if (configState == CONFIG_S_REST) {
    if (enc.isRight()) { settings.shortRestTime++; needUpdate = true; }
    if (enc.isLeft()) { settings.shortRestTime--; needUpdate = true; }
    settings.shortRestTime = constrain(settings.shortRestTime, 1, 20);
  }
  else if (configState == CONFIG_L_REST) {
    if (enc.isRight()) { settings.longRestTime++; needUpdate = true; }
    if (enc.isLeft()) { settings.longRestTime--; needUpdate = true; }
    settings.longRestTime = constrain(settings.longRestTime, 1, 60);
  }
  
  if (enc.isClick()) {
    needUpdate = true;
    tft.fillScreen(COLOR_BG);
    if (configState == CONFIG_MENU) {
      if (selectedParam == 0) configState = CONFIG_WORK;
      else if (selectedParam == 1) configState = CONFIG_S_REST;
      else if (selectedParam == 2) configState = CONFIG_L_REST;
      else if (selectedParam == 3) { startPomodoro(); return; }
    } else configState = CONFIG_MENU;
  }
  if (needUpdate) displayConfig();
}

void startPomodoro() {
  timer.sessionNumber = 1;
  timer.pomodorosCompleted = 0;
  timer.isWorkSession = true;
  timer.duration = (long)settings.workTime * 60000;
  timer.startTime = millis();
  timer.isRunning = true;
  currentState = STATE_POMODORO_ACTIVE;
  soundWorkStart();
  tft.fillScreen(COLOR_BG);
  displayPomodoro(true);
}

void pauseTimer() {
  if (!timer.isRunning) return;
  timer.isRunning = false;
  unsigned long elapsed = millis() - timer.startTime;
  if (elapsed < timer.duration) timer.savedRemainingTime = timer.duration - elapsed;
  else timer.savedRemainingTime = 0;
}

void resumeTimer() {
  if (timer.isRunning) return;
  timer.isRunning = true;
  timer.startTime = millis() - (timer.duration - timer.savedRemainingTime);
}

void checkTimerFinish() {
  unsigned long elapsed = millis() - timer.startTime;
  if (elapsed >= timer.duration) {
    if (timer.isWorkSession) {
      timer.isWorkSession = false;
      timer.pomodorosCompleted++;
      if (timer.pomodorosCompleted % 4 == 0) {
        timer.duration = (long)settings.longRestTime * 60000;
        soundLongRest();
      }
      else {
        timer.duration = (long)settings.shortRestTime * 60000;
         soundShortRest();
      }
    } else {
      timer.isWorkSession = true;
      timer.sessionNumber++;
      timer.duration = (long)settings.workTime * 60000;
      soundWorkStart();
    }
    timer.startTime = millis();
    timer.isRunning = true; 
    tft.fillScreen(COLOR_BG); 
    displayPomodoro(true);
  }
}

unsigned long getRemainingTime() {
  if (!timer.isRunning) return timer.savedRemainingTime;
  unsigned long elapsed = millis() - timer.startTime;
  if (elapsed >= timer.duration) return 0;
  return timer.duration - elapsed;
}

void drawRealTimeCorner() {
  updateCurrentTime();
  tft.setTextSize(1);
  tft.setTextColor(COLOR_DIM, COLOR_BG); 
  tft.setCursor(125, 5);
  
  if (currentHour < 10) tft.print("0");
  tft.print(currentHour);
  tft.print(":");
  if (currentMinute < 10) tft.print("0");
  tft.print(currentMinute);
}

void displayClock(bool fullRedraw) {
  if (fullRedraw) tft.fillScreen(COLOR_BG);
  drawRealTimeCorner(); 
  updateCurrentTime();
  tft.setTextSize(3);
  tft.setTextColor(COLOR_ACCENT, COLOR_BG); 
  tft.setCursor(35, 40); 
  
  if (currentHour < 10) tft.print("0");
  tft.print(currentHour);
  tft.print(":");
  if (currentMinute < 10) tft.print("0");
  tft.print(currentMinute);
  
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT, COLOR_BG);
  tft.setCursor(45, 80);
  tft.print("Press START");
}

void displayPomodoro(bool fullRedraw) {
  if (fullRedraw) tft.fillScreen(COLOR_BG);
  drawRealTimeCorner(); 
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  
  if (timer.isWorkSession) {
    tft.setTextColor(COLOR_ALERT, COLOR_BG);
    tft.print("WORK   "); 
  } else {
    if (timer.duration > (long)settings.shortRestTime * 60000 + 1000) { 
        tft.setTextColor(COLOR_BLUE, COLOR_BG);
        tft.print("L-REST");
    } else {
        tft.setTextColor(COLOR_ACCENT, COLOR_BG);
        tft.print("REST   ");
    }
  }
  
  tft.setTextColor(COLOR_DIM, COLOR_BG);
  tft.setTextSize(1);
  tft.setCursor(90, 5); 
  tft.print("#"); tft.print(timer.sessionNumber);

  tft.setTextSize(4);
  tft.setTextColor(COLOR_TEXT, COLOR_BG); 
  tft.setCursor(20, 50); 
  unsigned long remaining = getRemainingTime();
  if (remaining > 0) remaining += 900; 
  int minutes = (remaining / 1000) / 60;
  int seconds = (remaining / 1000) % 60;
  if (minutes < 10) tft.print("0");
  tft.print(minutes);
  tft.print(":"); 
  if (seconds < 10) tft.print("0");
  tft.print(seconds);
  tft.setTextSize(1);
  tft.setCursor(60, 110);
  if (timer.isRunning) {
      tft.setTextColor(COLOR_DIM, COLOR_BG);
      tft.print("RUNNING");
  } else {
      tft.setTextColor(COLOR_ALERT, COLOR_BG);
      tft.print("PAUSED ");
  }
}

void displayConfig() {
  drawRealTimeCorner();
  tft.setTextColor(COLOR_TEXT, COLOR_BG);
  tft.setTextSize(2);
  tft.setCursor(10, 5);
  tft.print("SETUP");
  tft.setTextSize(1);
  int yStart = 35;
  int yStep = 20;
  
  if (configState == CONFIG_MENU) {
    tft.setCursor(10, yStart);
    tft.print(selectedParam == 0 ? "> " : "  "); tft.print("Work: "); tft.print(settings.workTime); tft.print(" min  ");
    tft.setCursor(10, yStart + yStep);
    tft.print(selectedParam == 1 ? "> " : "  "); tft.print("Rest: "); tft.print(settings.shortRestTime); tft.print(" min  ");
    tft.setCursor(10, yStart + yStep * 2);
    tft.print(selectedParam == 2 ? "> " : "  "); tft.print("Long: "); tft.print(settings.longRestTime); tft.print(" min  ");
    tft.setCursor(10, yStart + yStep * 3);
    tft.print(selectedParam == 3 ? "> " : "  "); tft.print("START TIMER");
  } else {
    tft.setCursor(10, 40);
    tft.setTextSize(2);
    if (configState == CONFIG_WORK) {
      tft.print("Work Time:"); tft.setCursor(60, 70); tft.setTextSize(3);
      tft.setTextColor(COLOR_ACCENT, COLOR_BG); tft.print(settings.workTime);
    }
    else if (configState == CONFIG_S_REST) {
      tft.print("Short Rest:"); tft.setCursor(60, 70); tft.setTextSize(3);
      tft.setTextColor(COLOR_ACCENT, COLOR_BG); tft.print(settings.shortRestTime);
    }
    else if (configState == CONFIG_L_REST) {
      tft.print("Long Rest:"); tft.setCursor(60, 70); tft.setTextSize(3);
      tft.setTextColor(COLOR_BLUE, COLOR_BG); tft.print(settings.longRestTime);
    }
    tft.print(" "); 
    tft.setTextSize(1); tft.setTextColor(COLOR_DIM, COLOR_BG);
    tft.setCursor(10, 100); tft.print("Click to Save");
  }
}
void soundWorkStart() {
  tone(BUZZER_PIN, 1200, 150);
  delay(180);
  tone(BUZZER_PIN, 1600, 150);
}

void soundShortRest() {
  tone(BUZZER_PIN, 800, 300);
}

void soundLongRest() {
  tone(BUZZER_PIN, 600, 200);
  delay(250);
  tone(BUZZER_PIN, 600, 200);
  delay(250);
  tone(BUZZER_PIN, 1000, 400);
}
