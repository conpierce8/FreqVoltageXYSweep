#ifndef LOCKINSETTINGS_H_
#define LOCKINSETTINGS_H_

#include <fstream>
#include <list>
#include <sstream>

#include "GPIB.h"
#include "LockinParameters.h"

class Option {
	std::string desc;
	std::string assignCmd;
	std::string queryCmd;
	std::list<Parameter*> params;
	
public:
	Option();
	Option(std::string desc, std::string assignCmd, std::string queryCmd);
	
	std::string getDesc();
	std::string getDisplayString();
	std::string getAssignCommand();
	std::string getQueryCommand();
	
	void addParameter(int index, Parameter* param);
	void removeParameter(Parameter* param);
	std::list<Parameter*> getParameters();
	
	void setValues(std::string val);
	OptionData getValues();
	
};

#define NUM_IOPTIONS 17
#define NUM_DOPTIONS 7
#define NUM_IIOPTIONS 2
#define NUM_DIOPTIONS 3
class LockinSettings {
	//TODO: build a system where failures to set the requested setting are passed back to the calling function
	std::map<std::string, Option> settings;
	GPIBInterface* g;
	
public:
	static std::ofstream* settingsLogger;
	
	LockinSettings(GPIBInterface* g);
	
	void queryAllOptions(Addr4882_t addr);
	void writeAllOptions(std::ofstream* opfs);
	
	bool isIOption(std::string option);
	bool isDOption(std::string option);
	bool isDIOption(std::string option);
	bool isIIOption(std::string option);
	
	void set(Addr4882_t addr, std::string option, int value);
	void set(Addr4882_t addr, std::string option, double value);
	void set(Addr4882_t addr, std::string option, double val1, int val2);
	void set(Addr4882_t addr, std::string option, int val1, int val2);
	
	OptionData get(std::string option);
	
};

const std::string iOptions[] = {"Reference Source", 
		"Reference Trigger", "Detection Harmonic",
		"Input Configuration", "Input Shield Grounding", "Input Coupling",
		"Input Line Notch Filter Status", "Sensitivity",
		"Dynamic Reserve Mode", "Time Constant", "Low Pass Filter Slope",
		"Synchronous Filter Status", "CH1 Output Quantity",
		"CH2 Output Quantity", "Data Storage Sample Rate",
		"End of Buffer Mode", "Data Storage Trigger Start Mode"};
const std::string dOptions[] = {"Reference Phase Shift", 
		"Reference Frequency", "Sine Output Amplitude",
		"Aux Out 1", "Aux Out 2", "Aux Out 3", "Aux Out 4"};
const std::string iiOptions[] = {"CH1 display settings",
		"CH2 display settings"};
static const std::string diOptions[] = {"X output settings",
		"Y output settings", "R output settings"};

#endif
