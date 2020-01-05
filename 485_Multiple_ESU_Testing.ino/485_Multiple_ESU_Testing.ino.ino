#include <bmp3.h>
#include <bmp3_defs.h>

struct bmp3_dev dev;
struct bmp3_dev dev2;
struct bmp3_data data;
struct bmp3_data data2;

static void parse_sensor_data(const uint8_t *reg_data, struct bmp3_uncomp_data *uncomp_data)
{
    /* Temporary variables to store the sensor data */
    uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;

    /* Store the parsed register values for pressure data */
    data_xlsb = (uint32_t)reg_data[0];
    data_lsb = (uint32_t)reg_data[1] << 8;
    data_msb = (uint32_t)reg_data[2] << 16;
    uncomp_data->pressure = data_msb | data_lsb | data_xlsb;

    /* Store the parsed register values for temperature data */
    data_xlsb = (uint32_t)reg_data[3];
    data_lsb = (uint32_t)reg_data[4] << 8;
    data_msb = (uint32_t)reg_data[5] << 16;
    uncomp_data->temperature = data_msb | data_lsb | data_xlsb;
}

static int8_t compensate_data(uint8_t sensor_comp,
                              const struct bmp3_uncomp_data *uncomp_data,
                              struct bmp3_data *comp_data,
                              struct bmp3_calib_data *calib_data)
{
    int8_t rslt = BMP3_OK;

    if ((uncomp_data != NULL) && (comp_data != NULL) && (calib_data != NULL))
    {
        /* If pressure or temperature component is selected */
        if (sensor_comp & (BMP3_PRESS | BMP3_TEMP))
        {
            /* Compensate the temperature data */
            comp_data->temperature = compensate_temperature(uncomp_data, calib_data);
        }
        if (sensor_comp & BMP3_PRESS)
        {
            /* Compensate the pressure data */
            comp_data->pressure = compensate_pressure(uncomp_data, calib_data);
        }

    }
    else
    {
        rslt = BMP3_E_NULL_PTR;
    }

    return rslt;
}

static void parse_calib_data(const uint8_t *reg_data, struct bmp3_dev *dev)
{
    /* Temporary variable to store the aligned trim data */
    struct bmp3_reg_calib_data *reg_calib_data = &dev->calib_data.reg_calib_data;
    struct bmp3_quantized_calib_data *quantized_calib_data = &dev->calib_data.quantized_calib_data;

    /* Temporary variable */
    double temp_var;

    /* 1 / 2^8 */
    temp_var = 0.00390625f;
    reg_calib_data->par_t1 = BMP3_CONCAT_BYTES(reg_data[1], reg_data[0]);
    quantized_calib_data->par_t1 = ((double)reg_calib_data->par_t1 / temp_var);
    reg_calib_data->par_t2 = BMP3_CONCAT_BYTES(reg_data[3], reg_data[2]);
    temp_var = 1073741824.0f;
    quantized_calib_data->par_t2 = ((double)reg_calib_data->par_t2 / temp_var);
    reg_calib_data->par_t3 = (int8_t)reg_data[4];
    temp_var = 281474976710656.0f;
    quantized_calib_data->par_t3 = ((double)reg_calib_data->par_t3 / temp_var);
    reg_calib_data->par_p1 = (int16_t)BMP3_CONCAT_BYTES(reg_data[6], reg_data[5]);
    temp_var = 1048576.0f;
    quantized_calib_data->par_p1 = ((double)(reg_calib_data->par_p1 - (16384)) / temp_var);
    reg_calib_data->par_p2 = (int16_t)BMP3_CONCAT_BYTES(reg_data[8], reg_data[7]);
    temp_var = 536870912.0f;
    quantized_calib_data->par_p2 = ((double)(reg_calib_data->par_p2 - (16384)) / temp_var);
    reg_calib_data->par_p3 = (int8_t)reg_data[9];
    temp_var = 4294967296.0f;
    quantized_calib_data->par_p3 = ((double)reg_calib_data->par_p3 / temp_var);
    reg_calib_data->par_p4 = (int8_t)reg_data[10];
    temp_var = 137438953472.0f;
    quantized_calib_data->par_p4 = ((double)reg_calib_data->par_p4 / temp_var);
    reg_calib_data->par_p5 = BMP3_CONCAT_BYTES(reg_data[12], reg_data[11]);

    /* 1 / 2^3 */
    temp_var = 0.125f;
    quantized_calib_data->par_p5 = ((double)reg_calib_data->par_p5 / temp_var);
    reg_calib_data->par_p6 = BMP3_CONCAT_BYTES(reg_data[14], reg_data[13]);
    temp_var = 64.0f;
    quantized_calib_data->par_p6 = ((double)reg_calib_data->par_p6 / temp_var);
    reg_calib_data->par_p7 = (int8_t)reg_data[15];
    temp_var = 256.0f;
    quantized_calib_data->par_p7 = ((double)reg_calib_data->par_p7 / temp_var);
    reg_calib_data->par_p8 = (int8_t)reg_data[16];
    temp_var = 32768.0f;
    quantized_calib_data->par_p8 = ((double)reg_calib_data->par_p8 / temp_var);
    reg_calib_data->par_p9 = (int16_t)BMP3_CONCAT_BYTES(reg_data[18], reg_data[17]);
    temp_var = 281474976710656.0f;
    quantized_calib_data->par_p9 = ((double)reg_calib_data->par_p9 / temp_var);
    reg_calib_data->par_p10 = (int8_t)reg_data[19];
    temp_var = 281474976710656.0f;
    quantized_calib_data->par_p10 = ((double)reg_calib_data->par_p10 / temp_var);
    reg_calib_data->par_p11 = (int8_t)reg_data[20];
    temp_var = 36893488147419103232.0f;
    quantized_calib_data->par_p11 = ((double)reg_calib_data->par_p11 / temp_var);
}

/*!
 * @brief This internal API is used to compensate the raw temperature data and
 * return the compensated temperature data in double data type.
 * for e.g. returns temperature 24.26 deg Celsius
 */
static double compensate_temperature(const struct bmp3_uncomp_data *uncomp_data, struct bmp3_calib_data *calib_data)
{
    uint32_t uncomp_temp = uncomp_data->temperature;
    double partial_data1;
    double partial_data2;

    partial_data1 = (double)(uncomp_temp - calib_data->quantized_calib_data.par_t1);
    partial_data2 = (double)(partial_data1 * calib_data->quantized_calib_data.par_t2);

    /* Update the compensated temperature in calib structure since this is
     * needed for pressure calculation */
    calib_data->quantized_calib_data.t_lin = partial_data2 + (partial_data1 * partial_data1) *
                                             calib_data->quantized_calib_data.par_t3;

    /* Returns compensated temperature */
    return calib_data->quantized_calib_data.t_lin;

    Serial.print("Temperature comped");
}

/*!
 * @brief This internal API is used to compensate the raw pressure data and
 * return the compensated pressure data in double data type.
 * For e.g. returns pressure in Pascal p = 95305.295 which is 953.05295 hecto pascal
 */
static double compensate_pressure(const struct bmp3_uncomp_data *uncomp_data, const struct bmp3_calib_data *calib_data)
{
    const struct bmp3_quantized_calib_data *quantized_calib_data = &calib_data->quantized_calib_data;

    /* Variable to store the compensated pressure */
    double comp_press;

    /* Temporary variables used for compensation */
    double partial_data1;
    double partial_data2;
    double partial_data3;
    double partial_data4;
    double partial_out1;
    double partial_out2;

    partial_data1 = quantized_calib_data->par_p6 * quantized_calib_data->t_lin;
    partial_data2 = quantized_calib_data->par_p7 * bmp3_pow(quantized_calib_data->t_lin, 2);
    partial_data3 = quantized_calib_data->par_p8 * bmp3_pow(quantized_calib_data->t_lin, 3);
    partial_out1 = quantized_calib_data->par_p5 + partial_data1 + partial_data2 + partial_data3;
    partial_data1 = quantized_calib_data->par_p2 * quantized_calib_data->t_lin;
    partial_data2 = quantized_calib_data->par_p3 * bmp3_pow(quantized_calib_data->t_lin, 2);
    partial_data3 = quantized_calib_data->par_p4 * bmp3_pow(quantized_calib_data->t_lin, 3);
    partial_out2 = uncomp_data->pressure *
                   (quantized_calib_data->par_p1 + partial_data1 + partial_data2 + partial_data3);
    partial_data1 = bmp3_pow((double)uncomp_data->pressure, 2);
    partial_data2 = quantized_calib_data->par_p9 + quantized_calib_data->par_p10 * quantized_calib_data->t_lin;
    partial_data3 = partial_data1 * partial_data2;
    partial_data4 = partial_data3 + bmp3_pow((double)uncomp_data->pressure, 3) * quantized_calib_data->par_p11;
    comp_press = (partial_out1 + partial_out2 + partial_data4);

    return comp_press;

    Serial.print("Pressure Comped");
}

static float bmp3_pow(double base, uint8_t power)
{
    float pow_output = 1;

    while (power != 0)
    {
        pow_output = (float) base * pow_output;
        power--;
    }

    return pow_output;
}

void setup() {
  // put your setup code here, to run once:s
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);

  uint8_t calib_data_1[21] = {0xD2, 0x69, 0x62, 0x49, 0xF6, 0x92, 0xFE, 
            0x1E, 0xF5, 0x23, 0x00, 0x7F, 0x65, 0xE7, 0x7B, 0xF3, 0xF6, 0x7B, 
            0x42, 0x0A, 0xC4};
  uint8_t calib_data_2[21] = {0x5A, 0x6B, 0xCB, 0x49, 0xF6, 0xAC, 0xFF, 0xB2,
            0xF4, 0x23, 0x00, 0x4B, 0x63, 0x04, 0x79, 0xF3, 0xF6, 0x39, 0x3F, 0x13, 0xC4
            };
  parse_calib_data(calib_data_1, &dev);
  parse_calib_data(calib_data_2, &dev2);

}

char command;
char value;
char value2;
uint8_t temp_bytes[2];
uint8_t temp_bytes2[2];
uint8_t humid_bytes[2];
uint8_t humid_bytes2[2];
uint8_t reg_data[6];
uint8_t reg_data2[6];
uint32_t result;
uint16_t temp_raw;
uint16_t humid_raw;
float temp;
float temp_Faren;
int count = 0;
int count2 = 0;

struct bmp3_uncomp_data uncomp_data;
struct bmp3_uncomp_data uncomp_data2;
uint8_t sensor_comp = BMP3_PRESS;

void loop() {
  if(Serial.available()){
    command = Serial.read();
    Serial2.write(command);
    Serial3.write(command);
  }
  if(Serial2.available()){
    value = Serial2.read();
    switch(count){
      case 0:
      count += 1;
      break;
      case 1:
      temp_bytes[0] = value;
      count += 1;
      break;
      case 2:
      temp_bytes[1] = value;
      count += 1;
      break;
      case 3:
      humid_bytes[0] = value;
      count += 1;
      break;
      case 4:
      humid_bytes[1] = value;
      count += 1;
      break;
      case 5:
      reg_data[0] = value;
      count += 1;
      break;
      case 6:
      count += 1;
      reg_data[1] = value;
      break;
      case 7:
      count += 1;
      reg_data[2] = value;
      break;
      case 8:
      count += 1;
      reg_data[3] = value;
      break;
      case 9:
      count += 1;
      reg_data[4] = value;
      break;
      case 10:
      count = 0;
      reg_data[5] = value;
      parse_sensor_data(reg_data, &uncomp_data);

      result = compensate_data(sensor_comp, &uncomp_data, &data, &dev.calib_data);
      Serial.print("Pressure Reading is: ");
      Serial.println(data.pressure);

      for(int i; i < sizeof(reg_data); i++){
       reg_data[i] = 0;
      }
      break;
      default:
      Serial.print("Error Receiving data!");
      count = 0;
      break;

    }
  }

  if(Serial3.available()){
    value2 = Serial3.read();
    switch(count2){
      case 0:
      count2 += 1;
      break;
      case 1:
      temp_bytes2[0] = value2;
      count2 += 1;
      break;
      case 2:
      temp_bytes2[1] = value2;
      count2 += 1;
      break;
      case 3:
      humid_bytes2[0] = value2;
      count2 += 1;
      break;
      case 4:
      humid_bytes2[1] = value2;
      count2 += 1;
      break;
      case 5:
      reg_data2[0] = value2;
      count2 += 1;
      break;
      case 6:
      count2 += 1;
      reg_data2[1] = value2;
      break;
      case 7:
      count2 += 1;
      reg_data2[2] = value2;
      break;
      case 8:
      count2 += 1;
      reg_data2[3] = value2;
      break;
      case 9:
      count2 += 1;
      reg_data2[4] = value2;
      break;
      case 10:
      reg_data2[5] = value2;
      parse_sensor_data(reg_data2, &uncomp_data2);

      result = compensate_data(sensor_comp, &uncomp_data2, &data2, &dev2.calib_data);
      Serial.print("Pressure 2 Reading is: ");
      Serial.println(data2.pressure);
  
      for(int i; i < sizeof(reg_data2); i++){
        reg_data2[i] = 0;
      }
      count2 = 0;
      break;
      default:
      Serial.print("Error Receiving data!");
      count2 = 0;
      break;

    }
  }
  if (data.pressure != 0 && data2.pressure != 0){
    float diff_pressure = data2.pressure - data.pressure;
    Serial.print("Diff Pressure is: ");
    Serial.println(diff_pressure);
    data.pressure = 0;
    data2.pressure = 0;  
  }
}
