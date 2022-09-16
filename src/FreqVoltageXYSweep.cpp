// FreqVoltageXYSweep.cpp
// encoding: utf-8
//
// Main program for FreqVoltageXYSweep.
//
// Author:   Connor D. Pierce
// Created:  2018-02-02
// Modified: 2022-09-15 15:02:20
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


#include "FreqVoltageXYSweep.h"


/*
 * Handle click events from the Win32 GUI main window.
 */
LRESULT CALLBACK WndProc(HWND hwndLcl, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int count = 0;
    switch(msg)
    {
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case MI_SWEEP_OVER_X:
                case MI_SWEEP_OVER_Y:
                case MI_SWEEP_OVER_A:
                case MI_SWEEP_OVER_F:
                case MI_AUTO_SENS:
                {
                    updateActiveParams(LOWORD(wParam));
                    validateSweepParams(false);
                    break;
                }
                case MI_CUSTOM_BLANK:
                {
                    int ret = DialogBox(
                        GetModuleHandle(NULL),
                        MAKEINTRESOURCE(CUSTOM_XY_DIALOG),
                        hwnd,
                        CustomXYDlgProc
                    );
                    if(ret == IDOK) {
                        enableCustomSweep(false);
                    } else {
                        numCustom = 0;
                        settingCustomSweep = false;
                        currCustomStep = 0;
                    }
                    break;
                }
                case MI_CUSTOM_XY:
                {
                    logger << "WndProc: new blank sweep, numCustom=" << numCustom <<std::endl;
                    enableCustomSweep(true);
                    break;
                }
                case MI_CUSTOM_DISABLE:
                {
                    settingCustomSweep = false;
                    updateActiveParams(MI_CUSTOM_BLANK);
                    break;
                }
                case MI_DET_HARM:
                {
                    int ret = DialogBox(
                        GetModuleHandle(NULL),
                        MAKEINTRESOURCE(DET_HARM_DIALOG),
                        hwnd,
                        DetHarmDlgProc
                    );
                    if(ret == IDOK) {
                        logger << "WndProc: user set the detection harmonic" << std::endl;
                        validateSweepParams(false);
                    }
                    break;
                }
                case MI_AVERAGING:
                {
                    HMENU menu = GetMenu(hwnd);
                    if(averaging) {
                        averaging = false;
                        CheckMenuItem(menu, MI_AVERAGING, MF_UNCHECKED);
                    } else {
                        int ret = DialogBox(
                            GetModuleHandle(NULL),
                            MAKEINTRESOURCE(AVERAGING_DIALOG),
                            hwnd,
                            AveragingDlgProc
                        );
                        if(ret == IDOK) {
                            logger << "WndProc: user enabled averaging; points: " << numAvgPts << std::endl;
                            averaging = true;
                            CheckMenuItem(menu, MI_AVERAGING, MF_CHECKED);
                        }
                    }
                    break;
                }
                case MI_EXIT:
                {
                    PostMessage(hwndLcl, WM_CLOSE, 0, 0);
                    break;
                }
                case BTN_RUN_SWEEP:
                {
                    if(GetSaveFileName(&ofn)) {
                        validateSweepParams(true);
                    }
                    logger << "Run Sweep!" << std::endl;
                    break;
                }
                case MI_RAMP_NONE:
                {
                    rampType = 0;
                    HMENU menu = GetMenu(hwnd);
                    CheckMenuItem(menu, MI_RAMP_NONE, MF_CHECKED);
                    CheckMenuItem(menu, MI_RAMP_ALL, MF_UNCHECKED);
                    CheckMenuItem(menu, MI_RAMP_LOG, MF_UNCHECKED);
                    break;
                }
                case MI_RAMP_ALL:
                {
                    rampType = 1;
                    HMENU menu = GetMenu(hwnd);
                    CheckMenuItem(menu, MI_RAMP_NONE, MF_UNCHECKED);
                    CheckMenuItem(menu, MI_RAMP_ALL, MF_CHECKED);
                    CheckMenuItem(menu, MI_RAMP_LOG, MF_UNCHECKED);
                    break;
                }
                case MI_RAMP_LOG:
                {
                    rampType = 2;
                    HMENU menu = GetMenu(hwnd);
                    CheckMenuItem(menu, MI_RAMP_NONE, MF_UNCHECKED);
                    CheckMenuItem(menu, MI_RAMP_ALL, MF_UNCHECKED);
                    CheckMenuItem(menu, MI_RAMP_LOG, MF_CHECKED);
                    break;
                }
                case BTN_CANCEL_SWEEP:
                {
                    cancelSweep = true;
                    SetEvent(cancelSweepEvent);
//                    logger << count << std::endl;
                    break;
                }
                case BTN_C_ADD:
                {
                    if(settingCustomSweep) {
                        if(getCustomXYValues(customX[currCustomStep], customY[currCustomStep])) {
                            ++currCustomStep;
                            logger << "WndProc: currCustomStep incremented to: " << currCustomStep << std::endl;
                            if(currCustomStep == numCustom){
                                logger << "WndProc: currCustomStep equals numCustom" << std::endl;
                                currCustomStep = 0;
                                settingCustomSweep = false;
                            }
                            updateCustomXYText();
                        }
                    }
                    validateSweepParams(false);
                    break;
                }
                case BTN_C_DELETE:
                {
                    if(settingCustomSweep) {
                        deleteCustomXYPoint();
                        if(currCustomStep == numCustom) {
                            currCustomStep == 0;
                            settingCustomSweep = false;
                        }
                        updateCustomXYText();
                    }
                    validateSweepParams(false);
                    break;
                }
                case LTEXT_C_XVOLTS: case LTEXT_C_YVOLTS:
                {
                    double X, Y;
                    bool flag = getCustomXYValues(X, Y);
                    if(flag && connReady) {
                        sendCommandToLockin(SWEEP_X, X);
                        sendCommandToLockin(SWEEP_Y, Y);
                    }
                    break;
                }
                case LTEXT_SUMMARY:
                    break;
                case LTEXT_F_AUTOTC:
                {
                    sweepSetup.autoTimeConst = !sweepSetup.autoTimeConst;
                    HWND chkBx = GetDlgItem(hwnd, LTEXT_F_AUTOTC);
                    if(sweepSetup.autoTimeConst) {
                        SendMessage(chkBx, BM_SETCHECK, BST_CHECKED, 0);
                    } else {
                        SendMessage(chkBx, BM_SETCHECK, BST_UNCHECKED, 0);
                    }
                    break;
                }
                case LTEXT_F_LOG:
                {
                    sweepSetup.logSpacing[SWEEP_F] = !sweepSetup.logSpacing[SWEEP_F];
                    HWND chkBx = GetDlgItem(hwnd, LTEXT_F_LOG);
                    if(sweepSetup.logSpacing[SWEEP_F]) {
                        SendMessage(chkBx, BM_SETCHECK, BST_CHECKED, 0);
                    } else {
                        SendMessage(chkBx, BM_SETCHECK, BST_UNCHECKED, 0);
                    }
                    break;
                }
                case LTEXT_A_LOG:
                {
                    sweepSetup.logSpacing[SWEEP_A] = !sweepSetup.logSpacing[SWEEP_A];
                    HWND chkBx = GetDlgItem(hwnd, LTEXT_A_LOG);
                    if(sweepSetup.logSpacing[SWEEP_A]) {
                        SendMessage(chkBx, BM_SETCHECK, BST_CHECKED, 0);
                    } else {
                        SendMessage(chkBx, BM_SETCHECK, BST_UNCHECKED, 0);
                    }
                    break;
                }
                default:
                {
                    if(HIWORD(wParam) == EN_CHANGE) {
                        count++;
                        if(acceptTextInput) {
                            validateSweepParams(false);
                        }
                        break;
                    } else {
                        logger << LOWORD(wParam) << ";" << HIWORD(wParam) << ";"
                                << LOWORD(lParam) << ";" << HIWORD(lParam)
                                << std::endl;
                    }
                    break;
                }
            }
            break;
        }
        case WM_CLOSE:
        {
            SetEvent(cancelSweepEvent);
            SetEvent(quitGpibCheckerEvent);
            WaitForSingleObject(bgThreadHandle, 5000ul);
            WaitForSingleObject(gpibCheckerHandle, 1000);
            logger.flush();
            logger.close();
            DestroyWindow(hwndLcl);
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hwndLcl, msg, wParam, lParam);
    }
    return 0;
}

/*
 * Extract custom X,Y values from the "custom XY" dialog box.
 */
bool getCustomXYValues(double &x, double &y)
{
    char content[80];  //Create a temp variable to store the text entered by user
    double tmpX, tmpY; //Create temp variables to store the values entered by user
    
    //Obtain a handle to the field for X value
    HWND ctrl = GetDlgItem(hwnd, LTEXT_C_XVOLTS);
    
    //Extract user input from text field and store in "content"
    GetWindowText(ctrl, content, 80);
    
    //Attempt to convert user input to numerical value. If conversion succeeds,
    //"res" will be equal to CONV_SUCCESS
    STRCONV_ERROR res = str2dbl(tmpX, content);
    if(res != CONV_SUCCESS) {
        //Conversion was not successful; we cannot assign a numerical value to "x"
        return false;
    }
    
    //Repeat above steps to obtain y value
    ctrl = GetDlgItem(hwnd, LTEXT_C_YVOLTS);
    GetWindowText(ctrl, content, 80);
    res = str2dbl(tmpY, content);
    if(res != CONV_SUCCESS) {
        return false;
    }
    
    //If we reach this point in function, both conversions succeeded. Store
    //the temporary numerical values and return true. The galvo system can
    //only handle voltages from -10V to +10V, so x and y are restricted to
    //these values.
    if(tmpX > VOLTAGE_MAX) {
        x = VOLTAGE_MAX;
    } else if(tmpX < VOLTAGE_MIN) {
        x = VOLTAGE_MIN;
    } else {
        x = tmpX;
    }
    if(tmpY > VOLTAGE_MAX) {
        y = VOLTAGE_MAX;
    } else if(tmpY < VOLTAGE_MIN) {
        y = VOLTAGE_MIN;
    } else {
        y = tmpY;
    }
    return true;
}

/*
 * Update the input prompt message in the custom X,Y dialog box.
 */
void updateCustomXYText()
{
    std::ostringstream oss;
    logger << "updateCustomXYText: currCustomStep=" << currCustomStep << std::endl;
    logger << "updateCustomXYText: customX=" << customX[currCustomStep] << std::endl;
    logger << "updateCustomXYText: numCustom=" << numCustom << std::endl;
    oss << customX[currCustomStep];
    HWND ctrl = GetDlgItem(hwnd, LTEXT_C_XVOLTS);
    SetWindowText(ctrl, oss.str().c_str());
    std::ostringstream oss2;
    oss2 << customY[currCustomStep];
    ctrl = GetDlgItem(hwnd, LTEXT_C_YVOLTS);
    SetWindowText(ctrl, oss2.str().c_str());
    std::ostringstream oss3;
    oss3 << currCustomStep;
    ctrl = GetDlgItem(hwnd, LTEXT_C_STEP);
    SetWindowText(ctrl, oss3.str().c_str());
}

/*
 * Delete the currCustomStep-th entry from the array of custom X,Y points.
 */
void deleteCustomXYPoint()
{
    for(int i = currCustomStep; i < numCustom-1; i++) {
        customX[i] = customX[i+1];
        customY[i] = customY[i+1];
    }
    numCustom--;
}

/*
 * Handle click events from the custom X,Y dialog box.
 */
LRESULT CALLBACK CustomXYDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message) {
        case WM_INITDIALOG:
            return TRUE;
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDOK:
                {
                    STRCONV_ERROR res;
                    HWND ctrl;
                    ctrl = GetDlgItem(hwnd, LTEXT_DIALOG);
                    char content[80];
                    GetWindowText(ctrl, content, 80);
                    res = str2int(numCustom, content);
                    if(res == CONV_SUCCESS){
                        EndDialog(hwnd, IDOK);
                    }
                    break;
                }
                case IDCANCEL:
                {
                    EndDialog(hwnd, IDCANCEL);
                    break;
                }
            }
            break;
        }
        default:
            return FALSE;
    }
    return TRUE;
}

/*
 * Handle click events from the detection harmonic dialog box.
 */
LRESULT CALLBACK DetHarmDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    HWND ctrl = GetDlgItem(hwnd, LTEXT_DET_HARM);
    char content[80];
    switch(Message)
    {
        case WM_INITDIALOG:
        {
            snprintf(content, 80, "%d", sweepSetup.detHarm);
            SetWindowText(ctrl, content);
            return TRUE;
        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDOK:
                {
                    GetWindowText(ctrl, content, 80);
                    STRCONV_ERROR res = str2int(sweepSetup.detHarm, content);
                    if(res == CONV_SUCCESS){
                        EndDialog(hwnd, IDOK);
                    }
                    break;
                }
                case IDCANCEL:
                    EndDialog(hwnd, IDCANCEL);
                    break;
            }
            break;
        }
        default:
            return FALSE;
    }
    return TRUE;
}

/*
 * Handle click events from the measurement averaging dialog box.
 */
LRESULT CALLBACK AveragingDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    HWND ctrl = GetDlgItem(hwnd, LTEXT_AVG_PTS);
    char content[80];
    switch(Message)
    {
        case WM_INITDIALOG:
        {
            snprintf(content, 80, "%d", numAvgPts);
            SetWindowText(ctrl, content);
            return TRUE;
        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDOK:
                {
                    GetWindowText(ctrl, content, 80);
                    STRCONV_ERROR res = str2int(numAvgPts, content);
                    if(res == CONV_SUCCESS){
                        EndDialog(hwnd, IDOK);
                    }
                    break;
                }
                case IDCANCEL:
                {
                    EndDialog(hwnd, IDCANCEL);
                    break;
                }
            }
            break;
        }
        default:
            return FALSE;
    }
    return TRUE;
}

/*
 * Main function for Win32 GUI program: initialize GUI.
 */
int WINAPI WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow
)
{
    WNDCLASSEX wc;
    MSG Msg;

    HICON hMyIcon   = (HICON) LoadImage(
        hInstance,
        MAKEINTRESOURCE(SWEEP_ICON),
        IMAGE_ICON,
        32,
        32,
        0
    );
    HICON hMyIconSm = (HICON) LoadImage(
        hInstance,
        MAKEINTRESOURCE(SWEEP_ICON),
        IMAGE_ICON,
        16,
        16,
        0
    );
    logger << "Loaded icons!" << std::endl;
    
    //Step 1: Registering the Window Class
    wc.cbSize         = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc     = WndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = hMyIcon;
    wc.hIconSm       = hMyIconSm;
    wc.hCursor         = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(SWEEP_MENU);
    wc.lpszClassName = g_szClassName;

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    logger << "Registered Window Class" << std::endl;

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        FVXY_version,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        760,
        550,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if(hwnd == NULL) {
        MessageBox(
            NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK
        );
        return 0;
    }
    
    logger << "Created the window" << std::endl;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = (
        OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
    );
    
    logger << "Created filechooser struct" << std::endl;

    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwICC = ICC_TREEVIEW_CLASSES;
    initCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);

    BOOL iccInitFinished = InitCommonControlsEx(&initCtrls);
    logger << "Common Control Initialization:" << iccInitFinished << std::endl;
    
    sweepSetup.detHarm = 1;
    for(int i = 0; i<NUM_AVAIL_PARAMS; i++) {
        sweepSetup.logSpacing[i] = false;
    }
    createControls(hInstance);
    acceptTextInput = TRUE;
    LPDWORD thrdId = NULL;
    for(int i = 0; i<NUM_DEVICES - 1; i++) {
        nstrmnts[i] = i+1;
    }
    nstrmnts[NUM_DEVICES - 1] = NOADDR;
    gpibCheckerHandle = CreateThread(NULL, 0, connectToAmp, NULL, 0, thrdId);
    logger << "Started GPIB checking" << std::endl;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    logger << "Showed window" << std::endl;

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        if(!IsDialogMessage(hwnd,&Msg)) {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);
        }
    }
    return Msg.wParam;
}

/*
 * Periodically check for connection to lock-in when a sweep is not active.
 */
DWORD WINAPI connectToAmp(LPVOID lpParam)
{
    while(1) {
        if(cancelSweep) {
            //Only perform this check if a sweep is not currently active
            if(gpibInterface == NULL) {
                //Currently not connected; try to connect by creating
                //new GPIBInterface and new SR830 objects
                try {
                    logger << "connectToAmp: trying to create interface" << std::endl;
                    gpibInterface = new GPIBInterface(0);
                    logger << "  connectToAmp: created interface" << std::endl;
                    lockin = new SR830(gpibInterface,8);
                    logger << "    connectToAmp: created SR830" << std::endl;
                    connReady = true;
//                    lockin->settings.queryAllOptions(lockin->address);
                } catch(DisconnectedException &ex) {
                    logger << "  connectToAmp: disconnected exception:" << ex.what() << std::endl;
                    delete gpibInterface;
                    gpibInterface = NULL;
                    connReady = false;
                } catch(std::exception &e) {
                    logger << "Exception in GPIB checker: " << e.what();
                    gpibInterface = NULL;
                    connReady = false;
                    break;
                }
            } else {
                //We were connected at last check; see if we're still connected
                logger << "connectToAmp: checking to see if still connected" << std::endl;
                FindLstn(0, nstrmnts, rslt, 31);
                if(ibsta & ERR) {
                    logger << "  connectToAmp: error occured in FindLstn" << std::endl;
                    //Connection to GPIB board had some kind of error; we're
                    //probably not connected anymore. Free up the memory used
                    //by the current GPIBInterface and SR830 objects.
                    delete gpibInterface;
                    delete lockin;
                    gpibInterface = NULL;
                    lockin = NULL;
                    connReady = false;
                }
            }
        }
        logger << "  connectToAmp: validating sweep params" << std::endl;
        validateSweepParams(false);
        if(WaitForSingleObject(quitGpibCheckerEvent, 5000) == WAIT_OBJECT_0) {
            break;
        }
    }
    return 1;
}

/*
 * Create the text input fields and buttons that appear in the main GUI window.
 */
void createControls(HINSTANCE hInstance)
{
    CreateWindow(
        "BUTTON", 
        "Run Sweep",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        220,
        370,
        100,
        100,
        hwnd,
        (HMENU) BTN_RUN_SWEEP,
        hInstance,
        NULL
    );
    CreateWindow(
        "BUTTON",
        "Cancel Sweep",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        330,
        370,
        100,
        100,
        hwnd,
        (HMENU) BTN_CANCEL_SWEEP,
        hInstance,
        NULL
    );
    CreateWindowEx(
        WS_EX_LEFT,
        WC_TREEVIEW,
        TEXT("Tree View"),
        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
        10,
        370,
        200,
        100,
        hwnd,
        (HMENU)TVIEW_SWEEP_ORDER,
        hInstance,
        NULL
    );
    CreateWindow(
        "EDIT",
        (connReady)?"GPIB Ready":"GPIB Disconnected!",
        WS_VISIBLE | WS_CHILD | ES_READONLY | ES_MULTILINE,
        450,
        370,
        290,
        100,
        hwnd,
        (HMENU) LTEXT_SUMMARY,
        hInstance,
        NULL
    );

    int yPos = 10;
    for(int ctrl = SWEEP_X; ctrl <= SWEEP_A; ctrl++) {
        createParamControls(hInstance, yPos, ctrl, activeParams[ctrl]);
        yPos += 70;
    }
    createCustomXYControls(hInstance, yPos, activeParams[SWEEP_CUSTOM]);
}

/*
 * Initialize text input fields for a given parameter (e.g. Frequency, etc.) in the GUI
 * main window.
 */
void createParamControls(HINSTANCE hInstance, int yPos, int param, BOOL active)
{
    uintptr_t controlIDs[CONTROLS_PER_PARAM];
    getControlIDs(controlIDs, param);

    uintptr_t ltext_hdg    = controlIDs[0]; uintptr_t ltext_start = controlIDs[1];
    uintptr_t ltext_end    = controlIDs[2]; uintptr_t ltext_steps = controlIDs[3];
    uintptr_t ltext_repeat = controlIDs[4]; uintptr_t ltext_wait  = controlIDs[5];
    uintptr_t ltext_order  = controlIDs[6];
    std::string paramStr;

    switch(param) {
        case SWEEP_X:
        {
            paramStr   ="Parameters for X sweep:";
            break;
        }
        case SWEEP_Y:
        {
            paramStr   ="Parameters for Y sweep:";
            break;
        }
        case SWEEP_F:
        {
            paramStr   ="Parameters for F sweep:";
            break;
        }
        case SWEEP_A:
        {
            paramStr   ="Parameters for A sweep:";
            break;
        }
        case SWEEP_CUSTOM:
        {
            paramStr   ="Parameters for custom X,Y sweep:";
            break;
        }
    }

    HWND tmp;

    CreateWindow(
        "STATIC",
        "A frame to surround the parameter controls",
        WS_VISIBLE | WS_CHILD | SS_ETCHEDFRAME,
        5,
        yPos-5,
        730,
        55,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );

    tmp = CreateWindow(
        "STATIC",
        paramStr.data(),
        WS_VISIBLE | WS_CHILD,
        10,
        yPos,
        150,
        15,
        hwnd,
        (HMENU) ltext_hdg,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "Order:",
        WS_VISIBLE | WS_CHILD,
        620,
        yPos,
        50,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        675,
        yPos,
        50,
        15,
        hwnd,
        (HMENU) ltext_order,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "Start at:",
        WS_VISIBLE | WS_CHILD,
        10,
        yPos + 25,
        50,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        65,
        yPos+25,
        50,
        15,
        hwnd,
        (HMENU) ltext_start,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "Sweep to:",
        WS_VISIBLE | WS_CHILD,
        135,
        yPos + 25,
        65,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        205,
        yPos+25,
        50,
        15,
        hwnd,
        (HMENU) ltext_end,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "# of steps:",
        WS_VISIBLE | WS_CHILD,
        275,
        yPos + 25,
        75,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        355,
        yPos + 25,
        50,
        15,
        hwnd,
        (HMENU) ltext_steps,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);
    
    if(param == SWEEP_F) {
        tmp = CreateWindow(
            "BUTTON",
            "Auto Time Constant",
            WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
            250,
            yPos,
            150,
            15,
            hwnd,
            (HMENU) LTEXT_F_AUTOTC,
            hInstance,
            NULL
        );
        EnableWindow(tmp, active);
        tmp = CreateWindow(
            "BUTTON",
            "Log Spacing",
            WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
            425,
            yPos,
            150,
            15,
            hwnd,
            (HMENU) LTEXT_F_LOG,
            hInstance,
            NULL
        );
        EnableWindow(tmp, active);
    } else if(param == SWEEP_A) {
        tmp = CreateWindow(
            "BUTTON",
            "Log Spacing",
            WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
            425,
            yPos,
            150,
            15,
            hwnd,
            (HMENU) LTEXT_A_LOG,
            hInstance,
            NULL
        );
        EnableWindow(tmp, active);
    }

    CreateWindow(
        "STATIC",
        "# of repeats:",
        WS_VISIBLE | WS_CHILD,
        425,
        yPos + 25,
        85,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        515,
        yPos + 25,
        30,
        15,
        hwnd,
        (HMENU) ltext_repeat,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "Wait time (ms):",
        WS_VISIBLE | WS_CHILD,
        565,
        yPos + 25,
        105,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        675,
        yPos + 25,
        50,
        15,
        hwnd,
        (HMENU) ltext_wait,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);
}

/*
 * Initialize custom X,Y controls in the GUI main window.
 */
void createCustomXYControls(HINSTANCE hInstance, int yPos, BOOL active)
{
    uintptr_t controlIDs[CONTROLS_PER_PARAM];
    getControlIDs(controlIDs, SWEEP_CUSTOM);

    uintptr_t ltext_hdg    = controlIDs[0]; uintptr_t ltext_wait   = controlIDs[1];
    uintptr_t ltext_xvolts = controlIDs[2]; uintptr_t ltext_yvolts = controlIDs[3];
    uintptr_t btn_confirm  = controlIDs[4]; uintptr_t btn_delete   = controlIDs[5];
    uintptr_t ltext_order  = controlIDs[6];
    std::string paramStr ="Parameters for custom X,Y sweep:";

    HWND tmp;
    HWND tmpEdit;

    CreateWindow(
        "STATIC",
        "A frame to surround the parameter controls",
        WS_VISIBLE | WS_CHILD | SS_ETCHEDFRAME,
        5,
        yPos - 5,
        730,
        55,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );

    tmp = CreateWindow(
        "STATIC",
        paramStr.data(),
        WS_VISIBLE | WS_CHILD,
        10,
        yPos,
        150,
        15,
        hwnd,
        (HMENU) ltext_hdg,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);
    
    CreateWindow(
        "STATIC",
        "Wait (ms):",
        WS_VISIBLE | WS_CHILD,
        505,
        yPos,
        35,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        545,
        yPos,
        50,
        15,
        hwnd,
        (HMENU) ltext_wait,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "Order:",
        WS_VISIBLE | WS_CHILD,
        620,
        yPos,
        50,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        675,
        yPos,
        50,
        15,
        hwnd,
        (HMENU) ltext_order,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "Step #:",
        WS_VISIBLE | WS_CHILD,
        10,
        yPos + 25,
        50,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD,
        65,
        yPos + 25,
        50,
        15,
        hwnd,
        (HMENU) LTEXT_C_STEP,
        hInstance,
        NULL
    );
    EnableWindow(tmp, false);

    CreateWindow(
        "STATIC",
        "X voltage:",
        WS_VISIBLE | WS_CHILD,
        135,
        yPos + 25,
        65,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        205,
        yPos + 25,
        50,
        15,
        hwnd,
        (HMENU) ltext_xvolts,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    CreateWindow(
        "STATIC",
        "Y voltage:",
        WS_VISIBLE | WS_CHILD,
        275,
        yPos + 25,
        75,
        15,
        hwnd,
        (HMENU) -1,
        hInstance,
        NULL
    );
    tmp = CreateWindow(
        "EDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP,
        355,
        yPos + 25,
        50,
        15,
        hwnd,
        (HMENU) ltext_yvolts,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    tmp = CreateWindow(
        "BUTTON",
        "Confirm Point",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        425,
        yPos + 25,
        100,
        20,
        hwnd,
        (HMENU) btn_confirm,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);

    tmp = CreateWindow(
        "BUTTON",
        "Delete Point",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        535,
        yPos + 25,
        100,
        20,
        hwnd,
        (HMENU) btn_delete,
        hInstance,
        NULL
    );
    EnableWindow(tmp, active);
}

/*
 * Get the Win32 identifiers for the controls corresponding to a given sweep parameter.
 *
 * The values are stored in the array `IDs`, which should be an int array of size
 * CONTROLS_PER_PARAM. (See resource.h for a description of the meaning of
 * CONTROLS_PER_PARAM). The last value in the `IDs` array will always be the Win32
 * identifier of the "Order" field.
 *
 * Inputs
 * ------
 * IDs: int array of size CONTROLS_PER_PARAM
 *     array in which to store the Win32 control identifiers
 * param: int
 *     sweep parameter. One of the following:
 *       SWEEP_X,
 *       MI_SWEEP_OVER_X,
 *       SWEEP_Y,
 *       MI_SWEEP_OVER_Y,
 *       SWEEP_F,
 *       MI_SWEEP_OVER_F,
 *       SWEEP_A,
 *       MI_SWEEP_OVER_A,
 *       SWEEP_CUSTOM,
 *       MI_CUSTOM_BLANK,
 *       MI_CUSTOM_XY
 *
 * Outputs
 * -------
 * IDs[0] = ID for the LTEXT heading
 * IDs[1] = ID for the EDIT control for the start value
 * IDs[2] = ID for the EDIT control for the end value
 * IDs[3] = ID for the EDIT control for the # of steps value
 * IDs[4] = ID for the EDIT control for the # of repeats value
 * IDs[5] = ID for the EDIT control for the wait time value
 * IDs[CONTROLS_PER_PARAM-1] = ID for the EDIT control for the order value
 */
void getControlIDs(uintptr_t* IDs, int param)
{
    switch(param)
    {
        case SWEEP_X:
        case MI_SWEEP_OVER_X:
        {
            IDs[0] = LTEXT_X_PARAMS;
            IDs[1] = LTEXT_X_START;
            IDs[2] = LTEXT_X_END;
            IDs[3] = LTEXT_X_STEPS;
            IDs[4] = LTEXT_X_REPEAT;
            IDs[5] = LTEXT_X_WAIT;
            IDs[6] = LTEXT_X_ORDER;
            break;
        }
        case SWEEP_Y:
        case MI_SWEEP_OVER_Y:
        {
            IDs[0] = LTEXT_Y_PARAMS;
            IDs[1] = LTEXT_Y_START;
            IDs[2] = LTEXT_Y_END;
            IDs[3] = LTEXT_Y_STEPS;
            IDs[4] = LTEXT_Y_REPEAT;
            IDs[5] = LTEXT_Y_WAIT;
            IDs[6] = LTEXT_Y_ORDER;
            break;
        }
        case SWEEP_F:
        case MI_SWEEP_OVER_F:
        {
            IDs[0] = LTEXT_F_PARAMS;
            IDs[1] = LTEXT_F_START;
            IDs[2] = LTEXT_F_END;
            IDs[3] = LTEXT_F_STEPS;
            IDs[4] = LTEXT_F_REPEAT;
            IDs[5] = LTEXT_F_WAIT;
            IDs[6] = LTEXT_F_ORDER;
            break;
        }
        case SWEEP_A:
        case MI_SWEEP_OVER_A:
        {
            IDs[0] = LTEXT_A_PARAMS;
            IDs[1] = LTEXT_A_START;
            IDs[2] = LTEXT_A_END;
            IDs[3] = LTEXT_A_STEPS;
            IDs[4] = LTEXT_A_REPEAT;
            IDs[5] = LTEXT_A_WAIT;
            IDs[6] = LTEXT_A_ORDER;
            break;
        }
        case SWEEP_CUSTOM:
        case MI_CUSTOM_BLANK:
        case MI_CUSTOM_XY:
        {
            IDs[0] = LTEXT_C_PARAMS;
            IDs[1] = LTEXT_C_WAIT;
            IDs[2] = LTEXT_C_XVOLTS;
            IDs[3] = LTEXT_C_YVOLTS;
            IDs[4] = BTN_C_ADD;
            IDs[5] = BTN_C_DELETE;
            IDs[6] = LTEXT_C_ORDER;
            break;
        }
    }
}

/*
 * Update the list of active sweep parameters and enable/disable the corresponding
 * GUI items when a menu item is clicked.
 */
void updateActiveParams(int changedParam)
{
    int idx = -1;
    HMENU menu = GetMenu(hwnd);
    
    switch(changedParam)
    {
        case MI_SWEEP_OVER_X:
        {
            idx = SWEEP_X;
            break;
        }
        case MI_SWEEP_OVER_Y:
        {
            idx = SWEEP_Y;
            break;
        }
        case MI_SWEEP_OVER_A:
        {
            idx = SWEEP_A;
            break;
        }
        case MI_SWEEP_OVER_F:
        {
            idx = SWEEP_F;
            break;
        }
        case MI_CUSTOM_BLANK:
        case MI_CUSTOM_XY:
        case MI_CUSTOM_FROMFILE:
        {
            idx = SWEEP_CUSTOM;
            break;
        }
        case MI_AUTO_SENS:
        {
            sweepSetup.autoSens = !sweepSetup.autoSens;
            if(sweepSetup.autoSens) {
                CheckMenuItem(menu, changedParam, MF_CHECKED);
            } else {
                CheckMenuItem(menu, changedParam, MF_UNCHECKED);
            }
            return;
        }
        case MI_AVERAGING:
        {
            sweepSetup.autoSens = !sweepSetup.autoSens;
            if(sweepSetup.autoSens) {
                CheckMenuItem(menu, changedParam, MF_CHECKED);
            } else {
                CheckMenuItem(menu, changedParam, MF_UNCHECKED);
            }
            return;
        }
    }

    if(activeParams[idx]) {
        logger << "Active Parameter; de-activating" << std::endl;
        activeParams[idx] = FALSE;
        numActiveParams--;
        if(idx == SWEEP_CUSTOM) {
            EnableMenuItem(menu, MI_CUSTOM_DISABLE, MF_DISABLED);
        } else {
            CheckMenuItem(menu, changedParam, MF_UNCHECKED);
        }
    } else {
        logger << "Inactive Parameter; activating" << std::endl;
        activeParams[idx] = TRUE;
        numActiveParams++;
        if(idx == SWEEP_CUSTOM) {
            EnableMenuItem(menu, MI_CUSTOM_DISABLE, MF_ENABLED);
        } else {
            CheckMenuItem(menu, changedParam, MF_CHECKED);
        }
    }

    uintptr_t controlIDs[CONTROLS_PER_PARAM];
    getControlIDs(controlIDs, idx);
    for(int i = 0; i < CONTROLS_PER_PARAM; i++) {
        HWND ctrl = GetDlgItem(hwnd, controlIDs[i]);
        EnableWindow(ctrl, activeParams[idx]);
    }

    EnableWindow(GetDlgItem(hwnd, BTN_RUN_SWEEP), numActiveParams > 0);

    //TODO: update sweep overview graphics
}

/*
 * Validate the user-provided entries in the sweep parameter input fields and enable the
 * "Run Sweep" button if all parameters are provided and valid.
 */
bool validateSweepParams(bool runSweep)
{
    HWND tree = GetDlgItem(hwnd, TVIEW_SWEEP_ORDER);
    SendMessage(tree, TVM_DELETEITEM, 0, (LPARAM) (LPTVINSERTSTRUCT) TVI_ROOT);
    HWND lblSummary = GetDlgItem(hwnd, LTEXT_SUMMARY);
    SetWindowText(
        lblSummary,
        (connReady) ? "GPIB Ready" + std::endl : "GPIB Disconnected!" + std::endl
    );
    EnableWindow(GetDlgItem(hwnd, BTN_RUN_SWEEP), FALSE);
    int order = -1;

    sweepSetup.maxRecursionLevel = numActiveParams - 1;
    for(int i = 0; i < NUM_AVAIL_PARAMS; i++) {
        sweepSetup.parameters[i] = -1;
    }

    uintptr_t controlIDs[CONTROLS_PER_PARAM];
    STRCONV_ERROR res = CONV_SUCCESS;
    HWND ctrl;

    for(int i = 0; i < NUM_AVAIL_PARAMS; i++) {
        char content[80];
        if(i == SWEEP_CUSTOM && activeParams[i]) {
            logger << "validating; custom sweep is active";
            ctrl = GetDlgItem(hwnd, LTEXT_C_ORDER);
            GetWindowText(ctrl, content, 80);
            res = str2int(order, content);
            if(order >= 1 && order <= numActiveParams && res == CONV_SUCCESS) {
                if(sweepSetup.parameters[order - 1] != -1) {
                    return false;
                } else {
                    sweepSetup.parameters[order - 1] = i;
                }
            } else {
                return false;
            }
            ctrl = GetDlgItem(hwnd, LTEXT_C_WAIT);
            GetWindowText(ctrl, content, 80);
            res = str2long(sweepSetup.waits[i], content);
            if(sweepSetup.waits[i] < 1) {
                return false;
            }
            sweepSetup.repeats[i] = 1;
        } else if(activeParams[i]) { //Only validate parameters which are set as active
            getControlIDs(controlIDs, i);

            ctrl = GetDlgItem(hwnd, controlIDs[CONTROLS_PER_PARAM-1]);
            GetWindowText(ctrl, content, 80);
            res = str2int(order, content);
            if(order < 1 || order > numActiveParams || res != CONV_SUCCESS) {
                //Given order is out of range; input is invalid
                return false;
            } else {
                if(sweepSetup.parameters[order - 1] != -1) {
                    //The given order is a duplicate; input is invalid
                    return false;
                } else {
                    sweepSetup.parameters[order - 1] = i;
                }
            }

            for(int j = 1; j < CONTROLS_PER_PARAM-1; j++) {
                ctrl = GetDlgItem(hwnd, controlIDs[j]);
                GetWindowText(ctrl, content, 80);
                switch(j)
                {
                    case 1:
                    {
                        res = str2dbl(sweepSetup.starts[i], content);
                        break;
                    }
                    case 2:
                    {
                        res = str2dbl(sweepSetup.ends[i], content);
                        break;
                    }
                    case 3:
                    {
                        res = str2int(sweepSetup.steps[i], content);
                        if(sweepSetup.steps[i] < 1) {
                            //Must have at least one step in the sweep;
                            //this is an invalid value
                            return false;
                        }
                        break;
                    }
                    case 4:
                    {
                        res = str2int(sweepSetup.repeats[i], content);
                        if(sweepSetup.repeats[i] < 1) {
                            //Must perform the sweep at least once; this
                            //is an invalid value
                            return false;
                        }
                        break;
                    }
                    case 5:
                    {
                        res = str2long(sweepSetup.waits[i], content);
                        if(sweepSetup.waits[i] < 1) {
                            //Wait time cannot be less than 0; this is
                            //invalid value
                            return false;
                        }
                        break;
                    }
                }
                if(res != CONV_SUCCESS) { // Found an invalid value
                    return false;
                } else {
                    if(i == SWEEP_X || i == SWEEP_Y) {
                        if(sweepSetup.starts[i] > VOLTAGE_MAX) {
                            sweepSetup.starts[i] = VOLTAGE_MAX;
                        } else if(sweepSetup.starts[i] < VOLTAGE_MIN) {
                            sweepSetup.starts[i] = VOLTAGE_MIN;
                        }
                        if(sweepSetup.ends[i] > VOLTAGE_MAX) {
                            sweepSetup.ends[i] = VOLTAGE_MAX;
                        } else if(sweepSetup.ends[i] < VOLTAGE_MIN) {
                            sweepSetup.ends[i] = VOLTAGE_MIN;
                        }
                    }
                }
            }
        }
    }

    EnableWindow(GetDlgItem(hwnd,BTN_RUN_SWEEP), TRUE);

    populateTree(tree);

    long dur = calculateSweepDuration();
    int hours =  dur / (3600000);
    int minutes = (dur % 3600000) / 60000;
    double seconds = (dur % 60000) / (1000.0);
    std::ostringstream desc;
    desc << ((connReady) ? "GPIB Ready" + std::endl : "GPIB Disconnected!" + std::endl);
    desc << "Estimated Duration:" + std::endl;
    desc << hours << "hr " << minutes << "min " << seconds << "s";

    if(runSweep && connReady && !settingCustomSweep) {
        LPDWORD threadId = NULL;
        cancelSweep = false;
        ResetEvent(cancelSweepEvent);
        bgThreadHandle = CreateThread(NULL, 0, SweepThreadFunction,
                NULL, 0, threadId);
        logger << bgThreadHandle << std::endl;
//        WaitForSingleObject(bgThreadHandle, INFINITE);
    }
    if(!cancelSweep) {
        desc << "\r\n" << "Sweeping...";
    }
    char charDesc[80];
    const char* s = desc.str().c_str();
    strcpy(charDesc, s);
    SetWindowText(lblSummary, s);
    EnableWindow(GetDlgItem(hwnd, BTN_RUN_SWEEP), TRUE);

    return true;
}

/*
 * Enable a sweep over X,Y values with custom values.
 *
 * Inputs
 * ------
 * useXY:
 *     if true, create initial values for the custom sweep from the values entered in
 *     the X,Y parameter sweep inputs, otherwise initialize the custom X,Y values to 0
 */
bool enableCustomSweep(bool useXY) {
    if(useXY) {
        if(
            activeParams[SWEEP_X] && activeParams[SWEEP_Y] && validateSweepParams(false)
        ) {
            numCustom = (
                (sweepSetup.steps[SWEEP_X] + 1)
                * (sweepSetup.steps[SWEEP_Y] + 1)
                * (sweepSetup.repeats[SWEEP_X])
                * (sweepSetup.repeats[SWEEP_Y])
            );
            customX = new double[numCustom];
            customY = new double[numCustom];
            double xInterval = (
                (sweepSetup.ends[SWEEP_X] - sweepSetup.starts[SWEEP_X])
                / sweepSetup.steps[SWEEP_X]
            );
            double yInterval = (
                (sweepSetup.ends[SWEEP_Y] - sweepSetup.starts[SWEEP_Y])
                / sweepSetup.steps[SWEEP_Y]
            );
            int count = 0;
            for(int nx = 0; nx<sweepSetup.repeats[SWEEP_X]; nx++) {
                for(int ix = 0; ix<sweepSetup.steps[SWEEP_X]+1; ix++) {
                    for(int ny = 0; ny<sweepSetup.repeats[SWEEP_Y]; ny++) {
                        for(int iy = 0; iy<sweepSetup.steps[SWEEP_Y]+1; iy++) {
                            customX[count] = sweepSetup.starts[SWEEP_X] + ix*xInterval;
                            customY[count] = sweepSetup.starts[SWEEP_Y] + iy*yInterval;
                            count++;
                        }
                    }
                }
            }
        } else {
            numCustom = -1;
            settingCustomSweep = FALSE;
            return false;
        }
    } else {
        customX = new double[numCustom];
        customY = new double[numCustom];
        for(int i = 0; i < numCustom; i++) {
            customX[i] = 0;
            customY[i] = 0;
        }
    }
    
    settingCustomSweep = true;
    currCustomStep = 0;
    
    if(activeParams[SWEEP_X]){
        updateActiveParams(MI_SWEEP_OVER_X);
    }
    if(activeParams[SWEEP_Y]) {
        updateActiveParams(MI_SWEEP_OVER_Y);
    }
    if(!activeParams[SWEEP_CUSTOM]){
        updateActiveParams(MI_CUSTOM_BLANK);
    }
    
    updateCustomXYText();
    return true;
}

// 2022-09-16:
// looks like the following line is not needed?
// int count = 0;

/*
 * Start a parameter sweep in a separate thread.
 */
DWORD WINAPI SweepThreadFunction( LPVOID lpParam )
{
    logger <<"Sweep called; lockin="<<lockin->address<< std::endl;
    int retVal = sweep(0, "");
    validateSweepParams(false);
    return retVal;
}

/*
 * Convert the lock-in sensitivity setting (which is stored as an int) to its value
 * (measured in volts).
 */
double getSensValue(int i)
{
    int m = i % 3;
    return (m*m + 2*m + 2) * pow(10.0, i/3 - 9.0);
}

/*
 * Convert the lock-in time constant setting (which is stored as an int) to its value
 * (measured in seconds).
 */
double getTimeConstValue(int i)
{
    return (2*(i%2) + 1) * pow(10.0, i/2 - 5.0);
}

/*
 * Calculate the minimum time constant greater than or equal to `reqTau` from among the
 * time constants available on the lockin. Return the value as an integer as specified
 * in LockinSettings.
 */
int getTimeConst(double reqTau)
{
    double logTau = log10(reqTau);
    double iPart = floor(logTau);
    //logger << "   getTimeConst: reqTau is " << reqTau <<std::endl;
    //logger << "   getTimeConst: logTau is " << logTau <<std::endl;
    //logger << "   getTimeConst: iPart is " << iPart <<std::endl;
    //logger << "   getTimeConst: rT / pow() is " << (reqTau / pow(10.0, iPart)) <<std::endl;
    
    return (2*iPart) + (((reqTau / pow(10.0, iPart)) > 3)?12:11);
}

/*
 * Convert the lock-in filter slope setting (which is stored as an int) to its value
 * (measured in decibels/octave).
 */
double getFiltSlopeValue(int i)
{
    return -6.0 *(i+1);
}

/*
 * Get the appropriate wait time factor (per the SR830 user's manual) for a low-pass
 * filter of a given slope.
 */
double getWaitFactor(int filtSlope)
{
    switch(filtSlope)
    {
        case 0:
            return 5.0;
        case 1:
            return 7.0;
        case 2:
            return 9.0;
        case 3:
            return 10.0;
    }
}

/*
 * Determine whether a given lock-in voltage measurement falls outside the range of
 * appropriate values for a given lock-in sensitivity setting.
 *
 * The range of appropriate values is defined as follows:
 *    - maximum value: equal to the lock-in sensitivity setting. For example, if the
 *                     measured voltage from the lock-in is 505mV and the lock-in is set
 *                     to a sensitivity of 500mV, this function will return true.
 *    - minimum value: equal to 100 times the resolution of the analog-to-digital
 *                     converter in the lockin. The lock-in converts an internal analog
 *                     voltage to a digital representation with a 16-bit ADC. This means
 *                     that the full scale of the lock-in (selected by the sensitivity
 *                     setting) is resolved into one of 2^16 (65536) discrete values. To
 *                     ensure that the signal is adequately resolved by the ADC, this
 *                     function returns true when the measured value is less than 100
 *                     times the discrete voltage step, or about 0.0016 times the full-
 *                     scale voltage
 *
 * Inputs
 * ------
 * ampl:
 *     the measured voltage amplitude
 * sens:
 *     integer sensitivity setting of the lock-in
 */
bool outOfRange(double ampl, int sens)
{
    double fullScale = getSensValue(sens);
    double relAmpl   = ampl / fullScale;
    return (relAmpl < 0.0016) || (relAmpl > 1);
}

/*
 * Chooses the best lock-in sensitivity setting for a given voltage.
 */
int getBestSens(double ampl)
{
    if(ampl <= 1.8e-9) {
        return 0;
    } else if(ampl >= 0.9) {
        return 26;
    } else {
        double logAmp = log10(ampl);
        double base   = floor(logAmp);
        double frac   = logAmp - base;
        
        int decade = 3 * ( (int) base + 9);
        int offset;
        if(frac <= 0.25527) {
            offset = 0;
        } else if(frac <= 0.63521) {
            offset = 1;
        } else if(frac <= 0.95424) {
            offset = 2;
        } else {
            offset = 3;
        }
        logger << "getBestSens: for voltage " << ampl << ", best sensitivity is " << getSensValue(decade+offset) << std::endl;
        return decade + offset;
    }
}

/*
 * Perform a recursive sweep over all active parameters.
 *
 * This function contains the core logic of a parametric sweep. The active parameters
 * and parameter values for the sweep are defined in the sweepParameters object from
 * FreqVoltageXYSweep.h.
 *
 * The multi-parameter sweep can be thought of as a set of nested `for` loops, with the
 * outermost loop defined as level 0, the next loop defined as level 1, etc. For
 * example:
 *
 * ```
 * // Sweep over frequency
 * for(int i = 0; i < num_freq_values; i++) { // Level 0
 *     set_frequency(frequencies[i]);
 *     
 *     // Sweep over amplitude
 *     for(int j = 0; j < num_ampl_values; j++) { // Level 1
 *         set_amplitude(amplitudes[j]);
 *         
 *         // Sweep over X
 *         for(int k = 0; k < num_x_values; k++) { // Level 2
 *             set_x(x_values[k]);
 *             wait(wait_duration);
 *             measure(&amplitude, &phase);
 *             writeToFile(amplitude, phase);
 *         }
 *     }
 * }
 * ```
 *
 * The typical call signature for this function is `sweep(0, "")`, as it will
 * recursively build the prefix of parameter values for each measurement in the sweep.
 *
 * Inputs
 * ------
 * recursionLevel:
 *     level of the `for` loop in the recursive parameter sweep
 * prefix:
 *     string prepend to the amplitude and phase measurement
 *   
 * Returns:
 *     1 - if the sweep finished uninterrupted
 *     0 - if the sweep was canceled before it finished
 */
int sweep(int recursionLevel, std::string prefix)
{
    static std::ofstream outps;
    static int initialSens0;
    int initialSens = 0; // TODO: consider whether this initialization is appropriate
    
    if(recursionLevel == 0) {
        // Open the output file where results will be stored
        outps.open(szFileName);
        
        // Modify the file name appropriately for the settings files, and
        // open the settings file. 
        std::string ofilename(szFileName);
        int idx = ofilename.rfind(".txt");
        std::string settingsFileName = ofilename.substr(0, idx);
        settingsFileName.append("_settings.txt");
        LockinSettings::settingsLogger->flush();
        LockinSettings::settingsLogger->close();
        delete LockinSettings::settingsLogger;
        LockinSettings::settingsLogger = new std::ofstream(settingsFileName.c_str());
        
        // Write the lockin device description and current settings to the settings file
        (*LockinSettings::settingsLogger) << lockin->get_device_description() << std::endl;
        lockin->settings.queryAllOptions(lockin->address);
        lockin->settings.writeAllOptions(&outps);
        
        // Log the start of the sweep
        time_t now = time(0);
        struct tm* tmNow = localtime(&now);
        (*LockinSettings::settingsLogger) << std::endl << "=== SWEEP STARTED at " 
                << (1900 + tmNow->tm_year) << "-" << (1 + tmNow->tm_mon) << "-"
                << tmNow->tm_mday << " " << tmNow->tm_hour << ":" << tmNow->tm_min
                << ":" << tmNow->tm_sec << " ===" << std::endl;
        
        // Ensure detection harmonic is set correctly
        lockin->set_harmonic(sweepSetup.detHarm);
        
        // Write output file header
        outps << std::endl;
        for(int i = 0; i < NUM_AVAIL_PARAMS; i++) {
            if(sweepSetup.parameters[i] == SWEEP_CUSTOM) {
                outps << "X\tY\t";
            } else if(sweepSetup.parameters[i] == SWEEP_F) {
                outps << "Frequency\t";
            } else if(sweepSetup.parameters[i] == SWEEP_A) {
                outps << "Amplitude\t";
            } else if(sweepSetup.parameters[i] == SWEEP_X) {
                outps << "X\t";
            } else if(sweepSetup.parameters[i] == SWEEP_Y) {
                outps << "Y\t";
            }
        }
        
        initialSens0 = initialSens = lockin->get_sensitivity();
        if(averaging) {
            outps << "R\tTheta\tStdDev\t" << std::endl;
        } else {
            outps << "R\tTheta\t" << std::endl;
        }
    } else {
        initialSens = initialSens0;
    }

    int exitVal = 1;

    int currParam    = sweepSetup.parameters[recursionLevel];
    double currStart, currEnd;
    if(sweepSetup.logSpacing[currParam]) {
        currStart = log10(sweepSetup.starts[currParam]);
        currEnd   = log10(sweepSetup.ends[currParam]);    
    } else {
        currStart = sweepSetup.starts[currParam];
        currEnd   = sweepSetup.ends[currParam];
    }
    
    int currSteps;
    if(currParam == SWEEP_CUSTOM){
        currSteps = numCustom - 1;
    } else {
        currSteps = sweepSetup.steps[currParam];
    }
    int currRepeats  = sweepSetup.repeats[currParam];
    long currWait    = sweepSetup.waits[currParam];
    int waitTime = currWait;

    double stepSize = (currEnd - currStart) / currSteps;
    
    int i = 0;
    for(int r = 0; r < currRepeats; r++) {
        (*LockinSettings::settingsLogger) << "At level " << recursionLevel << " of sweep; "
                << "starting loop #" << r << std::endl;
        
        if(cancelSweep) {
            logCanceledSweep();
            exitVal = 0;
            break;
        }

        for(i = 0; i < currSteps+1; i++) {
            if(cancelSweep) {
                logCanceledSweep();
                exitVal = 0;
                break;
            }
            
            if(sweepSetup.autoSens && i == 0) {
                lockin->set_sensitivity(initialSens);
            }
            
            // Update lock-in settings for the current parameter value
            double currVal;
            if(sweepSetup.logSpacing[currParam]) {
                currVal = pow(10, currStart + stepSize*i);
            } else {
                currVal = currStart + stepSize*i;
            }
            if(currParam == SWEEP_CUSTOM) {
                sendCommandToLockin(SWEEP_X, customX[i]);
                sendCommandToLockin(SWEEP_Y, customY[i]);
            } else {
                sendCommandToLockin(currParam, currVal);
            }
            
            // AUTOMATICALLY SET THE TIME CONSTANT IF APPLICABLE
            if(currParam == SWEEP_F && sweepSetup.autoTimeConst) {
                int filtSlope = lockin->settings.get("Low Pass Filter Slope").intVal1;
                
                double tau_req = pow(2, ATTEN_2F/getFiltSlopeValue(filtSlope))/(4*acos(-1)*currVal);
                int currTimeConst = lockin->settings.get("Time Constant").intVal1;
                //logger << "Required tau is " << tau_req << ", current tau is " << getTimeConstValue(currTimeConst) << std::endl;
                int newTimeConst = getTimeConst(tau_req);
                //logger << " New tau is " << getTimeConstValue(newTimeConst) << std::endl;
                if(newTimeConst != currTimeConst) {
                    // The current setting for the time constant is not correct. Update
                    // the lockin settings accordingly.
                    lockin->settings.set(lockin->address, "Time Constant", newTimeConst);
                    //logger << "  Changed the time constant." << std::endl;
                }
                waitTime = getWaitFactor(filtSlope)*getTimeConstValue(newTimeConst)*1000;
            }
            
            /* WAIT ON FIRST SWEEP STEP:
             * If this is the start of a sweep, wait an additional amount of
             * time to allow the lockin output to settle. This is important
             * because the lockin output needs extra time to settle after e.g.
             * the large frequency jump from (max freq) to (min freq) that 
             * occurs at the end of every sweep.
             */
            if(WaitForSingleObject(cancelSweepEvent, waitTime + ((i==0)?FIRST_STEP_WAIT:0) )==WAIT_OBJECT_0) {
                logCanceledSweep();
                exitVal = 0;
                break;
            }

            // TAKE MEASUREMENT OR PROCEED TO NEXT SWEEP LEVEL
            if(sweepSetup.maxRecursionLevel > recursionLevel) {
                std::ostringstream prefixTmp;
                if(currParam == SWEEP_CUSTOM) {
                    prefixTmp << prefix << customX[i] << "\t" << customY[i] << "\t";
                } else {
                    prefixTmp << prefix << currVal << "\t";
                }
                sweep(recursionLevel+1, prefixTmp.str());
            } else {
                double ampl = 0;
                double phs = 0;
                lockin->get_AmplPhase(ampl, phs);
                
                int ct = 0;
                while(sweepSetup.autoSens && outOfRange(ampl, lockin->get_sensitivity()) && ct < 10) {
//                    logger << "Adjusting sensitivity for " << prefix << currVal << std::endl;
//                    logger << "    Old sensitivity: " << sens << std::endl;
                    
                    int sens = getBestSens(ampl);
                    
//                    logger << "    New sensitivity: " << sens << std::endl;
                    
                    lockin->set_sensitivity(sens);
                    // WaitForSingleObject(cancelSweepEvent, 500);
                    if(WaitForSingleObject(cancelSweepEvent, waitTime)==WAIT_OBJECT_0) {
                        logCanceledSweep();
                        exitVal = 0;
                        break;
                    }
                    lockin->get_AmplPhase(ampl, phs);
                    ct++;
                }
                if(ct == 10 && outOfRange(ampl, lockin->get_sensitivity())) {
                    (*LockinSettings::settingsLogger) << "Unable to determine appropriate sensitivity for "
                            << prefix << currVal << std::endl;
                }
                
                double stdDev = 0;
                if(averaging) {
//                    logger << "Averaging." << std::endl;
                    // Create arrays to hold the all the measurements. Store (amplitude-phase)
                    // as a complex number. Note that if lockin->isPhaseAccessible() is false,
                    // all phases will be stored as zeros, and all measurements will be real
                    // numbers. Since the generalized formula for standard deviation of complex
                    // numbers holds for real numbers, no special accommodation is needed for
                    // this case.
                    double* re = new double[numAvgPts];
                    double* im = new double[numAvgPts];
                    
                    // Store the existing measurement
                    re[0] = ampl*cos(M_PI*phs/180);
                    im[0] = ampl*sin(M_PI*phs/180);
//                    logger << "\t" << re[0] << "\t" << im[0] << std::endl;
                    
                    // Take repeated measurements, convert to complex numbers and store,
                    // and update the sum.
                    double reTot = re[0];
                    double imTot = im[0];
                    for(int m = 1; m < numAvgPts; m++) {
                        lockin->get_AmplPhase(ampl, phs);
                        re[m] = ampl*cos(M_PI*phs/180);
                        im[m] = ampl*sin(M_PI*phs/180);
                        reTot += re[m];
                        imTot += im[m];
//                        logger << "\t" << re[m] << "\t" << im[m] << std::endl;
                    }
                    
                    // Compute the average: (sum)/(count)
                    reTot /= numAvgPts;
                    imTot /= numAvgPts;
                    
                    // Compute the variance: E[ (x-u)(x-u)* ], where E[] is the expected
                    // (average) value, x is a measurement, u is the mean of all measurements,
                    // and * is the complex conjugate.
                    double varianceTot = 0;
                    for(int m = 0; m < numAvgPts; m++) {
                        double R = re[m] - reTot;
                        double I = im[m] - imTot;
                        varianceTot += R*R + I*I;
                    }
                    varianceTot /= numAvgPts;
                    
                    // Compute the standard deviation: the square root of the variance
                    stdDev = sqrt(varianceTot);
//                    logger << reTot << "\t" << imTot << "\t" << stdDev << std::endl;
                    
                    // Convert the average value back to (amplitude-phase) form
                    ampl = sqrt(reTot*reTot + imTot*imTot);
                    phs  = 180*atan2(imTot, reTot)/M_PI;
                    
                    // Clean up the dynamically allocated array.
                    delete[] re;
                    delete[] im;
                }
                
                // WRITE MEASURED VALUE TO OUTPUT STREAM
                if(currParam == SWEEP_CUSTOM) {
                    outps << prefix<<customX[i]<<'\t'<<customY[i]<<'\t'<<ampl;
                } else {
                    outps << prefix<<currVal<<'\t'<<ampl;
                }
                
                if(lockin->isPhaseAccessible()) {
                    outps << '\t' << phs;
                }
                
                if(averaging) {
                    outps << '\t' << stdDev;
                }
                
                outps << std::endl;
                if(i % 10 == 0) {
                    outps.flush();
                }
            }
            if(cancelSweep) {
                logCanceledSweep();
                exitVal = 0;
                break;
            }
        }
        /* RESET THE SWEEP CHANGES
         * Reset the current parameter to its initial value, e.g. (min freq) or
         * (min amplitude). As of V1.0.8, this has been moved inside the "repeats"
         * loop, rather than occurring after the repeats loop.
         */
        
        (*LockinSettings::settingsLogger) << "At level " << recursionLevel << " of sweep; "
                << "ramping down to initial value." << std::endl;
        if(rampType == 1 || currParam == SWEEP_CUSTOM) {
            for( --i; i >= 0; i--) {
                if(currParam == SWEEP_CUSTOM) {
                    sendCommandToLockin(SWEEP_X, customX[i]);
                    sendCommandToLockin(SWEEP_Y, customY[i]);
                } else {
                    double currVal;
                    if(sweepSetup.logSpacing[currParam]) {
                        currVal = pow(10, currStart + stepSize*i);
                    } else {
                        currVal = currStart + stepSize*i;
                    }
                    sendCommandToLockin(currParam, currVal);
                }
            }
        } else if(rampType == 2) {
            double endDecade = log10(sweepSetup.ends[currParam]);
            double startDecade = log10(sweepSetup.starts[currParam]);
            double decades = abs(endDecade - startDecade);
            int steps = (int) ceil(STEPS_PER_RAMP_DECADE * decades);
            double rampStep = (endDecade - startDecade) / steps;
            for(int m = steps-1; m >= 0; m--) {
                double currVal = pow(10, startDecade + rampStep*m);
                sendCommandToLockin(currParam, currVal);
            }
        }
    }
    
    if(sweepSetup.autoSens) {
        lockin->set_sensitivity(initialSens0);
    }
    if(recursionLevel == 0) {
        time_t now = time(0);
        struct tm* tmNow = localtime(&now);
        (*LockinSettings::settingsLogger) << std::endl << "=== SWEEP ENDED at " 
                << (1900 + tmNow->tm_year) << "-" << (1 + tmNow->tm_mon) << "-"
                << tmNow->tm_mday << " " << tmNow->tm_hour << ":" << tmNow->tm_min
                << ":" << tmNow->tm_sec << " ===" << std::endl;
        outps.flush();
        outps.close();
        cancelSweep = true;
    }
    return exitVal;
}

/*
 * Log the time that the sweep was canceled.
 */
void logCanceledSweep()
{
    time_t now = time(0);
    struct tm* tmNow = localtime(&now);
    (*LockinSettings::settingsLogger) << std::endl << "=== SWEEP CANCELED at " 
            << (1900 + tmNow->tm_year) << "-" << (1 + tmNow->tm_mon) << "-"
            << tmNow->tm_mday << " " << tmNow->tm_hour << ":" << tmNow->tm_min
            << ":" << tmNow->tm_sec << " ===" << std::endl;
}

/*
 * Command the lock-in to set a parameter to a new value.
 */
void sendCommandToLockin(int currParam, double currVal)
{
    switch(currParam) {
        case SWEEP_F:
            lockin->set_frequency(currVal);
        break;
        case SWEEP_X:
            lockin->set_auxout1(currVal);
        break;
        case SWEEP_Y:
            lockin->set_auxout2(currVal);
        break;
        case SWEEP_A:
            lockin->set_reference_amplitude(currVal);
        break;
    }
}

/*
 * Estimate the duration of a sweep based on the user-provided wait times.
 */
long calculateSweepDuration()
{
    long millis = 0;
    for(int i = numActiveParams-1; i >= 0; i--) {
        if(sweepSetup.parameters[i] == SWEEP_CUSTOM) {
            millis += sweepSetup.waits[sweepSetup.parameters[i]];
            millis *= numCustom;
        } else {
            millis += sweepSetup.waits[sweepSetup.parameters[i]];
            millis *= sweepSetup.steps[sweepSetup.parameters[i]] + 1;
            millis *= sweepSetup.repeats[sweepSetup.parameters[i]];
        }
    }
    return millis;
}

/*
 * Update the tree in the GUI to show the nested levels of the recursive sweep.
 */
void populateTree(HWND tree)
{
    HTREEITEM parent;
    std::ostringstream desc;
    for(int i = 0; i < numActiveParams; i++) {
        desc << PARAM_DESCRIPTIONS[sweepSetup.parameters[i]];
        desc << " from " << sweepSetup.starts[sweepSetup.parameters[i]] << " to ";
        desc << sweepSetup.ends[sweepSetup.parameters[i]];
        char charDesc[80];
        const char* s = desc.str().data();
        strcpy(charDesc, s);

        TVITEM tvi;
        tvi.pszText = charDesc;
        tvi.mask = TVIF_TEXT;

        TVINSERTSTRUCT tviStruct;
        tviStruct.hParent = (i == 0)?TVI_ROOT:parent;
        tviStruct.hInsertAfter = TVI_FIRST;
        tviStruct.item = tvi;

        HTREEITEM tmp = (HTREEITEM) SendMessage(
            tree, TVM_INSERTITEM, 0, (LPARAM) (LPTVINSERTSTRUCT) &tviStruct
        );


        TreeView_Expand(tree, parent, TVE_EXPAND);
        parent = tmp;
        desc.str( std::string() );
        desc.clear();
    }
    desc << "Detection Harmonic: " << sweepSetup.detHarm;
    char charDesc[80];
    strcpy(charDesc, desc.str().data());
    TVITEM tvi;
    tvi.pszText = charDesc;
    tvi.mask = TVIF_TEXT;
    TVINSERTSTRUCT tviStruct;
    tviStruct.hParent = TVI_ROOT;
    tviStruct.hInsertAfter = TVI_LAST;
    tviStruct.item = tvi;
    SendMessage(tree, TVM_INSERTITEM, 0, (LPARAM) (LPTVINSERTSTRUCT) &tviStruct);
}

