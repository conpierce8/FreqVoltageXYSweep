#ifndef LOCKINPARAMETERS_H_
#define LOCKINPARAMETERS_H_

#include <map>
#include <string>
#include <cstdio>

struct OptionData {
	double dblVal1, dblVal2;
	int intVal1, intVal2;
	
	bool dblVal1Set;
	bool dblVal2Set;
	bool intVal1Set;
	bool intVal2Set;
	
	OptionData() {
		dblVal1 = dblVal2 = 0;
		intVal1 = intVal2 = 0;
		dblVal1Set = dblVal2Set = intVal1Set = intVal2Set = false;
	}
	
};

class Parameter {

protected:
	std::string desc;
	std::string units;
	std::string type;
	
public:
	std::string getDescription();
	std::string getType();
	std::string getUnits();
	void setUnits();
	
	virtual void setValue(std::string val) = 0;
	virtual OptionData getValue() = 0;
	virtual std::string getDisplayString() = 0;
	virtual bool isValid(std::string value) = 0;
	
};

class IntParameter: public Parameter {

protected:
	const int minValue, maxValue;
	int value;
	
public:
	IntParameter(std::string description, std::string units, int minValue, int maxValue);
	
	void setValue(int val);
	int getMinValue();
	void setMinValue(int val);
	int getMaxValue();
	void setMaxValue(int val);
	std::string getUnits();
	void setUnits(std::string units);
	
	virtual void setValue(std::string val);
	virtual OptionData getValue();
	virtual std::string getDisplayString();
	virtual bool isValid(std::string value);
	
};

class ListParameter: public IntParameter {
	std::map<int, std::string> displayVals;
	
public:
	ListParameter(std::string description, std::string units, int minValue, int maxValue);
	
	void setDisplayValue(int key, std::string displayVal);
	std::map<int, std::string> getDisplayValues();
	
	virtual std::string getDisplayString();
	
};

class DoubleParameter: public Parameter {
	const double minValue, maxValue;
	double value;
	
public:
	DoubleParameter(std::string description, std::string units, double minValue, double maxValue);
	void setValue(double val);
	double getMinValue();
	void setMinValue(double val);
	double getMaxValue();
	void setMaxValue(double val);
	std::string getUnits();
	
	virtual void setValue(std::string val);
	virtual OptionData getValue();
	virtual std::string getDisplayString();
	virtual bool isValid(std::string value);
	
};

#endif