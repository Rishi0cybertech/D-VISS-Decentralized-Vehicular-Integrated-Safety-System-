// DVISS - PHASE 1 
// Rishi Gauttam & Amit Singh Rawat
// Swami Rama Himalayan University
// Arduino Uno R3

// ---- PIN DEFINITIONS ----
#define RADAR_POT    A0
#define THERMAL_POT  A1
#define GREEN_LED    8
#define YELLOW_LED   9
#define RED_LED      10
#define BUZZER_PIN   5

// ---- THRESHOLDS ----
#define SAFE_DISTANCE      500
#define THERMAL_THRESHOLD  35

// ---- GLOBAL VARIABLES ----

float distance_cm    = 0;
float temperature_c  = 0;
bool  radar_detected    = false;
bool  thermal_verified  = false;
bool  fusion_confirmed  = false;
float ttc_seconds    = 0;
int   alert_level    = 0;
int   scan_cycle     = 0;

// ---- FORWARD DECLARATIONS ----
void readSensors();
void processFusion();
void calculateTTC();
void determineAlert();
void printReport();
void setOutputs();
void testStartup();

// SETUP

void setup() {

  // Start serial at 9600
  Serial.begin(9600);

  // Small wait for serial to connect
  delay(1000);

  // Set pin modes
  pinMode(GREEN_LED,  OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // All outputs OFF
  digitalWrite(GREEN_LED,  LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED,    LOW);
  noTone(BUZZER_PIN);

  // Run startup test
  testStartup();
}

// LOOP

void loop() {

  scan_cycle++;

  readSensors();
  processFusion();
  calculateTTC();
  determineAlert();
  printReport();
  setOutputs();

  delay(1000);
}

// FUNCTION 1: Read Sensors

void readSensors() {

  // Read raw ADC values from potentiometers
  int radar_raw   = analogRead(RADAR_POT);
  int thermal_raw = analogRead(THERMAL_POT);

  // Map to real-world values
  // Radar:   0-1023 mapped to 50-600 cm
  // Thermal: 0-1023 mapped to 15-80 Celsius
  distance_cm   = map(radar_raw,   0, 1023, 50,  600);
  temperature_c = map(thermal_raw, 0, 1023, 15,  80);
}

// FUNCTION 2: Sensor Fusion Logic

void processFusion() {

  // LAYER 1: Radar check
  // Object detected if within 500cm
  if (distance_cm < SAFE_DISTANCE) {
    radar_detected = true;
  } else {
    radar_detected = false;
  }

  // LAYER 2: Thermal check
  // Heat verified if above 35 Celsius
  if (temperature_c > THERMAL_THRESHOLD) {
    thermal_verified = true;
  } else {
    thermal_verified = false;
  }

  // FUSION GATE: Both must be true
  // Eliminates false positives
  if (radar_detected == true && thermal_verified == true) {
    fusion_confirmed = true;
  } else {
    fusion_confirmed = false;
  }
}


// FUNCTION 3: Time To Collision

void calculateTTC() {

  // Vehicle speed = 60 km/h = 16.67 m/s
  // TTC = distance in meters / speed
  float speed = 16.67;
  float dist_meters = distance_cm / 100.0;
  ttc_seconds = dist_meters / speed;
}

// ========================================
// END OF DVISS PHASE 1
// ========================================
