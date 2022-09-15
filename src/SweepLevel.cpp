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
