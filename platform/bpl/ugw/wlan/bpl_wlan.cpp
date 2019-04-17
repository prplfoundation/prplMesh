/*
 * INTEL CONFIDENTIAL
 * Copyright 2018 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#include "bpl_wlan.h"
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "mapf/common/logger.h"

using namespace mapf;
using namespace std;

#define CONFIG_OUT_SUCCESS_STATUS "wlan_configuration_status=success"
#define CONFIG_OUT_FAILURE_STATUS "wlan_configuration_status=FAILURE"
char BASE_CTRL_PATH[] = "/opt/lantiq/wave/scripts/fapi_wlan_wave_bpl_set";
char CONFIG_OUT_PATH[] = "/tmp/wlan_wave/fapi_wlan_wave_out.conf";

static void get_return_status(int *return_status)
{
	FILE * fp;
	char * line = NULL;
	int statusFound = -1;
	unsigned int len = 0;

	if(*return_status == -1) {
		MAPF_ERR("Script execution failed.\n");
		return;
	}

	fp = fopen(CONFIG_OUT_PATH, "r");
	if(fp == NULL) {
		MAPF_ERR("parse_config_out: FAILED to open configuration file \n");
		*return_status = -1;
		return;
	}

	while(getline(&line, &len, fp) != -1) {
		if(!strncmp(line, CONFIG_OUT_SUCCESS_STATUS, sizeof(CONFIG_OUT_SUCCESS_STATUS))) {
			MAPF_INFO("parse_config_out, End of configuration output file with success status\n");
			statusFound = 0;
			*return_status = 0;
			break;
		}
		if(!strncmp(line, CONFIG_OUT_FAILURE_STATUS, sizeof(CONFIG_OUT_FAILURE_STATUS))) {
			MAPF_ERR("parse_config_out, End of configuration output file failure status\n");
			statusFound = 0;
			*return_status = -1;
			break;
		}
	}

	if(statusFound == -1) {
		MAPF_INFO("CONFIG_OUT_PATH file end without CONFIG_OUT_STATUS \n");
	}

	if(fp != NULL) {
		fclose(fp);
	}
	if(line) {
		free((void *)line);
	}

	return;
}

static int run_script(string script_cmd)
{
	int return_status = 0;
	FILE *fp = NULL;

	if(script_cmd.length() == 0) {
		return -1;
	}

	fp = popen(script_cmd.c_str(), "r");

	if(fp == NULL) {
		MAPF_ERR("running dispatcher script FAILED\n");
		return -1;
	} else {
		MAPF_INFO("running dispatcher script SUCCEED\n");
		return_status = pclose(fp);
		if(return_status != 0 && return_status != 1) {
			return_status = -1;
		}
	}
	return return_status;
}

int bpl_wlan_ap_start(const char* iface)
{
	int return_status;
	string script_cmd;

	if(iface == NULL) {
		return 0;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " ap_start ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);
	if(return_status == 1) {
		MAPF_INFO("Specified AP is already up : " << iface);
	}
	else {
		// process return status
		get_return_status(&return_status);
	}
	return return_status;
}

int bpl_wlan_ap_stop(const char* iface)
{
	int return_status;
	string script_cmd;

	if(iface == NULL) {
		return 0;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " ap_stop ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);
	if(return_status == 1) {
		MAPF_INFO("Specified AP is already down : " << iface);
	}
	else {
		// process return status
		get_return_status(&return_status);
	}
	return return_status;
}

int bpl_wlan_sta_start(const char* iface)
{
	int return_status;
	string script_cmd;

	if(iface == NULL) {
		return 0;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " sta_start ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_sta_stop(const char* iface)
{
	int return_status;
	string script_cmd;

	if(iface == NULL) {
		return 0;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " sta_stop";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_ap_enable(const char* iface)
{
	string script_cmd;
	int return_status;

	if(iface == NULL) {
		return -1;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " ap_enable ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_ap_disable(const char* iface)
{
	string script_cmd;
	int return_status;

	if(iface == NULL) {
		return -1;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " ap_disable ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_sta_enable(const char* iface)
{
	string script_cmd;
	int return_status;

	if(iface == NULL) {
		return -1;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " sta_enable ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_sta_disable(const char* iface)
{
	string script_cmd;
	int return_status;

	if(iface == NULL) {
		return -1;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " sta_disable ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_ready()
{
	string script_cmd;
	int return_status;

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " ready";

	// execute script
	return_status = run_script(script_cmd);

	return return_status;
}

int bpl_wlan_sta_acs(const char* iface)
{
	string script_cmd;
	int return_status;

	if(iface == NULL) {
		return -1;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " sta_acs ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_restore(const char* iface)
{
	string script_cmd;
	int return_status;

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " restore ";
	if(iface != NULL) {
		script_cmd += iface;
	}

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}

int bpl_wlan_ap_postinit(const char* iface)
{
	int return_status = 0;
	string script_cmd;

	if(iface == NULL) {
		return 0;
	}

	script_cmd = BASE_CTRL_PATH;
	script_cmd += " ap_postinit ";
	script_cmd += iface;

	// execute script
	return_status = run_script(script_cmd);

	// process return status
	get_return_status(&return_status);
	return return_status;
}
