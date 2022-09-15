// SweepLevel.h
// encoding: utf-8
//
// Object-oriented approach to defining recursive parameter sweeps.
//
// Author:   Connor D. Pierce
// Created:  2018-07-02 15:48
// Modified: 2022-09-15 15:06:48
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
