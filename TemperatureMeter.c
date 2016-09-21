/*
htop - TemperatureMeter.c
Based on patch from
(C) 2013 Ralf Stemmer
(C) 2004-2011 Hisham H. Muhammad
*
(C) 2016 Bousqi
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "TemperatureMeter.h"

#include "Platform.h"
#include "CRT.h"

#include <stdlib.h>
#include <unistd.h>

/*{
#include "Meter.h"
}*/

int TemperatureMeter_attributes[] = {
   TEMPERATURE_COOL,
   TEMPERATURE_MEDIUM,
   TEMPERATURE_HOT,
};

double getProcTempData(void) {
    double temperature;
    char buffer[100];
    FILE* fp = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
    size_t bytes_read = fread(buffer, 1, sizeof(buffer), fp);
    fclose(fp);
    if(bytes_read==0 || bytes_read == sizeof(buffer))
        return 0;
    buffer[bytes_read] = '\0';
    temperature = atof(buffer)/1000;
    return temperature;
}

static void TemperatureMeter_updateValues(Meter* this, char* buffer, int len) {
   double temperature = getProcTempData();
   this->values[0] = temperature;
   
   // output the temperature
   snprintf(buffer, len, "%.2f", temperature);
}

static void TemperatureMeter_display(Object* cast, RichString* out) {
   Meter* this = (Meter*)cast;

   int textColor   = CRT_colors[METER_TEXT];
   int coolColor   = CRT_colors[TEMPERATURE_COOL];
   int mediumColor = CRT_colors[TEMPERATURE_MEDIUM];
   int hotColor    = CRT_colors[TEMPERATURE_HOT];

   double temperature = this->values[0];
   
   // choose the color for the temperature
   int tempColor;
   if      (temperature < 45)                      tempColor = coolColor;
   else if (temperature >= 45 && temperature < 60) tempColor = mediumColor;
   else                                            tempColor = hotColor;

   // output the temperature
   char buffer[20];
   sprintf(buffer, "%.2f", temperature);
   RichString_append(out, tempColor, buffer);
   RichString_append(out, textColor, "°C ");
}

MeterClass TemperatureMeter_class = {
   .super = {
      .extends = Class(Meter),
      .delete = Meter_delete,
      .display = TemperatureMeter_display,
   },
   .updateValues = TemperatureMeter_updateValues,
   .defaultMode = TEXT_METERMODE,
   .maxItems = 1,
   .total = 100.0,
   .attributes = TemperatureMeter_attributes,
   .name = "Temperature",
   .uiName = "Core Temperature",
   .caption = "Core T°: "
};
