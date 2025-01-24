#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <TimeLib.h>
// Wi-Fi Credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";
int botRequestDelay = 0;
unsigned long lastTimeBotRan;
unsigned long startMillis = 0;  // Declare and initialize startMillis
// Telegram Bot Token
String BOTtoken = "";  // Your Bot Token
//create the bot here https://t.me/BotFather
// Authorized User Chat IDs
String authorizedChatIDs[] = {"0123456789"};
//get your id here https://t.me/myidbot
WiFiClientSecure clientTCP;
UniversalTelegramBot bot(BOTtoken, clientTCP);



#define POSITION 10  // Each wheel has 10 positions
#define DIGIT 4
#define DISP_POS 0
#define STEPS_PER_ROTATION 4096 // steps of a single rotation of motor
#define KILL_BACKLASH 15 // backlash compensation value
#define MOTOR_DELAY 1100 // wait for a single step of stepper (motor speed)
#define ENDSTOP_RELEASE -15
int port[4] = {13, 2, 14, 15};

// Sequence of stepper motor control
int seq[8][4] = {
  { LOW, HIGH, HIGH, LOW },
  { LOW, LOW, HIGH, LOW },
  { LOW, LOW, HIGH, HIGH },
  { LOW, LOW, LOW, HIGH },
  { HIGH, LOW, LOW, HIGH },
  { HIGH, LOW, LOW, LOW },
  { HIGH, HIGH, LOW, LOW },
  { LOW, HIGH, LOW, LOW }
};

typedef struct {
  int v[DIGIT];
} Digit;

void printDigit(Digit d);
Digit rotUp(Digit current, int digit, int num);
Digit rotDown(Digit current, int digit, int num);
Digit rotDigit(Digit current, int digit, int num);
Digit setDigit(Digit current, int digit, int num);
int setNumber(Digit n);

// Avoid error accumulation of fractional part of 4096 / POSITION
void rotStep(int s) {
  static long currentPos;
  static long currentStep;

  currentPos += s;
  long diff = currentPos * STEPS_PER_ROTATION / POSITION - currentStep;
  if (diff < 0) diff -= KILL_BACKLASH;
  else diff += KILL_BACKLASH;
  rotate(diff);
  currentStep += diff;
}

void printDigit(Digit d) {
  String s = "            ";
  int i;

  for (i = 0; i < DIGIT; i++) {
    s.setCharAt(i, d.v[i] + '0');
  }
  Serial.println(s);
}

Digit current = { DISP_POS, DISP_POS, DISP_POS, DISP_POS };

// Increase specified digit
Digit rotUp(Digit current, int digit, int num) {
  int freeplay = 0;
  int i;

  for (i = digit; i < DIGIT - 1; i++) {
    int id = current.v[i];
    int nd = current.v[i + 1];
    if (id <= nd) id += POSITION;
    freeplay += id - nd - 1;
  }
  freeplay += num;
  rotStep(-1 * freeplay);
  current.v[digit] = (current.v[digit] + num) % POSITION;
  for (i = digit + 1; i < DIGIT; i++) {
    current.v[i] = (current.v[i - 1] + (POSITION - 1)) % POSITION;
  }
  Serial.print("Up end: ");
  printDigit(current);
  return current;
}

// Decrease specified digit
Digit rotDown(Digit current, int digit, int num) {
  int freeplay = 0;
  int i;

  for (i = digit; i < DIGIT - 1; i++) {
    int id = current.v[i];
    int nd = current.v[i + 1];
    if (id > nd) nd += POSITION;
    freeplay += nd - id;
  }
  freeplay += num;
  rotStep(1 * freeplay);
  current.v[digit] = (current.v[digit] - num + POSITION) % POSITION;
  for (i = digit + 1; i < DIGIT; i++) {
    current.v[i] = current.v[i - 1];
  }
  Serial.print("Down end: ");
  printDigit(current);
  return current;
}

// Decrease or increase specified digit
Digit rotDigit(Digit current, int digit, int num) {
  if (num > 0) {
    return rotUp(current, digit, num);
  } else if (num < 0) {
    return rotDown(current, digit, -num);
  } else return current;
}

// Set single digit to the specified number
Digit setDigit(Digit current, int digit, int num) {
  int pd, cd = current.v[digit];
  if (digit == 0) {  // Most significant digit
    pd = DISP_POS;
  } else {
    pd = current.v[digit - 1];
  }
  if (cd == num) return current;

  // Check if increasing rotation is possible
  int n2 = num;
  if (n2 < cd) n2 += POSITION;
  if (pd < cd) pd += POSITION;
  if (pd <= cd || pd > n2) {
    return rotDigit(current, digit, n2 - cd);
  }
  // If not, do decrease rotation
  if (num > cd) cd += POSITION;
  return rotDigit(current, digit, num - cd);
}

int setNumber(Digit n) {
  int i;

  for (i = 0; i < DIGIT; i++) {
    if (current.v[i] != n.v[i]) {
      break;
    }
  }
  // Rotate most significant wheel only to follow current time ASAP
  if (i < DIGIT) {
    current = setDigit(current, i, n.v[i]);
  }
  if (i >= DIGIT - 1) {
    return true;  // Finished
  } else {
    return false;
  }
}

bool isAuthorized(String chat_id) {
  for (String id : authorizedChatIDs) {
    if (chat_id == id) {
      return true;
    }
  }
  return false;
}

void handleNewMessages(int numNewMessages) {
  Serial.print("Handle New Messages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (!isAuthorized(chat_id)) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Received message
    String text = bot.messages[i].text;
    Serial.println("Received: " + text);

    String from_name = bot.messages[i].from_name;
    if (text == "/start") {
      String welcome = "Welcome, " + from_name + "\n";
      welcome += "Please send a 4-digit number to display:\n";
      welcome += "Example: 1234";
      bot.sendMessage(chat_id, welcome, "");
    } else if (text.length() == 4 && text.toInt() != 0) {
      int number = text.toInt();
      bot.sendMessage(chat_id, "Displaying number: " + text, "");
      displayNumber(number); // Call the function to display the number
    } else {
      bot.sendMessage(chat_id, "Invalid input. Please send a 4-digit number.", "");
    }
  }
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  pinMode(port[0], OUTPUT);
  pinMode(port[1], OUTPUT);
  pinMode(port[2], OUTPUT);
  pinMode(port[3], OUTPUT);
  rotate(STEPS_PER_ROTATION * DIGIT);  // Reset all digits using physical end stop
  rotate(ENDSTOP_RELEASE * DIGIT);     // Release pushing force to align all digits better

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  clientTCP.setInsecure(); // For secure connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWi-Fi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  startMillis = millis();  // Set startMillis when the program starts
}

void processTelegramMessages() {
  // Handle Telegram messages
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}

void loop() {
  
  processTelegramMessages();
}

void displayNumber(int number) {
  // Parse the digits
  Digit n;
  n.v[0] = (number / 1000) % 10;  // Thousands
  n.v[1] = (number / 100) % 10;   // Hundreds
  n.v[2] = (number / 10) % 10;    // Tens
  n.v[3] = number % 10;           // Units

  Serial.println("Parsed Digits: " + String(n.v[0]) + String(n.v[1]) + String(n.v[2]) + String(n.v[3]));

  // Use the existing motor logic to rotate the digits
  while (!setNumber(n)) {
    delay(10); // Allow time for motors to rotate
  }

  Serial.println("Number displayed successfully.");
}


void rotate(int step) {
  int count = 0;
  static int phase = 0;
  int i, j;
  int delta = (step > 0) ? 1 : 7;

  step = (step > 0) ? step : -step;
  for (j = 0; j < step; j++) {
    phase = (phase + delta) % 8;
    for (i = 0; i < 4; i++) {
      digitalWrite(port[i], seq[phase][i]);
    }
    count++;
    delayMicroseconds(MOTOR_DELAY);
    if (step - j < 50) delayMicroseconds(MOTOR_DELAY * 2);  // Deceleration
    if (step - j < 100) delayMicroseconds(MOTOR_DELAY * 2);
    if (step - j < 200) delayMicroseconds(MOTOR_DELAY * 2);
    if (count < 40) delayMicroseconds(MOTOR_DELAY);  // Acceleration
    if (count < 30) delayMicroseconds(MOTOR_DELAY);
    if (count < 20) delayMicroseconds(MOTOR_DELAY);
    if (count < 10) delayMicroseconds(MOTOR_DELAY);
  }
  // Power cut
  for (i = 0; i < 4; i++) {
    digitalWrite(port[i], LOW);
  }
}

