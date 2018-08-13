/* cci_read: command line utility for monoSensor sensor
 *	Copyright (c) 2017, Inforce Computing Inc.
 *      
 *	Author : Akhil Xavier <akhilxavier@inforcecomputing.com>
 *
 *  This program "WriteMac.py" is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; only version 2 of the License .
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <stdio.h>
#include <dlfcn.h>
#include <math.h>
#include <asm-generic/errno-base.h>
#include <poll.h>
#include "sensor.h"
#include "modules.h"
#include "mct_pipeline.h"
#include "mct_module.h"
#include "module_sensor.h"
#include "mct_stream.h"
#include <media/msm_cam_sensor.h>

#define H2C_DATA_8BIT	1
#define H2C_DATA_16BIT	2
#define H2C_DATA_32BIT	4

enum msm_camera_i2c_cmd_type {
	MSM_CAMERA_I2C_CMD_WRITE,
	MSM_CAMERA_I2C_CMD_POLL,
};

uint32_t parse_i2c_address(const char *address_arg)
{
        long address;
        char *end;

        address = strtol(address_arg, &end, 0);
            if (*end || !*address_arg) {
                printf("Error:address is not a number!\n");
                return -1;
            }

        return address;
}

uint32_t parse_i2c_data(const char *address_arg)
{
        long data;
        char *end;

        data = strtol(address_arg, &end, 0);
            if (*end || !*address_arg) {
                printf("Error:data entered is not valid !!!!\n");
                return -1;
            }
        return data;
}

uint32_t parse_i2c_type(const char *address_arg)
{
        long dt;
        char *end;

        dt = strtol(address_arg, &end, 0);
            if (*end || !*address_arg) {
                printf("Error:data type invalid!!!\n");
                return -1;
            }
        return dt;
}

int main(int argc, char *argv[])
{
	int fd;
	struct sensorb_cfg_data *cdata;
	struct sensor_ctrl_t *ctrl;
	uint16_t r_addr = 0;
    	struct msm_camera_i2c_read_config *monoSensor_read_settings = NULL;
	cdata = malloc(sizeof(struct sensorb_cfg_data));
    	monoSensor_read_settings = malloc(sizeof(struct msm_camera_i2c_read_config));
	if (!cdata || !monoSensor_read_settings) {
		printf("malloc failed\n");
		goto ERROR;         
	}

	/*Checking required  parameters */
	if(argc < 3){
		printf("Enter all the required parameters in the below format:\n");
        printf("./cci_read <slavaddr> <addr> <no of bytes> \n");
       printf("\t\tExample: ./cci_read 0x0004 2\t\t\n");
		goto ERROR; 
	}

	/* Open subdev */
	fd = open("/dev/v4l-subdev16", O_RDWR);
        if (fd < 0) {
            printf("Device open /dev/v4l-subdev16 failed \n");
            goto ERROR;
        }

        monoSensor_read_settings->reg_addr = parse_i2c_address(argv[1]);
        if(monoSensor_read_settings->reg_addr >=0) {
                printf("Register to read = 0x%x\n",monoSensor_read_settings->reg_addr);
        } else {
                printf("Enter Valid Register address !!!\n");
                goto ERROR;
        }

	monoSensor_read_settings->data_type = parse_i2c_type(argv[2]);
	if(monoSensor_read_settings->data_type == 1 || monoSensor_read_settings->data_type == 2) {
		printf("No. of Bytes = %d\n",monoSensor_read_settings->data_type);
	} else {
		printf("Invalid No of bytes Enter 1/2 !!!\n");
		goto ERROR;
	}

	cdata->cfgtype = CFG_READ_MONO_REG;
   	cdata->cfg.setting = monoSensor_read_settings;

	if (ioctl(fd, VIDIOC_MSM_SENSOR_CFG, cdata) < 0) {
		printf(" IOCTL call failed !!!\n");
        goto ERROR;
	}

	printf("\t\t\t!!!! READ SUCCESS - Read Value = 0x%x !!!!\t\t\t\n", monoSensor_read_settings->data);

	return 0;

ERROR:
	printf(" Program failed .. Exiting !!!\n");
    return 0;	
}
