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
// FUNCTION 4: Determine Alert Level

void determineAlert() {

  if (fusion_confirmed == false) {
    // No verified threat
    alert_level = 0;

  } else if (ttc_seconds > 5.0) {
    // Threat far away
    alert_level = 1;

  } else if (ttc_seconds > 2.5) {
    // Threat getting close
    alert_level = 2;

  } else {
    // Imminent collision
    alert_level = 3;
  }
}


// FUNCTION 5: Print Report to Serial

void printReport() {

  Serial.println("----------------------------");
  Serial.print("CYCLE: ");
  Serial.println(scan_cycle);
  Serial.println("----------------------------");

  Serial.print("Distance    : ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  Serial.print("Temperature : ");
  Serial.print(temperature_c);
  Serial.println(" C");

  Serial.println("----------------------------");

  Serial.print("Radar       : ");
  if (radar_detected) {
    Serial.println("DETECTED");
  } else {
    Serial.println("CLEAR");
  }

  Serial.print("Thermal     : ");
  if (thermal_verified) {
    Serial.println("HEAT VERIFIED");
  } else {
    Serial.println("COLD - Ignored");
  }

  Serial.print("Fusion Gate : ");
  if (fusion_confirmed) {
    Serial.println("THREAT CONFIRMED");
  } else {
    Serial.println("NO THREAT");
  }

  Serial.println("----------------------------");

  Serial.print("TTC         : ");
  Serial.print(ttc_seconds);
  Serial.println(" sec");

  Serial.print("Alert Level : ");
  Serial.println(alert_level);

  if (alert_level == 0) {
    Serial.println("STATUS: GREEN - SAFE");
  }
  if (alert_level == 1) {
    Serial.println("STATUS: YELLOW - CAUTION");
  }
  if (alert_level == 2) {
    Serial.println("STATUS: RED - DANGER");
  }
  if (alert_level == 3) {
    Serial.println("STATUS: RED - CRITICAL");
    Serial.println(">> Engine cutoff ACTIVE");
    Serial.println(">> Emergency brake ENGAGED");
  }

  Serial.println("----------------------------");
  Serial.println("");
}


// FUNCTION 6: Set Physical Outputs

void setOutputs() {

  // Turn everything OFF first
  digitalWrite(GREEN_LED,  LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(RED_LED,    LOW);
  noTone(BUZZER_PIN);

  // Level 0 = Green LED only
  if (alert_level == 0) {
    digitalWrite(GREEN_LED, HIGH);
  }

  // Level 1 = Yellow LED + low buzz
  if (alert_level == 1) {
    digitalWrite(YELLOW_LED, HIGH);
    tone(BUZZER_PIN, 500);
  }

  // Level 2 = Red LED + medium buzz
  if (alert_level == 2) {
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER_PIN, 1000);
  }

  // Level 3 = Red LED + high buzz
  if (alert_level == 3) {
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER_PIN, 2000);
  }
}


// FUNCTION 7: Startup Test Sequence

void testStartup() {

  Serial.println("============================");
  Serial.println("  DVISS SYSTEM STARTING");
  Serial.println("  Rishi Gauttam");
  Serial.println("  Amit Singh Rawat");
  Serial.println("  SRHU - BTech AI & ML");
  Serial.println("============================");
  delay(500);

  Serial.println(">> Testing GREEN LED...");
  digitalWrite(GREEN_LED, HIGH);
  delay(500);
  digitalWrite(GREEN_LED, LOW);

  Serial.println(">> Testing YELLOW LED...");
  digitalWrite(YELLOW_LED, HIGH);
  delay(500);
  digitalWrite(YELLOW_LED, LOW);

  Serial.println(">> Testing RED LED...");
  digitalWrite(RED_LED, HIGH);
  delay(500);
  digitalWrite(RED_LED, LOW);

  Serial.println(">> Testing BUZZER...");
  tone(BUZZER_PIN, 1000);
  delay(400);
  noTone(BUZZER_PIN);

  delay(300);

  Serial.println("============================");
  Serial.println("  ALL SYSTEMS OK");
  Serial.println("============================");
  Serial.println("");
  Serial.println("INSTRUCTIONS:");
  Serial.println("Rotate POT on A0 = distance");
  Serial.println("Rotate POT on A1 = temperature");
  Serial.println("");
  Serial.println("ALERT LEVELS:");
  Serial.println("Level 0 = GREEN  = Safe");
  Serial.println("Level 1 = YELLOW = Caution");
  Serial.println("Level 2 = RED    = Danger");
  Serial.println("Level 3 = RED    = CRITICAL");
  Serial.println("");
  Serial.println(">> Scan starting now...");
  Serial.println("");
  delay(1000);
}


// END OF DVISS PHASE 1
