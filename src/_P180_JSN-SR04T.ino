//#######################################################################################################
//################ Plugin 180: Ultrasonic Sernsor JSN-SR04T / DYP-ME007Y-TX  ############################
//#######################################################################################################
/*
  Plugin written by: Marcus Schwager mwulf12__AT__gmx.net

  This plugin reads the co2 value of UltraSonic Distance Sensors.
  DYP-ME007Y-TX / JSN-SR04T

*/

#define PLUGIN_180
#define PLUGIN_ID_180         180
#define PLUGIN_NAME_180       "Ultrasonic Sensor JSN-SR04T"
#define PLUGIN_VALUENAME1_180 "Distance"

boolean Plugin_180_init = false;

#include <SoftwareSerial.h>
SoftwareSerial *Plugin_180_4T;

byte myBuffer[4] = {0};
byte myByte = 0;
long mili = 0;

boolean Plugin_180(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_180;
        Device[deviceCount].Type = DEVICE_TYPE_DUAL;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 1;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_180);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_180));
        break;
      }

    case PLUGIN_INIT:
      {
        Plugin_180_init = true;
        Plugin_180_4T = new SoftwareSerial(Settings.TaskDevicePin1[event->TaskIndex], Settings.TaskDevicePin2[event->TaskIndex]);
        // TODO: Explain this in plugin description RX=GPIO Setting 1, TX=GPIO Setting 2, Use 1kOhm in serie on datapins!
        success = true;
        break;
      }

    case PLUGIN_READ:
      {

        if (Plugin_180_init)
        {
          mili = -1; //set to fix value
          if (Plugin_180_4T->available() > 3) {
            myByte = Plugin_180_4T->read();
            if (myByte==0xFF) {
              myBuffer[0] = Plugin_180_4T->read(); //HighByte
			        myBuffer[1] = Plugin_180_4T->read(); //LowByte
			        Plugin_180_4T->flush(); //weitere Werte verwerfen
			      }
          }
		      mili = myBuffer[0]<<8 | myBuffer[1]; // High und Low verodern

          int value = mili;
          UserVar[event->BaseVarIndex] = (float)value;
          String log = F("JSN-SR04T distance in mm: ");
          log += value;
          addLog(LOG_LEVEL_INFO, log);
          success = true;
          break;
        }
        break;
      }
  }
  return success;
}
