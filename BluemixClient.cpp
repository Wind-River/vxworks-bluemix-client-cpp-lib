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
 
#include "BluemixClient.h"
#include "wiring-lite/vxWorks/common/serial/serial.h"
#include <string.h>

const char BluemixClientClass::pubTopic[] = "iot-2/evt/status/fmt/json";
const char BluemixClientClass::subTopic[] = "iot-2/cmd/+/fmt/json";

Datapoint::Datapoint(const char name[], float* data) : n(name), d((void*)data), t(eFloat) 
{  
}

Datapoint::Datapoint(const char name[], int* data) : n(name), d((void*)data), t(eInt) 
{  
}

Datapoint::Datapoint(const char name[], char* data) : n(name), d((void*)data), t(eString) 
{  
}

/*******************************************************************************
* BluemixClientClass library constructor (Registered mode)
*
* Arguments: none
* Returns:  none
*/
BluemixClientClass::BluemixClientClass(char* id, char* type, char* org, 
     char* token, void (*callback)(char*, char*, void*))  : quickstart(FALSE), 
     deviceID(id), deviceType(type), deviceOrg(org), deviceToken(token) 
{
}

/*******************************************************************************
* BluemixClientClass library constructor (Quickstart mode)
*
* Arguments: none
* Returns:  none
*/
BluemixClientClass::BluemixClientClass(char* id) : quickstart(TRUE), deviceID(id), 
     deviceType((char *)"quickstart"), deviceOrg((char *)"quickstart"), 
	 deviceToken(NULL) 
{
}

/*******************************************************************************
 * connect - connect to the IBM BluemixClientClass cloud (in either Quickstart or
 *           Registered mode). Note that if deviceType (second param) is not
 *           set then device will be connected in Quickstart mode.
 *
 * Arguments: none
 * Returns:  boolean, SUCCESS or FAILURE
 */
boolean BluemixClientClass::connect () {
    
    boolean rc = FALSE;
    char id[strlen("d:")+strlen(deviceOrg)+strlen(":")+strlen(deviceType)+strlen(":")+strlen(deviceID)+1];
  
    if (!isConnected()) {
		if (connectToBluemix(id)) {
			rc = TRUE;
        }
    }
	return (rc);
}

/*******************************************************************************
 * isConnected - return Bluemix connection status.
 *
 * Arguments: none
 * Returns:  boolean, connected (1) or unconnected (0)
 */
bool BluemixClientClass::isConnected() {
    return(connected);
}

/*******************************************************************************
 * publish - publish data payload to IBM BluemixClientClass cloud
 *
 * Arguments: d - datapoint array
 *            num - number of datapoints
 * Returns:  boolean, SUCCESS or FAILURE
 */
boolean BluemixClientClass::publish(Datapoint* d [], int num)  {
    boolean rc = FALSE;
    if (isConnected()) {
        formatPayload(payload, d, num);
        if (sendToBluemix(payload)) {
           rc = TRUE;
        }    
    }
    return 0;
}

/*******************************************************************************
 * formatPayload - format the datapoint payload into json format ready for
 *              transmission to IBM Watson IoT cloud
 *
 * Arguments: payload1 - char string to store formatted datapoints
 *            d - an array of datapoints
 *            num - number of datapoints
 * Returns:   none
 */
void BluemixClientClass::formatPayload(char payload1[], Datapoint* d [], int num)  {
    int index = 0;
    Datapoint* datapoint;
    
    sprintf(payload1,"{\"d\":{");
    index = strlen(payload1);    

    for (int i=0; i<num; i++) { 
        datapoint = d[i];
        switch (datapoint->t) {
        case Datapoint::eInt:
            sprintf(&payload1[index], "\"%s\":%d", datapoint->n, *(int*)datapoint->d);
        break; 
        case Datapoint::eFloat:
            sprintf(&payload1[index], "\"%s\":%0.2f", datapoint->n, *(float*)datapoint->d);
        break;
        case Datapoint::eString:
            sprintf(&payload1[index], "\"%s\":\"%s\"",datapoint->n, (char*)datapoint->d);
        break;        
        }
        
        index = strlen(payload1);    

        if (i < (num-1)) {
            sprintf(&payload1[index], ",");
            index = strlen(payload1);    
        }
 
    }
    sprintf(&payload1[index],"}}");
} 
