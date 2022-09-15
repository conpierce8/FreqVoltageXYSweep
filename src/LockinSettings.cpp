// LockinSettings.cpp
// encoding: utf-8
//
// Manage lock-in amplifier settings.
//
// Author:   Connor D. Pierce
// Created:  2018-07-02 15:48
// Modified: 2022-09-15 15:04:16
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


#ifndef LOCKINSETTINGS_CPP_
#define LOCKINSETTINGS_CPP_

#include "LockinSettings.h"

// ======= Method Implementation for class Option ==============================

/*
 * CONSTRUCTOR FOR Option
 * 
 * Option::Option()
 * 
 * Empty constructor. The std::string fields `desc`, `assignCmd`, 
 * and `queryCmd`are all initialized to the empty string. No other
 * initialization is required.
 */
Option::Option(): desc(""), assignCmd(""), queryCmd("") {
	// No other initialization needed
}

/* 
 * CONSTRUCTOR FOR Option
 * 
 * Option::Option(std::string, std::string, std::string)
 * 
 * The std::string fields `desc`, `assignCmd`, and `queryCmd` are initialized
 * to the provided values. No other initialization is needed.
 */
Option::Option(std::string desc, std::string assignCmd, std::string queryCmd) :
		desc(desc), 
		assignCmd(assignCmd), 
		queryCmd(queryCmd) {
	// No other initialization needed
}

std::string Option::getDesc() {
	return desc;
}

std::string Option::getAssignCommand() {
	return assignCmd;
}

std::string Option::getQueryCommand() {
	return queryCmd;
}

std::string Option::getDisplayString() {
	std::ostringstream oss;
	int lastParam = params.size()-1;
	int currParam = 0;
	for(std::list<Parameter*>::iterator i = params.begin(); i != params.end(); i++) {
		oss << desc;
		oss << (*i)->getDisplayString();
		if(currParam != lastParam) {
			oss << std::endl;
		}
		currParam++;
	}
	return oss.str();
}

void Option::addParameter(int index, Parameter* param) {
	std::list<Parameter*>::iterator it = params.begin();
	for(int i = 0; i < index; i++) {
		it++;
	}
	params.insert(it, param);
}

void Option::removeParameter(Parameter* param) {
	params.remove(param);
}

std::list<Parameter*> Option::getParameters() {
	return params;
}

void Option::setValues(std::string val) {
	int count = 0;
	int last = 0;
	while(val.find(",", last) != -1) {
		last = val.find(",", last)+1;
		count++;
	}
	if(params.size() != count+1) {
		std::cout << "INVALID NUMBER OF PARAMETERS SUPPLIED FOR OPTION " << desc << std::endl;
	} else {
		int firstInd = 0;
		int secondInd = 0;
		bool validValues = true;
		for(std::list<Parameter*>::iterator it = params.begin(); it != params.end(); it++) {
			secondInd = val.find(",", firstInd);
			bool isParamValid = (*it)->isValid(val.substr(firstInd, secondInd-firstInd));
			validValues = validValues && isParamValid;
			firstInd = secondInd+1;
		}
		
		firstInd = secondInd = 0;
		for(std::list<Parameter*>::iterator it = params.begin(); it != params.end(); it++) {
			secondInd = val.find(",", firstInd);
			if(validValues) {
				(*it)->setValue(val.substr(firstInd, secondInd-firstInd));
			} else {
				(*LockinSettings::settingsLogger) << "COULD NOT ";
			}
			(*LockinSettings::settingsLogger) << "SET OPTION: " << desc << (*it)->getDisplayString() << std::endl;
			firstInd = secondInd+1;
		}
	}
}

OptionData Option::getValues() {
	OptionData values;
//	values.dblVal1 = 0;
//	values.dblVal2 = 0;
//	values.intVal1 = 0;
//	values.intVal2 = 0;
	for(std::list<Parameter*>::iterator i = params.begin(); i != params.end(); i++) {
		if((*i)->getType() == "int") {
			if(values.intVal1Set) {
				values.intVal2 = (*i)->getValue().intVal1;
				values.intVal2Set = true;
			} else {
				values.intVal1 = (*i)->getValue().intVal1;
				values.intVal1Set = true;
			}
		} else {
			if(values.dblVal1Set) {
				values.dblVal2 = (*i)->getValue().dblVal1;
				values.dblVal2Set = true;
			} else {
				values.dblVal1 = (*i)->getValue().dblVal1;
				values.dblVal1Set = true;
			}
		}
	}
	return values;
}

// ===== Method Implementation for class Parameter =============================
std::string Parameter::getDescription() {
	return desc;
}

std::string Parameter::getType() {
	return type;
}

std::string Parameter::getUnits() {
	return this->units;
}

std::string Parameter::getDisplayString() {
	std::cout << "Warning: getting display string using virtual method of Parameter";
	return std::string("");
}

void Parameter::setValue(std::string val) {
	std::cout << "Warning: setting value using virtual method of Parameter";
}

// ===== Method Implementation for class IntParameter ==========================
IntParameter::IntParameter(const std::string description, const std::string units,
		int minValue, int maxValue):
		minValue(minValue),
		maxValue(maxValue) {
	desc = description;
	this->units = units;
	this->type = "int";
}
	
OptionData IntParameter::getValue() {
	OptionData data;
	data.intVal1 = this->value;
	data.intVal1Set = true;
	return data;
}

void IntParameter::setValue(int val) {
	if(val < minValue || val > maxValue) {
		(*LockinSettings::settingsLogger) << "Could not set value of " << desc << " to " << val << std::endl;
	} else {
		this->value = val;
	}
}

int IntParameter::getMinValue() {
	return this->minValue;
}

int IntParameter::getMaxValue() {
	return this->maxValue;
}

std::string IntParameter::getDisplayString() {
	std::ostringstream s;
	s << desc << "\t" << value << "\t" << units;
	return s.str();
}

void IntParameter::setValue(std::string val) {
    setValue(atoi(val.c_str()));
}

bool IntParameter::isValid(std::string value) {
    int val = atoi(value.c_str());
    return (val >= minValue && val <= maxValue);
}

// ===== Method Implementation for class ListParameter =========================
ListParameter::ListParameter(std::string description, std::string units,
		int minValue, int maxValue): IntParameter(description, units, minValue, maxValue) {
	// No further initialization needed
}

std::string ListParameter::getDisplayString() {
	std::ostringstream s;
	s << desc << "\t" << value << "\t" << displayVals.find(value)->second;
	return s.str();
}

void ListParameter::setDisplayValue(int key, std::string displayVal) {
	displayVals[key] = displayVal;
}

std::map<int, std::string> ListParameter::getDisplayValues() {
	return displayVals;
}

// ===== Method Implementation for class DoubleParameter =======================	
DoubleParameter::DoubleParameter(std::string description, std::string units, 
		double minValue, double maxValue):
		minValue(minValue),
		maxValue(maxValue) {
	desc = description;
	this->units = units;
	this->type = "double";
}
	
OptionData DoubleParameter::getValue() {
	OptionData data;
	data.dblVal1 = this->value;
	data.dblVal1Set = true;
	return data;
}

void DoubleParameter::setValue(double val) {
	if(val < minValue || val > maxValue) {
		(*LockinSettings::settingsLogger) << "Could not set value of " << desc << " to " << val << std::endl;
	} else {
		this->value = val;
	}
}

double DoubleParameter::getMinValue() {
	return this->minValue;
}

double DoubleParameter::getMaxValue() {
	return this->maxValue;
}

std::string DoubleParameter::getDisplayString() {
	std::ostringstream s;
	s << desc << "\t" << value << "\t" << units;
	return s.str();
}

void DoubleParameter::setValue(std::string val) {
	double v = atof(val.c_str());
    this->setValue(v);
}

bool DoubleParameter::isValid(std::string value) {
    double val = atof(value.c_str());
    return (val >= minValue && val <= maxValue);
}

// ===== Method Implementation for class LockinSettings ========================

std::ofstream* LockinSettings::settingsLogger = new std::ofstream("settings_log.txt");

/*
 * Available settings:
 *
 * Reference Phase Shift
 * Reference Source
 * Reference Frequency
 * Reference Trigger
 * Detection Harmonic
 * Sine Output Amplitude
 * Input Configuration
 * Input Shield Grounding
 * Input Coupling
 * Input Line Notch Filter Status
 * Sensitivity
 * Dynamic Reserve Mode
 * Time Constant
 * Low Pass Filter Slope
 * Synchronous Filter Status
 * CH1 display settings
 * CH2 display settings
 * CH1 Output Quantity
 * CH2 Output Quantity
 * X output settings
 * Y output settings
 * R output settings
 * Aux Out 1
 * Aux Out 2
 * Aux Out 3
 * Aux Out 4
 * Data Storage Sample Rate
 * End of Buffer Mode
 * Data Storage Trigger Start Mode
 */
LockinSettings::LockinSettings(GPIBInterface* g) {
	this->g = g;
	Option* o = new Option("", "PHAS %f\0", "PHAS?\0");
	DoubleParameter* d = new DoubleParameter("Reference Phase Shift", "deg", -180, 180);
	o->addParameter(0, d);
	settings["Reference Phase Shift"] = *o; 
	
	o = new Option("", "FMOD %d\0", "FMOD?\0");
	ListParameter* l = new ListParameter("Reference Source", "-", 0, 1);
	l->setDisplayValue(0, "external");
	l->setDisplayValue(1, "internal");
	o->addParameter(0, l);
	settings["Reference Source"] = *o;
	
	o = new Option("", "FREQ %f\0", "FREQ?\0");
	d = new DoubleParameter("Reference Frequency", "Hz", 0.001, 102000);
	o->addParameter(0, d);
	settings["Reference Frequency"] = *o;
	
	o = new Option("", "RSLP %d\0", "RSLP?\0");
	l = new ListParameter("Reference Trigger", "-", 0, 2);
	l->setDisplayValue(0, "sine zero crossing");
	l->setDisplayValue(1, "TTL rising edge");
	l->setDisplayValue(2, "TTL falling edge");
	o->addParameter(0, l);
	settings["Reference Trigger"] = *o;
	
	o = new Option("", "HARM %d\0", "HARM?\0");
	IntParameter* i = new IntParameter("Detection Harmonic", "-", 1, 19999);
	o->addParameter(0, i);
	settings["Detection Harmonic"] = *o;
	
	o = new Option("", "SLVL %f\0", "SLVL?\0");
	d = new DoubleParameter("Sine Output Amplitude", "V", 0.004, 5.0); // FIXED in V1.0.8: max is 5V, not 0.5V
	o->addParameter(0, d);
	settings["Sine Output Amplitude"] = *o;
	
	o = new Option("", "ISRC %d", "ISRC?");
	l = new ListParameter("Input Configuration", "-", 0, 3);
	l->setDisplayValue(0, "A");
	l->setDisplayValue(1, "A-B");
	l->setDisplayValue(2, "I (1 megaohm)");
	l->setDisplayValue(3, "I (100 megaohm)");
	o->addParameter(0, l);
	settings["Input Configuration"] = *o;
	
	o = new Option("", "IGND %d", "IGND?");
	l = new ListParameter("Input Shield Grounding", "-", 0, 1);
	l->setDisplayValue(0, "Float");
	l->setDisplayValue(1, "Ground");
	o->addParameter(0, l);
	settings["Input Shield Grounding"] = *o;
	
	o = new Option("", "ICPL %d", "ICPL?");
	l = new ListParameter("Input Coupling", "-", 0, 1);
	l->setDisplayValue(0, "AC");
	l->setDisplayValue(1, "DC");
	o->addParameter(0, l);
	settings["Input Coupling"] = *o;
	
	o = new Option("", "ILIN %d", "ILIN?");
	l = new ListParameter("Input Line Notch Filter Status", "-", 0, 1);
	l->setDisplayValue(0, "No Filters");
	l->setDisplayValue(1, "Line Notch In");
	l->setDisplayValue(2, "2xLine Notch In");
	l->setDisplayValue(3, "Both Notch Filters In");
	o->addParameter(0, l);
	settings["Input Line Notch Filter Status"] = *o;
	
	o = new Option("", "SENS %d", "SENS?");
	l = new ListParameter("Sensitivity", "-", 0, 26);
	l->setDisplayValue(0, "2 nV (fA)");
	l->setDisplayValue(1, "5 nV (fA)");
	l->setDisplayValue(2, "10 nV (fA)");
	l->setDisplayValue(3, "20 nV (fA)");
	l->setDisplayValue(4, "50 nV (fA)");
	l->setDisplayValue(5, "100 nV (fA)");
	l->setDisplayValue(6, "200 nV (fA)");
	l->setDisplayValue(7, "500 nV (fA)");
	l->setDisplayValue(8, "1 uV (pA)");
	l->setDisplayValue(9, "2 uV (pA)");
	l->setDisplayValue(10, "5 uV (pA)");
	l->setDisplayValue(11, "10 uV (pA)");
	l->setDisplayValue(12, "20 uV (pA)");
	l->setDisplayValue(13, "50 uV (pA)");
	l->setDisplayValue(14, "100 uV (pA)");
	l->setDisplayValue(15, "200 uV (pA)");
	l->setDisplayValue(16, "500 uV (pA)");
	l->setDisplayValue(17, "1 mV (nA)");
	l->setDisplayValue(18, "2 mV (nA)");
	l->setDisplayValue(19, "5 mV (nA)");
	l->setDisplayValue(20, "10 mV (nA)");
	l->setDisplayValue(21, "20 mV (nA)");
	l->setDisplayValue(22, "50 mV (nA)");
	l->setDisplayValue(23, "100 mV (nA)");
	l->setDisplayValue(24, "200 mV (nA)");
	l->setDisplayValue(25, "500 mV (nA)");
	l->setDisplayValue(26, "1 V (uA)");
	o->addParameter(0, l);
	settings["Sensitivity"] = *o;
	
	o = new Option("", "RMOD %d", "RMOD?");
	l = new ListParameter("Dynamic Reserve Mode", "-", 0, 2);
	l->setDisplayValue(0, "High Reserve");
	l->setDisplayValue(1, "Normal");
	l->setDisplayValue(2, "Low Noise");
	o->addParameter(0, l);
	settings["Dynamic Reserve Mode"] = *o;
	
	o = new Option("", "OFLT %d", "OFLT?");
	l = new ListParameter("Time Constant", "-", 0, 19);
	l->setDisplayValue(0, "10 us");
	l->setDisplayValue(1, "30 us");
	l->setDisplayValue(2, "100 us");
	l->setDisplayValue(3, "300 us");
	l->setDisplayValue(4, "1 ms");
	l->setDisplayValue(5, "3 ms");
	l->setDisplayValue(6, "10 ms");
	l->setDisplayValue(7, "30 ms");
	l->setDisplayValue(8, "100 ms");
	l->setDisplayValue(9, "300 ms");
	l->setDisplayValue(10, "1 s");
	l->setDisplayValue(11, "3 s");
	l->setDisplayValue(12, "10 s");
	l->setDisplayValue(13, "30 s");
	l->setDisplayValue(14, "100 s");
	l->setDisplayValue(15, "300 s");
	l->setDisplayValue(16, "1 ks");
	l->setDisplayValue(17, "3 ks");
	l->setDisplayValue(18, "10 ks");
	l->setDisplayValue(19, "30 ks");
	o->addParameter(0, l);
	settings["Time Constant"] = *o;
	
	o = new Option("", "OFSL %d", "OFSL?");
	l = new ListParameter("Low Pass Filter Slope", "-", 0, 3);
	l->setDisplayValue(0, "6 dB/oct");
	l->setDisplayValue(1, "12 dB/oct");
	l->setDisplayValue(2, "18 dB/oct");
	l->setDisplayValue(3, "24 dB/oct");
	o->addParameter(0, l);
	settings["Low Pass Filter Slope"] = *o;
	
	o = new Option("", "SYNC %d", "SYNC?");
	l = new ListParameter("Synchronous Filter Status", "-", 0, 1);
	l->setDisplayValue(0, "No synchronous filtering");
	l->setDisplayValue(1, "Synchronous filtering below 200Hz");
	o->addParameter(0, l);
	settings["Synchronous Filter Status"] = *o;
	
	o = new Option("CH1 ", "DDEF 1,%d,%d", "DDEF? 1");
	l = new ListParameter("Displayed Value", "-", 0, 4);
	l->setDisplayValue(0, "X");
	l->setDisplayValue(1, "R");
	l->setDisplayValue(2, "X Noise");
	l->setDisplayValue(3, "Aux In 1");
	l->setDisplayValue(4, "Aux In 2");
	ListParameter* l2 = new ListParameter("Ratio", "-", 0, 2);
	l2->setDisplayValue(0, "none");
	l2->setDisplayValue(1, "Aux In 1");
	l2->setDisplayValue(2, "Aux In 2");
	o->addParameter(0, l);
	o->addParameter(1, l2);
	settings["CH1 display settings"] = *o;
	
	o = new Option("CH2 ", "DDEF 2,%d,%d", "DDEF? 2");
	l = new ListParameter("Displayed Value", "-", 0, 4);
	l->setDisplayValue(0, "Y");
	l->setDisplayValue(1, "Theta");
	l->setDisplayValue(2, "Y Noise");
	l->setDisplayValue(3, "Aux In 3");
	l->setDisplayValue(4, "Aux In 4");
	l2 = new ListParameter("Ratio", "-", 0, 2);
	l2->setDisplayValue(0, "none");
	l2->setDisplayValue(1, "Aux In 3");
	l2->setDisplayValue(2, "Aux In 4");
	o->addParameter(0, l);
	o->addParameter(1, l2);
	settings["CH2 display settings"] = *o;
	
	o = new Option("CH1 ", "FPOP 1,%d", "FPOP? 1");
	l = new ListParameter("Output Quantity", "-", 0, 1);
	l->setDisplayValue(0, "CH 1 Display");
	l->setDisplayValue(1, "X");
	o->addParameter(0, l);
	settings["CH1 Output Quantity"] = *o;
	
	o = new Option("CH2 ", "FPOP 2,%d", "FPOP? 2");
	l = new ListParameter("Output Quantity", "-", 0, 1);
	l->setDisplayValue(0, "CH 2 Display");
	l->setDisplayValue(1, "Y");
	o->addParameter(0, l);
	settings["CH2 Output Quantity"] = *o;
	
	o = new Option("X Output ", "OEXP 1,%f,%d", "OEXP? 1");
	d = new DoubleParameter("Offset", "%", -105, 105);
	l = new ListParameter("Expand", "-", 0, 2);
	l->setDisplayValue(0, "No Expand");
	l->setDisplayValue(1, "Expand by 10");
	l->setDisplayValue(2, "Expand by 100");
	o->addParameter(0, d);
	o->addParameter(1, l);
	settings["X output settings"] = *o;
	
	
	o = new Option("Y Output ", "OEXP 2,%f,%d", "OEXP? 2");
	d = new DoubleParameter("Offset", "%", -105, 105);
	l = new ListParameter("Expand", "-", 0, 2);
	l->setDisplayValue(0, "No Expand");
	l->setDisplayValue(1, "Expand by 10");
	l->setDisplayValue(2, "Expand by 100");
	o->addParameter(0, d);
	o->addParameter(1, l);
	settings["Y output settings"] = *o;
	
	
	o = new Option("R Output ", "OEXP 2,%f,%d", "OEXP? 2");
	d = new DoubleParameter("Offset", "%", -105, 105);
	l = new ListParameter("Expand", "-", 0, 2);
	l->setDisplayValue(0, "No Expand");
	l->setDisplayValue(1, "Expand by 10");
	l->setDisplayValue(2, "Expand by 100");
	o->addParameter(0, d);
	o->addParameter(1, l);
	settings["R output settings"] = *o;
	
	o = new Option("", "AUXV 1,%f", "AUXV? 1");
	d = new DoubleParameter("Aux Output 1 ", "V", -10.5, 10.5);
	o->addParameter(0, d);
	settings["Aux Out 1"] = *o;
	
	o = new Option("", "AUXV 2,%f", "AUXV? 2");
	d = new DoubleParameter("Aux Output 2 ", "V", -10.5, 10.5);
	o->addParameter(0, d);
	settings["Aux Out 2"] = *o;
	
	o = new Option("", "AUXV 3,%f", "AUXV? 3");
	d = new DoubleParameter("Aux Output 3 ", "V", -10.5, 10.5);
	o->addParameter(0, d);
	settings["Aux Out 3"] = *o;
	
	o = new Option("", "AUXV 4,%f", "AUXV? 4");
	d = new DoubleParameter("Aux Output 4 ", "V", -10.5, 10.5);
	o->addParameter(0, d);
	settings["Aux Out 4"] = *o;
	
	o = new Option("", "SRAT %d", "SRAT?");
	l = new ListParameter("Data Storage Sample Rate", "-", 0, 14);
	l->setDisplayValue(0, "62.5 mHz");
	l->setDisplayValue(1, "125 mHz");
	l->setDisplayValue(2, "250 mHz");
	l->setDisplayValue(3, "500 mHz");
	l->setDisplayValue(4, "1 Hz");
	l->setDisplayValue(5, "2 Hz");
	l->setDisplayValue(6, "4 Hz");
	l->setDisplayValue(7, "8 Hz");
	l->setDisplayValue(8, "16 Hz");
	l->setDisplayValue(9, "32 Hz");
	l->setDisplayValue(10, "64 Hz");
	l->setDisplayValue(11, "128 Hz");
	l->setDisplayValue(12, "256 Hz");
	l->setDisplayValue(13, "512 Hz");
	l->setDisplayValue(14, "Trigger");
	o->addParameter(0, l);
	settings["Data Storage Sample Rate"] = *o;
	
	o = new Option("", "SEND %d", "SEND?");
	l = new ListParameter("End of Buffer Mode", "-", 0, 1);
	l->setDisplayValue(0, "1 Shot");
	l->setDisplayValue(1, "Loop");
	o->addParameter(0, l);
	settings["End of Buffer Mode"] = *o;
	
	o = new Option("", "TSTR %d", "TSTR?");
	l = new ListParameter("Data Storage Trigger Start Mode", "-", 0, 1);
	l->setDisplayValue(0, "Trigger Start Feature Off");
	l->setDisplayValue(1, "Trigger Starts the Scan");
	o->addParameter(0, l);
	settings["Data Storage Trigger Start Mode"] = *o;
}	

void LockinSettings::queryAllOptions(Addr4882_t addr) {
	(*LockinSettings::settingsLogger) << "=== CURRENT SETTINGS CONFIGURATION ===" << std::endl;
	LockinSettings::settingsLogger->flush();
	for(std::map<std::string, Option>::iterator it = settings.begin();
			it != settings.end(); it++) {
		char res[40];
		char cmd[40];
		strcpy(cmd, it->second.getQueryCommand().c_str());
		g->string_response_command(addr, cmd, res, 40);
//		std::cout << "queryAll: setting option "<< it->first << " to '" << std::string(res) << std::endl;
        it->second.setValues(std::string(res));
        OptionData vals = it->second.getValues();
//        std::cout << "queryAll: option " << it->first << " values (i1, i2, d1, d2) are: " << vals.intVal1 << ", " << vals.intVal2 << ", " << vals.dblVal1 << ", " << vals.dblVal2 << std::endl;
	}
}

bool LockinSettings::isIOption(std::string option) {
	for(int i = 0; i < NUM_IOPTIONS; i++) {
		if(option == iOptions[i]) {
			return true;
		}
	}
	return false;
}

void LockinSettings::set(Addr4882_t addr, std::string option, int value) {
	if(isIOption(option)) {
		char passVal[40];
		snprintf(passVal, 40, "%d", value);
		settings[option].setValues(std::string(passVal));
		char cmdStr[40];
		snprintf(cmdStr, 40, settings[option].getAssignCommand().c_str(),
				settings[option].getValues().intVal1);
		g->send_command(addr, cmdStr);
	} else {
		std::cout << "Option " << option << "is not an integer Option";
	}
}

bool LockinSettings::isDOption(std::string option) {
	for(int i = 0; i < NUM_DOPTIONS; i++) {
		if(option == dOptions[i]) {
			return true;
		}
	}
	return false;
}

void LockinSettings::set(Addr4882_t addr, std::string option, double value) {
	if(isDOption(option)) {
		char passVal[40];
		snprintf(passVal, 40, "%f", value);
		settings[option].setValues(std::string(passVal));
		char cmdStr[40];
		snprintf(cmdStr, 40, settings[option].getAssignCommand().c_str(),
				settings[option].getValues().dblVal1);
		g->send_command(addr, cmdStr);
	} else {
		std::cout << "Option " << option << "is not a double Option";
	}
}

bool LockinSettings::isDIOption(std::string option) {
	for(int i = 0; i < NUM_DIOPTIONS; i++) {
		if(option == diOptions[i]) {
			return true;
		}
	}
	return false;
}

void LockinSettings::set(Addr4882_t addr, std::string option, double val1, int val2) {
	if(isDIOption(option)) {
		char passVal[40];
		snprintf(passVal, 40, "%f,%d", val1, val2);
		settings[option].setValues(std::string(passVal));
		char cmdStr[40];
		snprintf(cmdStr, 40, settings[option].getAssignCommand().c_str(),
				settings[option].getValues().dblVal1, settings[option].getValues().intVal1);
		g->send_command(addr, cmdStr);
	} else {
		std::cout << "Option " << option << "is not a double,integer Option";
	}
}

bool LockinSettings::isIIOption(std::string option) {
	for(int i = 0; i < NUM_IIOPTIONS; i++) {
		if(option == iiOptions[i]) {
			return true;
		}
	}
	return false;
}

void LockinSettings::set(Addr4882_t addr, std::string option, int val1, int val2) {
	if(isIIOption(option)) {
		char passVal[40];
		snprintf(passVal, 40, "%d,%d", val1, val2);
		settings[option].setValues(std::string(passVal));
		char cmdStr[40];
		snprintf(cmdStr, 40, settings[option].getAssignCommand().c_str(),
				settings[option].getValues().intVal1, settings[option].getValues().intVal2);
		g->send_command(addr, cmdStr);
	} else {
		std::cout << "Option " << option << "is not an integer,integer Option";
	}
}

void LockinSettings::writeAllOptions(std::ofstream* opfs) {
	for(std::map<std::string, Option>::iterator i = settings.begin(); 
			i != settings.end(); i++) {
		(*opfs) << i->second.getDisplayString() << std::endl;
	}
}

OptionData LockinSettings::get(std::string option) {
	return settings[option].getValues();
}

#endif
