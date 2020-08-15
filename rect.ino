/**
 * The number of physical rotor faces; this will likely be 3 unless you're doing
 * something really neat and interesting
 */
#define ROTOR_FACES 3

/** The pin for our pressure transducer */
const int SENSOR_PIN = A7;
/** Hysteresis used in our peak detectors, as an ADC unit; 27 = ~5 PSI  */
const int HYSTERESIS = 27;

/** The input (domain) of the sensor, in PSI */
const float SENSOR_PSI_MAX = 150.0;
/** The output (range) of the sensor, in volts */
const float SENSOR_V[2] = {0.5, 4.5};
/** The maximum output of our ADC */
const int ADC_MAX = 1023; // 2^10 - 1
/** The maximum input voltage of our ADC */
const float ADC_V_MAX = 5.0;

/** How many times to read the sensor during calibration */
const int CALIBRATION_COUNT = 10;
/** The delay, in ms, between calibration reads */
const int CALIBRATION_DELAY = 100; // ms
/**
 * The average sensor value measured during calibration; this should correspond
 * to 0 PSI
 */
int CALIBRATION_VALUE = 0;

/**
 * Convert an ADC value to human friendly PSI
 *
 * We treat the CALIBRATION_VALUE measured at startup as 0 PSI, so start by
 * offsetting the measured value. We also know the volts per adc unit, and from
 * the sensor specs can determine the volts per psi. Using these values, we can
 * convert our ADC value to PSI.
 */
float convertToPSI(int value)
{
  const int delta = value - CALIBRATION_VALUE; // adc

  const float a = ADC_V_MAX / ADC_MAX;                          // volts / adc
  const float b = (SENSOR_V[1] - SENSOR_V[0]) / SENSOR_PSI_MAX; // volts / psi

  return delta * a / b;
}

/**
 * The avg time to perform a full rotation is the average peak to peak duration
 * multiplied by the number of rotor faces.
 *
 * This avoids the problem of RPM calculations being off due to any down time
 * between engine cranking.
 */
unsigned long calculateRotorRotationDuration(unsigned long peakTimes[ROTOR_FACES])
{
  // Calculate the average peak to peak duration
  unsigned long avgPeakToPeakDuration = 0.0; // ms
  for (int i = 0; i < ROTOR_FACES - 1; i++)
  {
    avgPeakToPeakDuration += (peakTimes[i + 1] - peakTimes[i]);
  }
  avgPeakToPeakDuration /= ROTOR_FACES - 1;

  // Now multiply the peak to peak duration by the number of faces
  return avgPeakToPeakDuration * ROTOR_FACES;
}

/**
 * The output shaft of a rotary engine rotates 3 times for every complete
 * rotation of the rotor. Thus if we know the duration of a single rotor
 * rotation, we can calculate the engine RPM.
 *
 * Unit analysis: a = (duration) ms / rotor_rotation b = (1000 * 60) ms / minute
 *  c = (3) output_rotations / rotor_rotation rpm = (c * b / a) output_rotations
 *  / minute
 */
float calculateRPM(unsigned long duration)
{
  return 3.0 * 1000.0 * 60.0 / duration;
}

/** Setup code */
void setup()
{
  Serial.begin(9600);
  Serial.println(F("RECT: Rotary Engine Compression Tester"));

  // Calibration routine
  Serial.println(F("Calibrating; do not crank engine…"));
  for (int i = 0; i < CALIBRATION_COUNT; i++)
  {
    delay(CALIBRATION_DELAY);
    const int v = analogRead(SENSOR_PIN);
    Serial.print(v);
    Serial.print(F(", "));
    CALIBRATION_VALUE += v;
  }
  Serial.print(F("AVG: "));
  CALIBRATION_VALUE /= CALIBRATION_COUNT;
  Serial.println(CALIBRATION_VALUE);
  Serial.println(F("Calibration complete, commence cranking"));
}

/**
 * To check the compression on a rotary engine, we want to know the chamber's
 * pressure for each of the rotor's 3 faces. As a rotor face passes the sensor,
 * we'll expect an upper pressure peak; this is the value we want to record. As
 * it continues, we should see a lower peak between the faces, indicating that
 * we've moved on to the next face, and need to repeat the measurement.
 */
void loop()
{
  /**
   * The pressure peaks recorded for each of the rotor's faces, as an ADC 10 bit
   * unit. We do not store as PSI to avoid float calculations during the data
   * collection phase.
   */
  int facePeaks[ROTOR_FACES] = {0};

  /**
   * The time, in ms, when the peak was recorded. This is used to calculate our
   * RPM
   */
  unsigned long peakTimes[ROTOR_FACES] = {0};

  /**
   * The current sensor value; defined outside the loop so we may access its
   * values across iterations.
   */
  int v = 0;

  // Data acquisition phase
  for (int i = 0; i < ROTOR_FACES; i++)
  {
    // Initial sensor read
    v = analogRead(SENSOR_PIN);
    // Upper peak detector; here we detect the max pressure the rotor face can
    // hold
    while (v >= facePeaks[i] - HYSTERESIS)
    {
      if (v > facePeaks[i])
      {
        facePeaks[i] = v;
        peakTimes[i] = millis();
      }
      v = analogRead(SENSOR_PIN);
    }

    // Initialize our minimum at the last max
    int minimum = facePeaks[i];
    // Lower peak detector; here we detect the transition between two rotor
    // faces, so we may move on to the next iteration of the loop.
    while (v <= minimum + HYSTERESIS)
    {
      if (v < minimum)
      {
        minimum = v;
      }
      v = analogRead(SENSOR_PIN);
    }
  }

  // Data processing phase
  const float rpm = calculateRPM(calculateRotorRotationDuration(peakTimes));
  float psi[ROTOR_FACES] = {0.0};
  for (int i = 0; i < ROTOR_FACES; i++)
  {
    psi[i] = convertToPSI(facePeaks[i]);
  }

  // Now we can present the data
  {
    Serial.print("[");
    for (int i = 0; i < ROTOR_FACES; i++)
    {
      Serial.print(psi[i]);
      if (i < ROTOR_FACES - 1)
      {
        Serial.print(", ");
      }
    }
    Serial.print("] PSI @ ");
    Serial.print(rpm);
    Serial.println(" RPM");
  }
}
