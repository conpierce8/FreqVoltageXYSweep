/*
 * FreqVoltageXYSweep.h
 *
 *  Created on: Feb 2, 2018
 *      Author: Connor D. Pierce
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <windows.h>
#include <commctrl.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <stdint.h>
#include <sstream>
#include <limits.h>
#include <cmath>
#include <ctime>
#include <fstream>

#include "resource.h"
#include "SR830.h"

// ADDED IN V1.0.15 {
#define STEPS_PER_RAMP_DECADE 25
// } ADDED IN V1.0.15

const char g_szClassName[] = "myWindowClass";

// ADDED IN V1.0.8 {
const char FVXY_version[] = "Vibrometer Controller V1.0.15";
// } ADDED IN V1.0.8

/*
 * struct SweepParameters
 *
 * This data structure defines the data that is needed to perform a sweep
 * across N variables. This structure describes the dimensionality, nesting
 * order, repetition, wait times, and ranges for a multi-parameter sweep.
 *
 * Fields:
 *   parameters - int array of size N. This parameter describes the nesting of
 *       the sweeps. parameters[0] contains an identifier for the parameter to
 *       be controlled in the top-level sweep. parameters[1] contains an
 *       identifier for the parameter to be controlled in the second-level
 *       sweep; that is, a full sweep of parameters[1] will be made for each
 *       value of parameters[0] in the top level of the sweep
 *   maxRecursionLevel - integer describing the size of parameters, identifying
 *       the number of parameters to be swept, and therefore the number of
 *       levels of recursion of the sweep
 *   starts - array with size equal to size(parameters). starts[i] gives the
 *       starting value for the i^th level of the sweep
 *   ends - array with size equal to size(parameters). ends[i] gives the final
 *       value for the i^th level of the sweep
 *   steps - array with size equal to size(parameters). steps[i] gives the
 *       number of steps between starts[i] and ends[i]. Note that a sweep with
 *       N steps will include N+1 points, in order to include the endpoints
 *   repeats - array with size equal to size(parameters). repeats[i] gives the
 *       number of times to repeat the i^th level of the sweep
 *   waits - array with size equal to size(parameters). waits[i] gives the
 *       number of milliseconds to wait after setting the value for the i^th
 *       parameter in the sweep. If i denotes the innermost level of the sweep,
 *       the program will wait for a period of waits[i] before measuring the
 *       response of the vibrometer. If i denotes a level which is not the
 *       innermost level of the sweep, the program will wait for a period of
 *       waits[i] before entering the (i+1)^th level of the sweep
 *   autoTimeConst - if true, allow the program to automatically adjust the time
 *       constant to be appropriate for the frequency being studied. This value
 *       will have no effect unless parameters[i] = SWEEP_F for some i.
 *   autoSens - allow the program to automatically adjust the lockin sensitivity
 *       to adjust to signal magnitudes which have a functional dependence on
 *       the sweep parameters.
 *   detHarm - detection harmonic, for example, if detHarm=2, the lockin will
 *       excite a frequency f and measure the signal component at 2f
 *   logSpacing - true to specify logarithmicly-spaced sweep values, false to
 *       specify linearly-spaced sweep values. ADDED IN V1.0.9. As of V1.0.9,
 *       only amplitude and frequency can be log-spaced.
 */
struct SweepParameters {
	int maxRecursionLevel;
	int parameters[NUM_AVAIL_PARAMS];
	double starts[NUM_AVAIL_PARAMS];
	double ends[NUM_AVAIL_PARAMS];
	int steps[NUM_AVAIL_PARAMS];
	int repeats[NUM_AVAIL_PARAMS];
	long waits[NUM_AVAIL_PARAMS];
	bool autoTimeConst;
	bool autoSens;
	bool logSpacing[NUM_AVAIL_PARAMS]; // Added in V1.0.9
	int detHarm; // Added in version 1.0.4
};

#define FIRST_STEP_WAIT 2000
#define ATTEN_2F -80
#define LOG10_3 0.477121

enum STRCONV_ERROR { CONV_SUCCESS, CONV_OVERFLOW, CONV_UNDERFLOW,
		CONV_INCONVERTIBLE };

STRCONV_ERROR str2int (int &i, char const *s) {
    char *end;
    long  l;
    errno = 0;
    l = strtol(s, &end, 10);
    if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
    	std::cout << "overflow" << std::endl;
        return CONV_OVERFLOW;
    }
    if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
        return CONV_UNDERFLOW;
    }
    if (*s == '\0' || *end != '\0') {
        return CONV_INCONVERTIBLE;
    }
    i = l;
    return CONV_SUCCESS;
}

STRCONV_ERROR str2long (long &l, char const *s) {
    char *end;
    long i;
    errno = 0;
    i = strtol(s, &end, 10);
    if (errno == ERANGE && i == LONG_MAX) {
        return CONV_OVERFLOW;
    }
    if (errno == ERANGE && i == LONG_MIN) {
        return CONV_UNDERFLOW;
    }
    if (*s == '\0' || *end != '\0') {
        return CONV_INCONVERTIBLE;
    }
    l = i;
    return CONV_SUCCESS;
}

STRCONV_ERROR str2dbl (double &d, char const *s) {
    char *end;
    double i;
    errno = 0;
    i = strtod(s, &end);
    if (errno == ERANGE && (i == HUGE_VAL || i == -HUGE_VAL)) {
        return CONV_OVERFLOW;
    }
    if (errno == ERANGE) {
        return CONV_UNDERFLOW;
    }
    if (*s == '\0' || *end != '\0') {
        return CONV_INCONVERTIBLE;
    }
    d = i;
    return CONV_SUCCESS;
}

void createControls(HINSTANCE hInstance);

void createParamControls(HINSTANCE hInstance, int yPos, int param,
		BOOL active);

int sweep(int recursionLevel, std::string prefix);

void updateActiveParams(int changedParam);

void getControlIDs(uintptr_t* IDs, int param);

bool validateSweepParams(bool runSweep);

void populateTree(HWND tree);

long calculateSweepDuration();

DWORD WINAPI SweepThreadFunction( LPVOID lpParam );

DWORD WINAPI connectToAmp( LPVOID lpParam );

LRESULT CALLBACK CustomXYDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK DetHarmDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

// ADDED IN V1.0.12 {
LRESULT CALLBACK AveragingDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
// } ADDED IN V1.0.12

void sendCommandToLockin(int currParam, double currVal);

bool enableCustomSweep(bool useXY);

void updateCustomXYText();

bool storeCustomXYPoint();

void createCustomXYControls(HINSTANCE hInstance, int yPos, BOOL active);

void deleteCustomXYPoint();

bool getCustomXYValues(double &x, double &y);

void logCanceledSweep(); // ADDED IN V1.0.8

/* double getSensValue(int i)
 *
 * Returns the double-precision value representing the value of the sensitivity
 * given by the index i. See LockinSettings.cpp for valid indices and the values
 * of the sensitivity to which they correspond. Returned value is in volts.
 */
double getSensValue(int i);

/* double getTimeConstValue(int i)
 *
 * Returns the double-precision value representing the value of the time 
 * constant given by the index i. See LockinSettings.cpp for valid indices and
 * the values of the time constant to which they correspond. Returned value is
 * in seconds.
 */
double getTimeConstValue(int i);

/* int getTimeConst(double reqTau)
 * 
 * Returns the integer value used by the lockin to denote the smallest
 * available time constant larger than reqTau.
 */
int getTimeConst(double reqTau);

/* double getWaitFactor(int filtSlope)
 *
 * Returns the multiplier which relates the wait time to the time constant
 * that is appropriate for the indicated slope of the low-pass filter.
 */
double getWaitFactor(int filtSlope);

// ADDED IN V1.0.11 {

/* bool outOfRange(double ampl, int sens)
 *
 * Determines whether the given voltage `ampl` is out of range for the lockin
 * sensitivity `sens`.
 */
bool outOfRange(double ampl, int sens);

/* int getBestSens(double ampl)
 *
 * Returns the lockin sensitivity that is most appropriate for the given
 * voltage `ampl`. Specifically, it returns the integer representing the
 * smallest lockin sensitivity such that `ampl < 0.9*(sensitivity)`.
 */
int getBestSens(double ampl);
// } ADDED IN V1.0.11

/* double getFiltSlopeValue(int i)
 *
 * Returns the double-precision value representing the slope of the low-pass
 * filter given by the index i. See LockinSettings.cpp for valid indices and the
 * values of the filter slope to which they correspond. Returned value is in
 * dB/octet and is signed, i.e. a filter with roll-off of 18 dB/octet will be
 * represented by -18.0.
 */
double getFiltSlopeValue(int i);

int numActiveParams = 2;
volatile bool cancelSweep = true;
bool activeParams[] = {false, false, true, true, false};
bool acceptTextInput = false;
// ADDED IN V1.0.12 {
bool averaging = false;
int numAvgPts = 10;
// } ADDED IN V1.0.12
SweepParameters sweepSetup;
HANDLE bgThreadHandle = NULL, gpibCheckerHandle = NULL;
const HANDLE cancelSweepEvent = CreateEvent(NULL, TRUE, FALSE,
		"CancelSweepEvent");
const HANDLE quitGpibCheckerEvent = CreateEvent(NULL, TRUE, FALSE,
		"QuitGpibCheckerEvent");
GPIBInterface *gpibInterface = NULL; //GPIBInterface(0); //changed type to pointer
SR830 *lockin = NULL; // = SR830(&gpibInterface,8);
std::ofstream logger("log.txt");
volatile bool connReady = false;
HWND hwnd;
Addr4882_t nstrmnts[31], rslt[31];

OPENFILENAME ofn;
char szFileName[MAX_PATH] = "";

double* customX;
double* customY;
int numCustom;
bool settingCustomSweep = FALSE;
int currCustomStep = 0;
int rampType = 1;

#endif /* MAIN_H_ */


