// offender.cpp
//
// by Andy Royal Nov 2012

#include "stdafx.h"

#ifndef _WINDOWS
//#include <iostream>   // For std::cout
#include <sys/time.h>
//#include <GLES2/gl2.h>
//#include <EGL/egl.h>
//#include <math.h>
#include "opengl_utils.h"
#endif

//#define MEASURE_FRAME_RATE

using namespace Offender;

#ifdef _WINDOWS
bool active = true;
bool keys[256];            // Array Used For The Keyboard Routine
//signed int xPos, yPos;
#endif;

#ifdef _WINDOWS
class myContext : public WindowsDisplayContext {
#else
class myContext : public EGLDisplayContext {
#endif
    public:
        GLboolean Initialise(const char *);
#ifdef _WINDOWS
        virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
};

GLboolean myContext::Initialise(const char * l_title) {
#ifdef _EGL
    const EGLint attribList[] =
    {
        EGL_RED_SIZE,       5,
        EGL_GREEN_SIZE,     6,
        EGL_BLUE_SIZE,      5,
        EGL_ALPHA_SIZE,     EGL_DONT_CARE,
        EGL_DEPTH_SIZE,     8,
        EGL_STENCIL_SIZE,   EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, 0,
        EGL_NONE
    };
    bool success = Init(l_title, l_attribList, m_always_native);
#endif
#ifdef _WINDOWS
    OLECHAR wtitle[MAX_PATH+1];

    MultiByteToWideChar(CP_ACP, 0, l_title, -1, wtitle, MAX_PATH);
    bool success = Init(wtitle,16) && RegisterInputs();
#endif

    return success;
}

World* Init (DisplayContext* l_Context)
{
    World* l_world = new World(l_Context);    // A whole new world
    l_world->Register<Player>("Player");

    RenderGroup* l_rendergroup = l_world->GetRendergroup();

#ifdef SCREENSHOT
    Object* l_player = l_world->GetInstance("Player", ObjPos(0.0f, 1300.0f, 3000.0f),
                                                      ObjVec(0.0f, 0.0f, 0.0f),
                                                      ObjQuat(cos(static_cast<OBJ_NUMTYPE>(-M_PI)/16.0f), sin(static_cast<OBJ_NUMTYPE>(-M_PI)/16.0f), 0.0f, 0.0f));

#else
    Object* l_player = l_world->GetInstance("Player", ObjPos(0.0f, 700.0f, 2500.0f),
                                                      ObjVec(0.0f, 0.0f, 0.0f),
                                                      ObjQuat(1.0f, 0.0f, 0.0f, 0.0f));
#endif
    l_rendergroup->SetupBuffers();
    l_world->SetCameraTarget(l_player);

    return l_world;
}

#ifdef _WINDOWS
LRESULT CALLBACK myContext::MsgProc( HWND    hWnd,             // Handle For This Window
                                     UINT    uMsg,             // Message For This Window
                                     WPARAM  wParam,           // Additional Message Information
                                     LPARAM  lParam)           // Additional Message Information
{
    static bool resetmouse = true;
    static signed int xPos, yPos;

    switch (uMsg)                                    // Check For Windows Messages
    {
        case WM_ACTIVATE:                            // Watch For Window Activate Message
        {
            // LoWord Can Be WA_INACTIVE, WA_ACTIVE, WA_CLICKACTIVE,
            // The High-Order Word Specifies The Minimized State Of The Window Being Activated Or Deactivated.
            // A NonZero Value Indicates The Window Is Minimized.
            if ((LOWORD(wParam) != WA_INACTIVE) && !((BOOL)HIWORD(wParam))) {
                active=TRUE;                        // Program Is Active
                resetmouse = true;
            } else {
                active=FALSE;                        // Program Is No Longer Active
                MouseInfo()->ResetAll();
            }

            return 0;                                // Return To The Message Loop
        }

        case WM_SYSCOMMAND:                            // Intercept System Commands
        {
            switch (wParam)                            // Check System Calls
            {
                case SC_SCREENSAVE:                    // Screensaver Trying To Start?
                case SC_MONITORPOWER:                // Monitor Trying To Enter Powersave?
                return 0;                            // Prevent From Happening
            }
            break;                                    // Exit
        }

        case WM_CLOSE:                                // Did We Receive A Close Message?
        {
            PostQuitMessage(0);                        // Send A Quit Message
            return 0;                                // Jump Back
        }

        case WM_KEYDOWN:                            // Is A Key Being Held Down?
        {
            keys[wParam] = TRUE;                    // If So, Mark It As TRUE
            return 0;                                // Jump Back
        }

        case WM_KEYUP:                                // Has A Key Been Released?
        {
            keys[wParam] = FALSE;                    // If So, Mark It As FALSE
            return 0;                                // Jump Back
        }

        // Don't worry about resizing as with raw input we can only switch between windowed and fullscreen which is entirely within our control
        //case WM_SIZE:                                // Resize The OpenGL Window
        //{
        //    ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
        //    return 0;                                // Jump Back
        //}

        case WM_INPUT:
        {
            UINT dwSize;

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, 
                            sizeof(RAWINPUTHEADER));
            LPBYTE lpb = new BYTE[dwSize];
            if (lpb == NULL) 
            {
                return 0;
            } 

            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize )
            {
                MessageBox(NULL,L"GetRawInputData returned inconsistent size.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
            }

            RAWINPUT* raw = (RAWINPUT*)lpb;
            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                MouseInfo()->AddMouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
            }

            USHORT buttons = raw->data.mouse.usButtonFlags;
            if (buttons & RI_MOUSE_LEFT_BUTTON_DOWN)
                MouseInfo()->SetLeftDown(GL_TRUE);
            if (buttons & RI_MOUSE_LEFT_BUTTON_UP)
                MouseInfo()->SetLeftDown(GL_FALSE);
            if (buttons & RI_MOUSE_RIGHT_BUTTON_DOWN)
                MouseInfo()->SetRightDown(GL_TRUE);
            if (buttons & RI_MOUSE_RIGHT_BUTTON_UP)
                MouseInfo()->SetRightDown(GL_FALSE);

            delete[] lpb;
            return 0;
        }
    }

    // Pass All Unhandled Messages To DefWindowProc
    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain( HINSTANCE    hInstance,            // Instance
                    HINSTANCE    hPrevInstance,        // Previous Instance
                    LPSTR        lpCmdLine,            // Command Line Parameters
                    int          nCmdShow)             // Window Show State

{
//#ifdef _DEBUG
//    int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
//    tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
//    tmpDbgFlag |= _CRTDBG_CHECK_CRT_DF;
//    tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
//    _CrtSetDbgFlag(tmpDbgFlag);
//#endif

    myContext LocalContext;
    MSG       msg;                                       // Windows Message Structure
    BOOL      done=FALSE;                                // Bool Variable To Exit Loop
#ifdef MEASURE_FRAME_RATE
    clock_t   start_time, end_time;
    uint32_t  frames = ~uint32_t(0);
#endif

    // Create Our OpenGL Window
    if (!LocalContext.Initialise("Offender"))
    {
        return 0;                                    // Quit If Window Was Not Created
    }
    auto_ptr<World> world(Init(&LocalContext));

    if (world.get() == NULL)
    {
        return 0;                                    // Quit If World Was Not Created
    }

//#ifdef _DEBUG
//    _ASSERTE(_CrtCheckMemory());
//    _CrtDbgReport(_CRT_ASSERT, NULL, NULL, "Offender", NULL);
//    if (!_CrtCheckMemory())    // Do a memory check
//    {
//        MessageBox(NULL,L"CRTCheckMemory failed.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
//        return 0;
//    }
//#endif

    while(!done)                                    // Loop That Runs While done=FALSE
    {
        if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))    // Is There A Message Waiting?
        {
            if (msg.message==WM_QUIT)                // Have We Received A Quit Message?
            {
                done=TRUE;                            // If So done=TRUE
            }
            else                                    // If Not, Deal With Window Messages
            {
                TranslateMessage(&msg);                // Translate The Message
                DispatchMessage(&msg);                // Dispatch The Message
            }
        }
        else                                        // If There Are No Messages
        {
            //SetCursorPos(xPos, yPos);
            // Draw The Scene.
            if ((active && !world->Iteration()) || keys[VK_ESCAPE])
            {
                done=TRUE;
            }
            else                                    // Not Time To Quit, Update Screen
            {
                if (CheckForOpenGLErrors()) {
                    LocalContext.SwapBuff();
#ifdef MEASURE_FRAME_RATE
                    frames++;
                    if (frames == 0)
                        start_time = clock();
                    end_time = clock();
#endif
                } else {
                    return 0;
                }
            }

            if (keys[VK_F1])                         // Is F1 Being Pressed?
            {
                keys[VK_F1]=FALSE;                   // If So Make Key FALSE
                LocalContext.ToggleFullScreen();     // Toggle Fullscreen / Windowed Mode
                LocalContext.ReSizeWindow();
            }
        }
    }

#ifdef MEASURE_FRAME_RATE
    char message[1000];
    sprintf(message, "Average frame rate %.2fHz\n", static_cast<float>(CLOCKS_PER_SEC * frames / (end_time - start_time)));
    OLECHAR winfo[MAX_PATH+1];
    MultiByteToWideChar(CP_ACP, 0, message, -1, winfo, MAX_PATH);
    MessageBox(NULL,winfo,TEXT("Average frame rate"),MB_OK|MB_ICONEXCLAMATION);
#endif

    // Shutdown
    LocalContext.KillGLWindow();                     // Kill The Window
    return (msg.wParam);                             // Exit The Program
    
}
#endif
