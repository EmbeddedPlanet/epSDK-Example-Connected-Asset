# MC3479 Driver

This is a driver for the Memsic 3-axis Accelerometer. It utilizes the nRF SDK for use with the nRF52XXX series SoCs.

## Contents
**mc3479.h** - Driver header.

## Set up
1. Include **mc3479.h** in the necessary source files
2. Add the include path for **mc3479.h** to your Makefile
3. Add the path for the associated static library to your Makefile

## Use
Detailed use can be found in the header file. However, the most simplified steps are as follows:

```C++
/* Create empty MC3479 struct */
MC3479 mc;

/* Initialize mc struct with the proper settings. In this case, the twi0 interface that we configured earlier, at device address 76. */
mc3479_init(&mc, MC3479_ADDR_LOW, twi);

/* The MC defaults to 2G. We can change this using the mc3479_set_scale function. */
mc3479_set_scale(&mc, MC_ACCEL_2G);

/* Check to see if data is ready. If it is, receive and print the data */
if(mc3479_data_ready(&mc, &err)){

    //Retrieve data from sensor into data struct
    mc3479_sensor_data mc_data = mc3479_get_data(&mc, &err); 

    /* Data is automatically converted and scaled */
    DBGI("accel_x: %fmg\r\n", mc_data.accel_x);
    DBGI("accel_y: %fmg\r\n", mc_data.accel_y);
    DBGI("accel_z: %fmg\r\n", mc_data.accel_z);
    DBGI("tilt: %fdeg\r\n", mc_data.tilt);
}
```

## Versions
- V1.0.0 Initial Release
