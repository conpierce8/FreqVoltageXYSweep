#ifndef SWEEPLEVEL_H
#define SWEEPLEVEL_H

#include <string>

class SweepLevel {
	double start;
	double end;
	int steps;
	int repeats;
	SweepLevel* subLevel;
	std::string type;
	
public:
	/*
	 * Public constructor. Creates a SweepLevel with its subLevel equal to NULL.
	 */
	SweepLevel();
	
	/*
	 * Sets the starting value for the sweep.
	 */
	void setStart(double start);
	
	/*
	 * Returns the starting value for the sweep.
	 */
	double getStart();
	
	/*
	 * Sets the ending value for the sweep.
	 */
	void setEnd(double end);
	
	/*
	 * Returns the ending value for the sweep.
	 */
	double getEnd();
	
	/*
	 * Sets the number of steps for the sweep. The number of points will be
	 * equal to the number of steps + 1.
	 */
	void setSteps(int steps);
	
	/*
	 * Returns the number of steps for the sweep.
	 */
	int getSteps();
	
	/*
	 * Sets the number of times to repeat the sweep.
	 */
	void setRepeats(int repeats);
	
	/*
	 * Returns the number of times to repeat the sweep.
	 */
	int getRepeats();
	
	/*
	 * Sets the sublevel for the sweep.
	 */
	void setSubLevel(SweepLevel* subLevel);
	
	/*
	 * Returns the sublevel for the sweep.
	 */
	SweepLevel* getSubLevel();
	
	/*
	 * Performs the sweep, calling sublevels of the sweep as appropriate.
	 */
	virtual int sweep() { };
	
};

class FrequencySweepLevel : public SweepLevel {
	const std::string options[1];// = {""};
};

#endif
