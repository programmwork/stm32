/*
 * sensor_process.h
 *
 *  Created on: 2017Äê10ÔÂ28ÈÕ
 *      Author: lenovo
 */

#ifndef _SENSOR_PROCESS_H_
#define _SENSOR_PROCESS_H_

void uartprocess_task( void *pvParameters );
void check_event_sample_task( void *pvParameters );
void readdata_task(void *pvParameters);
void time_task(void *pvParameters);
void autosend();


void Self_test();

#endif /* _SENSOR_PROCESS_H_ */
