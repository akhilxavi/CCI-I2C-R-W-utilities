/* cci_write: command line utility for monoSensor sensor
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
	struct sensor_ctrl_t *ctrl ;
	uint16_t r_addr = 0;
	struct msm_camera_i2c_read_config *reg_settings = NULL;
    	struct msm_camera_i2c_read_config *monoSensor_write_settings;
	cdata = malloc(sizeof(struct sensorb_cfg_data));
    	monoSensor_write_settings = malloc(sizeof(struct msm_camera_i2c_read_config));
	if (!cdata || !monoSensor_write_settings) {
		printf("malloc failed\n");
		goto ERROR;         
	}

	/*Checking required  parameters */
	if(argc < 4){
		printf("Enter all the required parameters in the below format:\n");
        printf("./cci_write <addr> <data> <no of bytes> \n");
        printf("\t\tExample: ./cci_write 0x0004 0x8004 2\t\t\n");
		goto ERROR; 
	}

	/* Open subdev */
	fd = open("/dev/v4l-subdev16", O_RDWR);
        if (fd < 0) {
            printf("Device open /dev/v4l-subdev16 failed \n");
            goto ERROR;
        }

	monoSensor_write_settings->reg_addr = parse_i2c_address(argv[1]);
	if(monoSensor_write_settings->reg_addr >=0) {
		printf("Register to write = 0x%x\n",monoSensor_write_settings->reg_addr);
	} else {
		printf("Enter Valid Register address !!!\n");
		goto ERROR;
	}

	monoSensor_write_settings->reg_data = parse_i2c_address(argv[2]);
	if(monoSensor_write_settings->reg_data >0) {
		printf("Data = 0x%x \n",monoSensor_write_settings->reg_data);
	} else {
		printf("Enter valid data to write !!!\n");
		goto ERROR;
	}

	monoSensor_write_settings->data_type = parse_i2c_type(argv[3]);
	if(monoSensor_write_settings->data_type ==1 || monoSensor_write_settings->data_type  ==2) {
		printf("No. of Bytes = %d\n",monoSensor_write_settings->data_type);
	} else {
		printf("Invalid No of bytes Enter 1/2 !!!\n");
		goto ERROR;
	}

	reg_settings = monoSensor_write_settings;

	cdata->cfgtype = CFG_WRITE_MONO_REG;
   	cdata->cfg.setting = reg_settings;


	if (ioctl(fd, VIDIOC_MSM_SENSOR_CFG, cdata) < 0) {
		printf(" IOCTL call failed !!!\n");
        goto ERROR;
	}

	printf("\t\t\t!!!! WRITE SUCCESS !!!!\t\t\t\n");

	return 0;

ERROR:
	printf(" Program failed .. Exiting !!!\n");
    return 0;	
}
