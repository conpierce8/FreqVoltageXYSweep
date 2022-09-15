// resource.h
// encoding: utf-8
//
// FreqVoltageXYSweep Win32 resource header file.
//
// Author:   Connor D. Pierce
// Created:  2018-02-01
// Modified: 2022-09-15 14:44:42
//
// Copyright (C) 2022 Connor D. Pierce
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


#ifndef RESOURCE_H_
#define RESOURCE_H_

/*
 * This section defines the icons that are used in the application, for use by
 * the Win32 GUI. Icons are assigned numbers in the 100s.
 */
#define SWEEP_ICON 101

/*
 * This section defines the generic menu controls: the menu, the exit
 * menu item, and a menu item separator, for use by the Win32 GUI. Generic
 * menu items are assigned numbers in the 200s.
 */
#define SWEEP_MENU 201
#define MI_EXIT 202
#define MI_SEPARATOR 203

/*
 * This section defines the number of parameters which can be controlled with
 * this application.
 *
 * Currently, the available parameters are two mirror axes (X and Y), the
 * amplitude of the piezo driving signal, the frequency of the piezo driving
 * signal, and a custom XY sweep.
 */
#define NUM_AVAIL_PARAMS 5

/*
 * This section defines identifiers for menu items, for use by the Win32 GUI.
 * Each of these identifiers is paired with a corresponding identifier, for use
 * by sweep() in FreqVoltageXYSweep.cpp. Menu items for controlling the sweep
 * settings are assigned numbers in the 300s.
 */
#define MI_SWEEP_OVER_X 300
#define SWEEP_X 0

#define MI_SWEEP_OVER_Y 301
#define SWEEP_Y 1

#define VOLTAGE_MAX 10
#define VOLTAGE_MIN -10

#define MI_SWEEP_OVER_F 302
#define SWEEP_F 2

#define MI_SWEEP_OVER_A 303
#define SWEEP_A 3

#define MI_CUSTOM_BLANK 304
#define SWEEP_CUSTOM 4

#define MI_CUSTOM_XY 305

#define MI_CUSTOM_FROMFILE 307

#define MI_CUSTOM_DISABLE 306

#define MI_AUTO_SENS 308

#define MI_DET_HARM 309

#define MI_AVERAGING 310

#define MI_RAMP_NONE 311
#define MI_RAMP_ALL 312
#define MI_RAMP_LOG 313

const char PARAM_DESCRIPTIONS[][10] = {"X","Y","F","A","Custom XY"};

/*
 * This section defines controls (such as text fields, buttons, etc.), to be
 * used by the Win32 GUI. The run and cancel buttons are assigned numbers in
 * the 400s; text fields for entering sweep settings are assigned numbers in
 * the 500s.
 */
#define BTN_RUN_SWEEP 401
#define BTN_CANCEL_SWEEP 402

#define CONTROLS_PER_PARAM 7

#define LTEXT_X_PARAMS 501
#define LTEXT_X_START 502
#define LTEXT_X_END 503
#define LTEXT_X_STEPS 504
#define LTEXT_X_REPEAT 505
#define LTEXT_X_WAIT 506
#define LTEXT_X_ORDER 525

#define LTEXT_Y_PARAMS 507
#define LTEXT_Y_START 508
#define LTEXT_Y_END 509
#define LTEXT_Y_STEPS 510
#define LTEXT_Y_REPEAT 511
#define LTEXT_Y_WAIT 512
#define LTEXT_Y_ORDER 526

#define LTEXT_F_PARAMS 513
#define LTEXT_F_START 514
#define LTEXT_F_END 515
#define LTEXT_F_STEPS 516
#define LTEXT_F_REPEAT 517
#define LTEXT_F_WAIT 518
#define LTEXT_F_ORDER 527
#define LTEXT_F_AUTOTC 537
#define LTEXT_F_LOG 538

#define LTEXT_A_PARAMS 519
#define LTEXT_A_START 520
#define LTEXT_A_END 521
#define LTEXT_A_STEPS 522
#define LTEXT_A_REPEAT 523
#define LTEXT_A_WAIT 524
#define LTEXT_A_ORDER 528
#define LTEXT_A_LOG 539

#define LTEXT_C_PARAMS 529
#define LTEXT_C_STEP 530
#define LTEXT_C_XVOLTS 531
#define LTEXT_C_YVOLTS 532
#define BTN_C_ADD 533
#define BTN_C_DELETE 534
#define LTEXT_C_ORDER 535
#define LTEXT_C_WAIT 536

#define TVIEW_SWEEP_ORDER 701
#define LTEXT_SUMMARY 702

/*
 * This section defines controls identifiers for the custom XY sweep dialog.
 */
#define CUSTOM_XY_DIALOG 801
#define LTEXT_DIALOG 802 
#define DET_HARM_DIALOG 803
#define LTEXT_DET_HARM 804
#define AVERAGING_DIALOG 805
#define LTEXT_AVG_PTS 806

#endif /* RESOURCE_H_ */
