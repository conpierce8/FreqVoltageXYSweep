// SR830.h
// encoding: utf-8
//
// High-level functions for communicating with lock-in amplifier.
//
// Original author:  unknown
// Creation date:    unknown
// Adapted by:       Connor D. Pierce
// Last Modified:    2022-09-15 14:47:31
//
// Copyright (C) 2018-2022 Connor D. Pierce
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// SPDX-License-Identifier: MIT


#ifndef SR830_H
#define SR830_H

#include "GPIB.h"
#include "LockinSettings.h"
#include <string>

class SR830 { // Changed from struct to class

public:
    GPIBInterface *gInterface;
	Addr4882_t address;
	LockinSettings settings;
	bool phaseAccessible;

	SR830(GPIBInterface *interface1, int address1):
			settings(interface1) {
        address = interface1->DeviceAddress(address1);
        gInterface = interface1;
        settings.queryAllOptions(address);
        int bufLen = 60;
		char buf[bufLen];
		gInterface->getDeviceDesc(buf, bufLen);
		if(strstr(buf, "SR830")) {
			phaseAccessible = true;
		} else {
			phaseAccessible = false;
		}
    }
    
    // ADDED V1.0.8 {
    std::string get_device_description() {
    	char desc[BUF_SIZE];
    	gInterface->getDeviceDesc(desc, BUF_SIZE);
    	return std::string(desc);
	}
	// } ADDED V1.0.8

    double get_reference_amplitude() {
//    	char command[7];
//    	strcpy(command, "SLVL?\0");
//        return gInterface->numerical_response_command(address, command);
        return settings.get("Sine Output Amplitude").dblVal1;
    }

    void set_reference_amplitude(double ampl) {
//        char *ph = new char[256];
//        sprintf(ph,"SLVL %.6f\0",ampl);
//        std::cout << ph << "\n";
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Sine Output Amplitude", ampl);
    }

    int get_harmonic() {
//    	char command[7];
//    	strcpy(command, "HARM?\0");
//        return gInterface->integer_response_command(address, command);
        return settings.get("Detection Harmonic").intVal1;
    }

    void set_harmonic(int harmonic) {
//        char *ph = new char[256];
//        sprintf(ph,"HARM %d\0",harmonic);
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Detection Harmonic", harmonic);
    }

    double get_reference_phase() {
//    	char command[7];
//    	strcpy(command, "PHAS?\0");
//        return gInterface->numerical_response_command(address, command);
        return settings.get("Reference Phase Shift").dblVal1;
	}

	void set_reference_phase(double phas) {
		if(phas < 180 || phas > 180) {
			std::cout << "Note: phase " << phas << " will be wrapped to the range"
					<< "-180 to +180";
		}
//        char *ph = new char[256];
//        sprintf(ph,"PHAS %.6f\0",phas);
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Reference Phase Shift", phas);
	}

	double get_frequency() {
//    	char command[7];
//    	strcpy(command, "FREQ?\0");
//        return gInterface->numerical_response_command(address, command);
        return settings.get("Reference Frequency").dblVal1;
	}

	void set_frequency(double freq) {
//        char *ph = new char[256];
//        sprintf(ph,"FREQ %.6f\0",freq);
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Reference Frequency", freq);
	}

    void ground_shield() {
//        char *ph = new char[256];
//        sprintf(ph,"IGND 1\0");
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Input Shield Grounding", 1);
	}

    void float_shield() {
//        char *ph = new char[256];
//        sprintf(ph,"IGND 0\0");
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Input Shield Grounding", 0);
	}

    void internal_reference() {
//        char *ph = new char[256];
//        sprintf(ph,"FMOD 1\0");
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Reference Source", 1);
	}

    void AC_couple() {
//        char *ph = new char[256];
//        sprintf(ph,"ICPL 0\0");
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Input Coupling", 0);
	}

    void DC_couple() {
//        char *ph = new char[256];
//        sprintf(ph,"ICPL 1\0");
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Input Coupling", 1);
	}

	int get_time_constant() {
//    	char command[7];
//    	strcpy(command, "OFLT?\0");
//        return gInterface->integer_response_command(address, command);
        return settings.get("Time Constant").intVal1;
	}

	void set_time_constant(int tc) {
        if(tc >= 0 && tc <= 19) {
//            char *ph = new char[256];
//            sprintf(ph,"OFLT %d\0",tc);
//            gInterface->send_command(address,ph);
//            delete[] ph;
            settings.set(address, "Time Constant", tc);
        } else {
//        	Changed on 7/5/2018 by Connor Pierce
//            std::cout << "SR830: Invalid lowpass filter order.\n";
            std::cout << "SR830: Invalid lowpass filter time constant.\n";
        }
	}
	
	int get_sensitivity() {
        return settings.get("Sensitivity").intVal1;
	}

	void set_sensitivity(int sens) {
        if(sens >= 0 && sens <= 26) {
            settings.set(address, "Sensitivity", sens);
        } else {
            std::cout << "SR830: Invalid lowpass filter sensitivity.\n";
        }
	}

	int get_order() {
//    	char command[7];
//    	strcpy(command, "OFSL?\0");
//        return gInterface->integer_response_command(address, command)+1;
        return settings.get("Low Pass Filter Slope").intVal1;
	}

	int set_order(int order) { //CONNOR, 2/5/2018: changed from return type double to int
        if(order >= 1 && order <= 4) {
//            char *ph = new char[256];
//            sprintf(ph,"OFSL %d\0",order-1);
//            gInterface->send_command(address,ph);
//            delete[] ph;
            settings.set(address, "Low Pass Filter Slope", order-1);
            return 1;
        } else {
            std::cout << "SR830: Invalid lowpass filter order.\n";
            return 0;
        }
	}

	double get_amplitude() {
    	char command[8];
    	strcpy(command, "OUTP?3\0");
        return gInterface->numerical_response_command(address, command);
	}
	
	// ADDED IN V1.0.12 {
	void get_AmplPhase(double &ampl, double &phs) {
		if(phaseAccessible) {
			char command[9];
			strcpy(command, "SNAP?3,4\0");
			char result[80];
			gInterface->string_response_command(address, command, result, 80);
			std::string response(result);
			int commaLoc = response.find(',');
			ampl = atof(response.substr(0,commaLoc).c_str());
			phs  = atof(response.substr(commaLoc+1).c_str());
		} else {
			ampl = get_amplitude();
			phs  = 0;
		}
	}
	// } ADDED IN V1.0.12
	
	int getIndex(char *string, int len, char c) {
		for(int i = 0; i<len; i++) {
			if(string[i] == c) {
				return i;
			}
		}
		return -1;
	}
	
	bool isPhaseAccessible() {
		return phaseAccessible;
	}

	double get_phase() {
		if(phaseAccessible) {
			char command[8];
	    	strcpy(command, "OUTP?4\0");
	        return gInterface->numerical_response_command(address, command);
		} else {
			std::cout << "SR830: phase is not accessible" << std::endl;
			return 0;
		}
	}

	double get_X() {
    	char command[8];
    	strcpy(command, "OUTP?1\0");
        return gInterface->numerical_response_command(address, command);
	}

	double get_Y() {
    	char command[8];
    	strcpy(command, "OUTP?2\0");
        return gInterface->numerical_response_command(address, command);
	}

    void set_auxout1(double xvol) {
        //std::cout << "SR830: WARNING: Trying to call an unlimited output voltage setting function\n";
        //std::cout << "SR830: Call ignored.\n";
        //return;
//        char *ph = new char[256];
//        sprintf(ph,"AUXV 1 , %.6f\0",xvol);
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Aux Out 1", xvol);
	}

    void set_auxout2(double yvol) {
        //std::cout << "SR830: WARNING: Trying to call an unlimited output voltage setting function\n";
        //std::cout << "SR830: Call ignored.\n";
        //return;
//        char *ph = new char[256];
//        sprintf(ph,"AUXV 2 , %.6f\0",yvol);
//        gInterface->send_command(address,ph);
//        delete[] ph;
        settings.set(address, "Aux Out 2", yvol);
	}

//    void set_06auxout1(double voltage) {
//        if(voltage < 0 || voltage > 6) {
//            std::cout << "SR830: WARNING: Trying to set a negative or too high voltage\n";
//            return;
//        }
//        char *ph = new char[256];
//        sprintf(ph,"AUXV 1 , %.6f\0",voltage);
//        gInterface->send_command(address,ph);
//        delete[] ph;
//	}

    double get_auxin1() {
    	char command[8];
    	strcpy(command, "OAUX?1\0");
        return gInterface->numerical_response_command(address, command);
	}

    double get_auxin2() {
    	char command[8];
    	strcpy(command, "OAUX?2\0");
        return gInterface->numerical_response_command(address, command);
	}

};

#endif
