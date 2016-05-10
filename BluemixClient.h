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

/* 
 * DESCRIPTION
 *  - BluemixClient.h
 * Defines abstract BluemixClientClass
 */

#ifndef _BLUEMIX_CLIENT_H_
#define _BLUEMIX_CLIENT_H_

typedef bool boolean;
typedef unsigned char byte;

class Datapoint {
    public:
        Datapoint(const char name[], float* data);
        Datapoint(const char name[], int* data);   
        Datapoint(const char name[], char* data);
        void show();
        const char* n;    /* pty */
        void*  d;
        enum {
            eFloat,
            eInt,
            eString
        } t;
 };

class BluemixClientClass {
    public:
        BluemixClientClass(char* id);
        BluemixClientClass(char* id, char* type, char* org, char* token, void (*callback)(char*, char*, void*));
        bool isConnected();
        boolean connect();
        boolean publish(Datapoint* d[], int num);

    protected:
        void formatPayload(char payload1[], Datapoint* d [], int num);
        virtual boolean connectToBluemix(char id[]) = 0;
        virtual boolean sendToBluemix(char payload[]) = 0;
        static const char subTopic[];
        static const char pubTopic[];
        bool quickstart;
        bool connected;
        char* deviceID;
        char* deviceType;
        char* deviceOrg;
        char* deviceToken;
        
    private: 
        enum {
            eMaxPayloadSize = 255
        };
        char payload[eMaxPayloadSize];    
};

#endif /* _BLUEMIX_CLIENT_H_ */