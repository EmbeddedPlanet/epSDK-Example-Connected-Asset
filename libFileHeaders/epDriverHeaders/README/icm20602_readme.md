# ICM-20602 Driver

This is a driver for the InvenSense/TDK ICM-20602 6-axis IMU. It utilizes the nRF SDK for use with the nRF52XXX series SoCs.

## Contents
**icm20602.h** - Driver header.

## Set up
1. Include **icm20602.h** in the necessary source files
2. Add the include path for **icm20602.h** to your Makefile
3. Add the path for the associated static library to your Makefile

## Use
Detailed use can be found in the header file. However, the most simplified steps are as follows:

```C++
/* Create empty ICM20602 struct */
ICM20602 icm;

/* Initialize icm struct with the proper settings. In this case, the twi0 interface that we configured earlier, at device address 0x68. */
icm20602_init(&icm, ICM20602_ADDR_LOW, twi);

/* The ICM defaults to 2G and 250dps scale. We can change this using the icm20602_set_scale function. */
icm20602_set_scale(&icm, ICM_ACCEL_8G, ICM_GYRO_1000DPS);

/* Check to see if data is ready. If it is, receive and print the data */
if(icm20602_data_ready(&icm, &err)){

    //Retrieve data from sensor into data struct
    icm20602_sensor_data icm_data = icm20602_get_data(&icm, &err); 

    /* Data is automatically converted and scaled */
    DBGI("accel_x: %f\r\n", icm_data.accel_x);
    DBGI("accel_y: %f\r\n", icm_data.accel_y);
    DBGI("accel_z: %f\r\n", icm_data.accel_z);
    DBGI("gyro_x: %f\r\n", icm_data.gyro_x);
    DBGI("gyro_y: %f\r\n", icm_data.gyro_y);
    DBGI("gyro_z: %f\r\n", icm_data.gyro_z);
    DBGI("temp: %f\r\n\r\n", icm_data.temp);
}
```

## Versions
- V1.0.0 Initial Release
