#include <Wire.h>
#include <Adafruit_LSM6DSOX.h>

#define DRDY_PIN 2

// ——— CONFIG ——————————————————————————————————————————————————————————————
static const uint8_t TCA9548A_ADDR = 0x70;  // I²C address of TCA9548A MUX
static const uint8_t MUX_CH0       = 0;     // MUX channel for IMU #1
static const uint8_t MUX_CH6       = 6;     // MUX channel for IMU #2
static const uint8_t MUX_CH4       = 4;     // MUX channel for IMU #3
static const uint8_t MUX_CH3       = 3;     // MUX channel for IMU #4
static const int     INT1_PIN      = 2;     // Arduino Due pin wired to both IMU INT1 lines (they can share)

// Data‐packet for one 6-axis IMU (24 bytes total: 6 floats × 4 bytes each)
struct SixAxis {
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
};

// Two sensor objects, one per channel
Adafruit_LSM6DSOX sox0 = Adafruit_LSM6DSOX();  // IMU on channel 0
Adafruit_LSM6DSOX sox6 = Adafruit_LSM6DSOX();  // IMU on channel 6
Adafruit_LSM6DSOX sox4 = Adafruit_LSM6DSOX();  // IMU on channel 4
Adafruit_LSM6DSOX sox3 = Adafruit_LSM6DSOX();  // IMU on channel 3

// Helper: select a single channel on the TCA9548A
void selectMuxChannel(uint8_t channel) {
  Wire.beginTransmission(TCA9548A_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void setup() {
  // Initialize SerialUSB at 2 000 000 baud
  SerialUSB.begin(2000000);
  while (!SerialUSB) {}

  // Initialize I²C on SDA1/SCL1 at 1 MHz
  Wire.begin();
  Wire.setClock(1000000UL);

  pinMode(DRDY_PIN, INPUT);

  // ——— Initialize IMU #1 (channel 0) —————————————————————————————————
  selectMuxChannel(MUX_CH0);
  if (!sox0.begin_I2C()) {
    // If IMU #1 isn't found, halt and emit error
    while (1) {
      SerialUSB.println("ERROR: LSM6DSOX not found on MUX channel 0");
      delay(500);
    }
  }
  // Configure IMU #1: accel 1.66 kHz ±2 g, gyro 1.66 kHz ±250 dps
  sox0.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);
  sox0.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  sox0.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);
  sox0.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  // Only route accel+gyro to INT1 (FIFO and tap disabled)
  sox0.configInt1(false, true, true, false, false);

  // ——— Initialize IMU #2 (channel 6) —————————————————————————————————
  selectMuxChannel(MUX_CH6);
  if (!sox6.begin_I2C()) {
    // If IMU #2 isn't found, halt and emit error
    while (1) {
      SerialUSB.println("ERROR: LSM6DSOX not found on MUX channel 6");
      delay(500);
    }
  }
  // Configure IMU #2: same data rates/ranges as IMU #1
  sox6.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);
  sox6.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  sox6.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);
  sox6.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  sox6.configInt1(false, true, true, false, false);

// ——— Initialize IMU #3 (channel 4) —————————————————————————————————
  selectMuxChannel(MUX_CH4);
  if (!sox4.begin_I2C()) {
    // If IMU #1 isn't found, halt and emit error
    while (1) {
      SerialUSB.println("ERROR: LSM6DSOX not found on MUX channel 4");
      delay(500);
    }
  }
  // Configure IMU #1: accel 1.66 kHz ±2 g, gyro 1.66 kHz ±250 dps
  sox4.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);
  sox4.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  sox4.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);
  sox4.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  // Only route accel+gyro to INT1 (FIFO and tap disabled)
  sox4.configInt1(false, true, true, false, false);

  // ——— Initialize IMU #4 (channel 3) —————————————————————————————————
  selectMuxChannel(MUX_CH3);
  if (!sox3.begin_I2C()) {
    // If IMU #1 isn't found, halt and emit error
    while (1) {
      SerialUSB.println("ERROR: LSM6DSOX not found on MUX channel 3");
      delay(500);
    }
  }
  // Configure IMU #1: accel 1.66 kHz ±2 g, gyro 1.66 kHz ±250 dps
  sox3.setAccelDataRate(LSM6DS_RATE_1_66K_HZ);
  sox3.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  sox3.setGyroDataRate(LSM6DS_RATE_1_66K_HZ);
  sox3.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  // Only route accel+gyro to INT1 (FIFO and tap disabled)
  sox3.configInt1(false, true, true, false, false);
}

void loop() {
  // Only proceed if at least one IMU has data ready (shared INT1_PIN)
  // if (digitalRead(DRDY_PIN)) {
    // ——— Read from IMU #1 (channel 0) —————————————————————————————————————
    selectMuxChannel(MUX_CH0);
    sensors_event_t accel0, gyro0, temp0;
    sox0.getEvent(&accel0, &gyro0, &temp0);

    SixAxis packet0;
    packet0.ax = accel0.acceleration.x;
    packet0.ay = accel0.acceleration.y;
    packet0.az = accel0.acceleration.z;
    packet0.gx = gyro0.gyro.x;
    packet0.gy = gyro0.gyro.y;
    packet0.gz = gyro0.gyro.z;

     //Now 中断 only connect MUX_CH0

    // ——— Read from IMU #2 (channel 6) —————————————————————————————————————
    selectMuxChannel(MUX_CH6);
    sensors_event_t accel6, gyro6, temp6;
    sox6.getEvent(&accel6, &gyro6, &temp6);

    SixAxis packet6;
    packet6.ax = accel6.acceleration.x;
    packet6.ay = accel6.acceleration.y;
    packet6.az = accel6.acceleration.z;
    packet6.gx = gyro6.gyro.x;
    packet6.gy = gyro6.gyro.y;
    packet6.gz = gyro6.gyro.z;

     // ——— Read from IMU #3 (channel 4) —————————————————————————————————————
    selectMuxChannel(MUX_CH4);
    sensors_event_t accel4, gyro4, temp4;
    sox4.getEvent(&accel4, &gyro4, &temp4);

    SixAxis packet4;
    packet4.ax = accel4.acceleration.x;
    packet4.ay = accel4.acceleration.y;
    packet4.az = accel4.acceleration.z;
    packet4.gx = gyro4.gyro.x;
    packet4.gy = gyro4.gyro.y;
    packet4.gz = gyro4.gyro.z;

    // ——— Read from IMU #4 (channel 3) —————————————————————————————————————
    selectMuxChannel(MUX_CH3);
    sensors_event_t accel3, gyro3, temp3;
    sox0.getEvent(&accel3, &gyro3, &temp3);

    SixAxis packet3;
    packet3.ax = accel3.acceleration.x;
    packet3.ay = accel3.acceleration.y;
    packet3.az = accel3.acceleration.z;
    packet3.gx = gyro3.gyro.x;
    packet3.gy = gyro3.gyro.y;
    packet3.gz = gyro3.gyro.z;

    // ——— Transmit both 6-axis packets (48 bytes total) ——————————————————————
    SerialUSB.write((uint8_t*)&packet0, sizeof(packet0));  // 24 bytes
    SerialUSB.write((uint8_t*)&packet6, sizeof(packet6));  // 24 bytes
    SerialUSB.write((uint8_t*)&packet4, sizeof(packet4));  // 24 bytes
    SerialUSB.write((uint8_t*)&packet3, sizeof(packet3));  // 24 bytes
    // delay(5000);
  // }
}
