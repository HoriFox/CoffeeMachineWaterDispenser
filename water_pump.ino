#define relaySignal 2 // DigitalPin D2
#define sensorPower 3 // DigitalPin D3
#define sensorPin A6  // AnalogPin A6

// SETTING
int threshold = 200;
int block_time = 10;

// TEMP
int temp_val = 0;
int current_time = 0;
bool block = false;
int current_level = threshold;

void printLine()
{
  Serial.println();
}

template <typename T, typename... Types>
void printLine(T first, Types... other)
{
  Serial.print(first);
  printLine(other...);
}

void ledSignalWarning() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}

void setup() {
	pinMode(sensorPower, OUTPUT);
  pinMode(relaySignal, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);

	digitalWrite(sensorPower, LOW);
  digitalWrite(relaySignal, LOW);
	
	Serial.begin(9600);
}

void loop() {
  // Блокировка насоса ввиду большого кол-во выливаемой жидкости
  if (block) {
    ledSignalWarning();
    return;
  }
  current_time = 0;
	current_level = readSensor();

  printLine("> [WAIT] Current level: ", current_level);
  digitalWrite(relaySignal, LOW);

	// Пока не дольёт до детектирования - не тормозим
  while (current_level < threshold) {
    current_time += 1;
    if (current_time >= block_time) {
      block = true;
      digitalWrite(relaySignal, LOW);
      printLine("BLOCK! RECONNECT PUMP...");
      break;
    }

    printLine("> [ADD WATER] Current level: ", current_level, " | Block timer - ", current_time, "/", block_time);

    digitalWrite(relaySignal, HIGH);
    delay(1000);
    current_level = readSensor();
  }

	delay(1000);
}

int readSensor() {
  // Подаём питание на сенсор
	digitalWrite(sensorPower, HIGH);
  // Ожидаем одупления сенсора
	delay(10);
	temp_val = analogRead(sensorPin);
	digitalWrite(sensorPower, LOW);
	return temp_val;
}