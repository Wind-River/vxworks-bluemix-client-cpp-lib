/* 
 * Copyright (c) 2016, Wind River Systems, Inc. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, are 
 * permitted provided that the following conditions are met: 
 * 
 * 1) Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer. 
 * 
 * 2) Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation and/or 
 * other materials provided with the distribution. 
 * 
 * 3) Neither the name of Wind River Systems nor the names of its contributors may be 
 * used to endorse or promote products derived from this software without specific 
 * prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */
 
#include <Arduino-lite.h>
#include "BluemixVxWorksClient.h"
#include "wiring-lite/vxWorks/common/serial/serial.h"
#include <Extras.h>

/* ******************************************************************* */
/* Add these defines to enable debug and/or error message printing
 * Messages take the form...
 *        DEBUG_BLUEMIX("Debug data is %d\n", value);
 *        ERROR_BLUEMIX("Error message: %.2d\n", floatvalue);
*/
#define ENABLE_ERROR_BLUEMIX_VX
#define ENABLE_DEBUG_BLUEMIX_VX

#ifdef ENABLE_ERROR_BLUEMIX_VX
#define ERROR_BLUEMIX_VX(...) fprintf (stderr, __VA_ARGS__)
#else
#define ERROR_BLUEMIX_VX(...)
#endif
#ifdef ENABLE_DEBUG_BLUEMIX_VX
#define DEBUG_BLUEMIX_VX(...) fprintf (stderr, __VA_ARGS__)
#else
#define DEBUG_BLUEMIX_VX(...)
#endif
/* ******************************************************************* */


/*******************************************************************************
 * BluemixVxWorksClientClass library constructor (Registered mode)
 *
 * Arguments: id - Bluemix device ID
 *            type - Bluemix device type
 *            org - Bluemix organisation code
 *            token - Bluemix token
 *            callback - callback routine for returned commands
 * Returns:  none
 */
BluemixVxWorksClientClass::BluemixVxWorksClientClass(char* id, char* type, char* org, char* token, void (*callback)(char*, char*, void*)) 
    : BluemixClientClass(id, type, org, token, callback) {
    
	// Set the our function to be called when we received messages we subscribed to
	devCB = callback;
}

/*******************************************************************************
 * BluemixVxWorksClientClass library constructor (Quickstart mode)
 *
 * Arguments: id - Bluemix device ID
 * Returns:  none
 */
BluemixVxWorksClientClass::BluemixVxWorksClientClass(char* id) : BluemixClientClass(id) {
   
	// Callback not used for QS mode
	devCB = NULL;
}

/*******************************************************************************
 * connectToBluemix - connect to the IBM BluemixClientClass cloud (in either 
 *         Quickstart or Registered mode).
 *
 * Arguments: id[] - id string, not used for VxWorks
 *
 * Returns:  boolean, SUCCESS or FAILURE
 */
boolean BluemixVxWorksClientClass::connectToBluemix (char id[]) {
	int rc = -1;
	char macAddress[30];

	/* if deviceID is either empty or null then find MAC address */
	if((strlen(deviceID) < 2) || (deviceID == NULL))
	{
		if((rc = getMacAddress(macAddress)) != SUCCESS)
		{
			ERROR_BLUEMIX_VX("[ERROR] bluemix::getMacAddress - Failed, returned %d.\n Quitting..\n", rc);
			return FAILURE;
		}
		deviceID = macAddress;
		DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect - MAC Address found = %s\n", deviceID);
	}

	if (quickstart) 		/* in Quickstart mode... */
    {
		DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect called in Quickstart mode with deviceID = %s\n", deviceID);
	    rc = initialize(&client, (char *)"quickstart", (char *)"iotsample", deviceID, NULL, NULL);
	    DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect - quickstart mode initialised\n");
    }
    else
    {					// in registered mode...
		DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect called in Registered mode with %s\n", deviceID);
        rc = initialize(&client, deviceOrg, deviceType, deviceID, (char *)"token", deviceToken);
	    DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect - registered mode initialised\n");
		if( devCB != NULL )
		{
			setCommandHandler(&client, devCB);
		    DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect - callback handler installed\n");
		}
    }

	if(rc != SUCCESS)
	{
	    ERROR_BLUEMIX_VX("[ERROR] bluemix::connect - Initialize returned rc = %d.\n Quitting..\n", rc);
		return FAILURE;
	}

	rc = connectiotf(&client);

	if(rc != SUCCESS)
	{
	    ERROR_BLUEMIX_VX("[ERROR] bluemix::connect - Connection returned rc = %d.\n Quitting..\n", rc);
		return FAILURE;
	}

	connected = TRUE;
    DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect - Connection Successful. Press Ctrl+C to quit\n");
	DEBUG_BLUEMIX_VX("[DEBUG] bluemix::connect - View the visualization at https://quickstart.internetofthings.ibmcloud.com/#/device/%s\n", deviceID);
	return SUCCESS;
}

/*******************************************************************************
 * sendToBluemix - send data payload to IBM BluemixClientClass cloud
 *
 * Arguments: payload[] - payload string
 * Returns:  boolean, SUCCESS or FAILURE
 */
boolean BluemixVxWorksClientClass::sendToBluemix(char payload[])  {
	int rc;

	rc = publishEvent(&client, (char *)"status", (char *)"json", (unsigned char *)payload , QOS0);
	if(rc != SUCCESS)
	{
	    ERROR_BLUEMIX_VX("[ERROR] bluemix::send - Publishing event status failed with rc = %d\n", rc);
	    return FALSE;
	} else {
    	yield(&client, 500);
    	return TRUE;
	}
}
