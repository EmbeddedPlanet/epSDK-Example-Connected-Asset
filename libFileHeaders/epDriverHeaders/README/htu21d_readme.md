# ICM-20602 Driver

This is a driver for the Si7021 & HTU21d temperature and humidity sensors. It utilizes the nRF SDK for use with the nRF52XXX series SoCs.

## Contents
**htu21d.h** - Driver header. 

## Set up
1. Include **htu21d.h** in the necessary source files
2. Add the include path for **htu21d.h** to your Makefile
3. Add the path for the associated static library to your Makefile

## Use
Detailed use can be found in the header file. However, the most simplified steps are as follows:

```C++

/* Initialize htu21d with the proper settings. In this case, the twi0 interface that we configured earlier. */
htu21_init(twi);

/* Check if sensor is connected */
err = htu21_is_connected();
if(err != NRFX_SUCCESS){
	DBGE("Si7021 Not Connected!");
}

/* Gather temp and humidity */
float si_temp, si_hum;
htu21_read_temperature_and_relative_humidity(&si_temp, &si_hum);
```

## Versions
- V1.0.0 Initial Release
