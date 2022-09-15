// SweepLevel.cpp
// encoding: utf-8
//
// Object-oriented approach to defining recursive parameter sweeps.
//
// Author:   Connor D. Pierce
// Created:  2018-07-02 15:48
// Modified: 2022-09-15 15:06:24
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


#ifndef SWEEPLEVEL_CPP
#define SWEEPLEVEL_CPP

#include "SweepLevel.h"

void SweepLevel::setStart(double start) {
	this->start = start;
}

double SweepLevel::getStart() {
	return this->start;
}

void SweepLevel::setEnd(double end) {
	this->end = end;
}

double SweepLevel::getEnd() {
	return this->end;
}

void SweepLevel::setSteps(int steps) {
	this->steps = steps;
}

int SweepLevel::getSteps() {
	return this->steps;
}

void SweepLevel::setRepeats(int repeats) {
	this->repeats = repeats;
}

int SweepLevel::getRepeats() {
	return this->repeats;
}

void SweepLevel::setSubLevel(SweepLevel* subLevel) {
	this->subLevel = subLevel;
}

SweepLevel* SweepLevel::getSubLevel() {
	return this->subLevel;
}

#endif
