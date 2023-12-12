// Includes- working Version 1.3
#include <LSM303AGR_ACC_Sensor.h>
#include <Adafruit_Microbit.h>
#include <Tone.h>

#define DEV_I2C Wire1  // Define which I2C bus is used. Wire1 for the Microbit V2. Wire is used for the Microbit V1
LSM303AGR_ACC_Sensor Acc(&DEV_I2C);
Adafruit_Microbit_Matrix microbit;

// Define initial threshold and sensitivity values
int Threshold = 1000; // This particular threshold value has been adjusted based on analysis and experimentation 
int Sensitivity = 100; // The sensitivity value is set to 100, influencing the detection range around the threshold.

// Initialize step counter variable
int StepCount = 0;

// Define debounce constants
const unsigned long DebounceTime = 500;  // Minimum time between consecutive step detections (in milliseconds)
unsigned long lastStepTime = 0;  // Time stamp of the last detected step

const int speaker = 27; // Define the pin to 27 

// Variables to store maximum and minimum peaks
int MaxPeak = 0;
int MinPeak = 0;

void setup() {
  // Initialize serial for output.
  Serial.begin(9600);
  // Initialize I2C bus.
  DEV_I2C.begin();
  // Initialize components.
  Acc.begin();
  Acc.Enable();
  microbit.begin();

  pinMode(speaker, OUTPUT); // Set the speaker as an output

}

void loop() {
  int32_t accelerometer[3];
  Acc.GetAxes(accelerometer); // reads the accelerometer data (X, Y, and Z axes) and stores it in the accelerometer array.

/*      Serial.print("A0:");
      Serial.print(accelerometer[0]);
      Serial.print(", A1:");
      Serial.print(accelerometer[1]);
      Serial.print(", A2:");
      Serial.print(accelerometer[2]);    
*/ // remove comments for accessing real-time data and view in serial plotter                                          
  // Euclidean Norm formula to calculate the magnitude of acceleration vector using all 3 axes
  float Magnitude = sqrt((pow(accelerometer[0], 2)) + (pow(accelerometer[1], 2)) + (pow(accelerometer[2], 2)));

  // Check for maximum and minimum peaks
  if (Magnitude > (Threshold + Sensitivity / 2)) {
    // Detected maximum peak
    MaxPeak = Magnitude;
  } else if (Magnitude < (Threshold - Sensitivity / 2)) {
    // Detected minimum peak
    MinPeak = Magnitude;
  }

  // Check for step detection
  if (MaxPeak > 0 && MinPeak > 0) {
    // Check if enough time has passed since the last step
    unsigned long currentTime = millis();
    if (currentTime - lastStepTime >= DebounceTime) 
    /*  If a step is detected, it increments the step counter, prints the count to the serial monitor, 
        displays it on the Micro:Bit's matrix, and updates the "lastStepTime"
    */
    {
      // Increment step counter
      StepCount++;
      Serial.println(StepCount);
      microbit.print(StepCount % 10);
      playSound(); // Play a short sound when a step is detected
      delay(250);
      // Update the last step time
      lastStepTime = currentTime;
    }

    // Reset peaks after step detection
    MaxPeak = 0;
    MinPeak = 0;
  }
/*    Serial.print("\nMagnitude=");
      Serial.println(Magnitude);
      Serial.print("Threshold=");
      Serial.println(Threshold);
*/ // remove comments to view magnitude and threshold values at any instant
// Print step count data to Serial Monitor
  if (Serial.available() > 0)
  {
    char command = Serial.read();
    if (command == 'D' || command == 'd') // press 'D' or 'd' to access the data at that instant 
    {
      Serial.print("\nA0:");
      Serial.print(accelerometer[0]);
      Serial.print(", A1:");
      Serial.print(accelerometer[1]);
      Serial.print(", A2:");
      Serial.print(accelerometer[2]);
      Serial.print("\nMagnitude=");
      Serial.println(Magnitude);
      Serial.print("Threshold=");
      Serial.println(Threshold);
      Serial.print("Step Count:");
      Serial.println(StepCount);
    }
  }

  delay(50); // delay is set to 50ms for accurate data
}
// function to play sound when a step is detected 
void playSound()
{
  // Generate a short sound when a step is detected
  tone(speaker, 1500, 50); // Adjust the frequency and duration as needed
  delay(50); // Add a short delay to distinguish individual steps
  noTone(speaker); // Turn off the sound
}
