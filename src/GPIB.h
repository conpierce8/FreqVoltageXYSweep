#ifndef GPIB_H
#define GPIB_H

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <exception>

#include <ni4882.h>
//using namespace std;  //Removed 2/5/2018 by Connor

#define NUM_DEVICES 31
#define BUF_SIZE 1024

class DisconnectedException: public std::exception {
	char message[80];
	
public:
	DisconnectedException(std::string msg) {
		strcpy(message, msg.c_str());
	}
	
	const char* what() {
		return message;
	}
};

class GPIBInterface {
    int id;
    char *command;
    char deviceDesc[BUF_SIZE];
    int num_listeners;
    Addr4882_t instruments[NUM_DEVICES], result[NUM_DEVICES];

    void gpib_error(int errnum, std::string errmsg) {
        std::cout << "Error #" << errnum << ": " << errmsg << std::endl;
        ibonl(0,0); //take the board offline
//        exit(1); //terminate program  //REMOVED BY CONNOR 2/5/2018
    }

public:
    GPIBInterface(int idd) {
        id = idd;
        char buffer[BUF_SIZE];
        int i; //the number of listeners on the bus
        unsigned short address; //the address of a listener

        SendIFC(id);
        //check for an error
        if(ibsta & ERR) {
        	gpib_error(1, "GPIB-Interface: Could not send IFC");
        	throw DisconnectedException("GPIB disconnected; error in SendIFC");
        }

        for(i = 0; i < NUM_DEVICES - 1; i++) {
        	instruments[i] = i + 1;
        }
        instruments[NUM_DEVICES - 1] = NOADDR;

        FindLstn(id, instruments, result, NUM_DEVICES);

        //check for error
        if(ibsta & ERR) {
        	gpib_error(2, "GPIB-Interface: Could not find listeners");
        	throw DisconnectedException("GPIB disconnected; error in FindLstn");
        }

        num_listeners = ibcnt;
        result[num_listeners] = NOADDR;

        std::cout << "GPIB-Interface: Found " << num_listeners << " GPIB devices." << std::endl;

        char *sIDN=new char[11];

        sIDN[0] = '*';
        sIDN[1] = 'I';
        sIDN[2] = 'D';
        sIDN[3] = 'N';
        sIDN[4] = '?';
        sIDN[5] = '\0';

        SendList(id, result, sIDN, 5L, NLend);

        if (ibsta & ERR) {
        	gpib_error(3, "GPIB-Interface: Could not send *IDN? to devices");
        }

        std::cout << "GPIB-Interface: Device list:\n";
        std::cout << "----------------------------------------\n";
        for(int i = 0; i < num_listeners; i++) {
            Receive(id, result[i], buffer, BUF_SIZE, STOPend);

            if(ibsta & ERR) {
            	gpib_error(4, "GPIB-Interface: Could not receive from device");
            }

            address = GetPAD(result[i]);

            buffer[ibcnt] = '\0';
            
            strcpy(deviceDesc, buffer);

            //Now output the results
            std::cout << ibcnt << std::endl;
            std::cout << "#" << i+1 << " ADDRESS: " <<  address << " ID: " << buffer;
        }
    }

	/* void getDeviceDesc(char* buffer, int len)
     * 
     * Copies the name of the active device into the character array "buffer",
     * never exceeding the "buffer" length "len". Returns the entire device 
	 * name plus a null character if "len"-1 is greater than the length of the
	 * device name, or the first "len"-1 characters of the device name plus a 
	 * null character if the length of the device name is greater than "len"-1.
	 */
    void getDeviceDesc(char* buffer, int len) {
    	strncpy(buffer, deviceDesc, len);
    	buffer[len-1] = '\0';
	}
	
    Addr4882_t DeviceAddress(unsigned short GPIBaddress) {
        for(int i=0;i<num_listeners;i++) {
            if(GetPAD(result[i]) == GPIBaddress) {
                return result[i];
            }
        }
        std::cout << "GPIB-Interface: Warining: There is no GPIB device at address " << GPIBaddress << "\n";
        return NOADDR;
    }

    void disconnect_gpib() {
        ibonl(0,0);
    }

    void send_command(Addr4882_t address, char *command) {
        Send(id, address, command, strlen(command), NLend);
    }

    double numerical_response_command(Addr4882_t address, char *command) {
        char *result = new char[1024];
        Send(id, address, command, strlen(command), NLend);
        Receive(id, address, result, 1024, STOPend);
        result[ibcnt]='\0';
        double numval = atof(result);
        delete[] result;
        return numval;
    }

    int integer_response_command(Addr4882_t address, char *command) {
        char *result = new char[1024];
        Send(id, address, command, strlen(command), NLend);
        Receive(id, address, result, 1024, STOPend);
        result[ibcnt]='\0';
        int numval = atoi(result);
        delete[] result;
        return numval;
    }
    
    /*
     * Created by Connor Pierce on 7/4/2018.
     *
     * Returns the raw string returned by the 488.2 device.
     */
    void string_response_command(Addr4882_t address, char* command, char* result, int resultLen) {
    	Send(id, address, command, strlen(command), NLend);
//    	std::cout << "string_response_command(): command = " << command << std::endl;
//    	std::cout << "  string_response_command(): strlen(result) = " << strlen(result) << std::endl;
    	Receive(id, address, result, resultLen, STOPend);
    	result[ibcnt] = '\0';
//    	std::cout << "  string_response_command(): result = " << result << std::endl;
	}

};

#endif
