#include "stdafx.h"

#ifndef _WINDOWS
#include "opengl_utils.h"
#endif

using namespace std;

namespace OpenGLUtils {

void GenericErrorDumper(const char* text) {
    cerr << text << endl;

#ifdef _WINDOWS
    OLECHAR winfo[MAX_PATH+1];
    MultiByteToWideChar(CP_ACP, 0, text, -1, winfo, MAX_PATH);

    MessageBox(NULL,winfo,TEXT("ERROR"),MB_OK|MB_ICONEXCLAMATION);
#endif
}

void GenericErrorDumper(string& text) {
    GenericErrorDumper(text.data());
}

void DumpOpenGLError(GLenum status) {
    switch (status)
    {
        case GL_NO_ERROR:
            GenericErrorDumper("GL_NO_ERROR: No error has been recorded.");
            break;
        case GL_INVALID_ENUM:
            GenericErrorDumper("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
            break;
        case GL_INVALID_VALUE:
            GenericErrorDumper("GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
            break;
        case GL_INVALID_OPERATION:
            GenericErrorDumper("GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            GenericErrorDumper("GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.");
            break;
        case GL_OUT_OF_MEMORY:
            GenericErrorDumper("GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");
            break;
        case GL_STACK_UNDERFLOW:
            GenericErrorDumper("GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.");
            break;
        case GL_STACK_OVERFLOW:
            GenericErrorDumper("GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.");
            break;
        default:
            GenericErrorDumper("GL_NO_ERROR: No error has been recorded.");
            break;
    }
}

void DumpDisplayError(long status) {
    switch (status)
    {
        case DISP_CHANGE_BADDUALVIEW:
            GenericErrorDumper("DISP_CHANGE_BADDUALVIEW: The settings change was unsuccessful because the system is DualView capable.");
            break;
        case DISP_CHANGE_BADFLAGS:
            GenericErrorDumper("DISP_CHANGE_BADFLAGS: An invalid set of flags was passed in.");
            break;
        case DISP_CHANGE_BADMODE:
            GenericErrorDumper("DISP_CHANGE_BADMODE: The graphics mode is not supported.");
            break;
        case DISP_CHANGE_BADPARAM:
            GenericErrorDumper("DISP_CHANGE_BADPARAM: An invalid parameter was passed in. This can include an invalid flag or combination of flags.");
            break;
        case DISP_CHANGE_FAILED:
            GenericErrorDumper("DISP_CHANGE_FAILED: The display driver failed the specified graphics mode.");
            break;
        case DISP_CHANGE_NOTUPDATED:
            GenericErrorDumper("DISP_CHANGE_NOTUPDATED: Unable to write settings to the registry.");
            break;
        case DISP_CHANGE_RESTART:
            GenericErrorDumper("DISP_CHANGE_RESTART: The computer must be restarted for the graphics mode to work.");
            break;
        default:
            GenericErrorDumper("Muh?");
            break;
    }
}

GLboolean CheckForOpenGLErrors() {
    GLenum status = glGetError();
    if (status != GL_NO_ERROR) {
        DumpOpenGLError(status);
        return GL_FALSE;
    }
    return GL_TRUE;
}

MouseInfo::MouseInfo() :
    m_leftdown(GL_FALSE),
    m_rightdown(GL_FALSE),
    m_xdelta(0),
    m_ydelta(0) {}

void MouseInfo::ResetAll() {
    m_leftdown = GL_FALSE;
    m_rightdown = GL_FALSE;
    ResetMouseMove();
}

void DisplayContext::CalcPerspective() {
    // Calculate The Aspect Ratio Of The Window
    float aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
    m_Perspective = PerspectiveMatrix<float>(45.0f, aspect, m_Near, m_Far);
    cout << "Width = " << GetWidth() << ", Height = " << GetHeight() << ", aspect = " << aspect << endl;
}

void DisplayContext::ReSizeGLScene(uint32_t l_width, uint32_t l_height) {
    m_width = l_width;
    m_height = l_height;
    ReSizeGLScene();
}

void DisplayContext::ReSizeGLScene()        // Resize And Initialize The GL Window
{
    if (m_height==0)                                     // Prevent A Divide By Zero By
    {
        m_height = 1;                                      // Making Height Equal One
    }

    glViewport(0,0,m_width,m_height);                    // Reset The Current Viewport

    // Calculate The Aspect Ratio Of The Window
    CalcPerspective();
}

#ifdef _WINDOWS
LRESULT CALLBACK WndProc( HWND    hWnd,             // Handle For This Window
                          UINT    uMsg,             // Message For This Window
                          WPARAM  wParam,           // Additional Message Information
                          LPARAM  lParam)           // Additional Message Information
{
    WindowsDisplayContext* l_context = reinterpret_cast<WindowsDisplayContext*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (l_context == NULL) {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    } else {
        return l_context->MsgProc(hWnd, uMsg, wParam, lParam);
    }
}

WindowsDisplayContext::WindowsDisplayContext() {
    m_hDC=NULL;
    m_hRC=NULL;
    m_hWnd=NULL;
    if (true) {
        m_fullscreen = false;
        SetSize(1200, 900);
        SetOffset(200, 50);
    } else {
        m_fullscreen = true;
        SetSize(1680, 1050);
        SetOffset(0, 0);
    }
}

void WindowsDisplayContext::ReSizeWindow() {
    RECT WindowRect;

    // Need to do this before we set size, as these might trigger WM_SIZE which could change size back again
    SetWindowLongPtr(m_hWnd, GWL_EXSTYLE, GetdwExStyle());
    SetWindowLongPtr(m_hWnd, GWL_STYLE, GetdwStyle());

    if (m_fullscreen) {
        SetSize(1680, 1050);
        SetOffset(0, 0);
    } else {
        SetSize(1200, 900);
        SetOffset(200, 50);
    }

    WindowRect.left=(long)0;               // Set Left Value To 0
    WindowRect.right=(long)GetWidth();     // Set Right Value To Requested Width
    WindowRect.top=(long)0;                // Set Top Value To 0
    WindowRect.bottom=(long)GetHeight();   // Set Bottom Value To Requested Height

    AdjustWindowRectEx(&WindowRect, GetdwStyle(), FALSE, GetdwExStyle());        // Adjust Window To True Requested Size
    SetWindowPos(m_hWnd, 0, GetOffsetX(), GetOffsetY(), GetWidth(), GetHeight(), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    ReSizeGLScene();
}

DWORD WindowsDisplayContext::GetdwExStyle() {
    DWORD dwExStyle;
    dwExStyle = WS_EX_APPWINDOW;
    if (!m_fullscreen) {
        dwExStyle |= WS_EX_WINDOWEDGE;
    }
    return dwExStyle;
}

DWORD WindowsDisplayContext::GetdwStyle() {
    DWORD dwStyle;
    dwStyle = WS_CLIPSIBLINGS |
              WS_CLIPCHILDREN;
    if (m_fullscreen) {
        dwStyle |= WS_POPUP;
    } else {
        dwStyle |= WS_OVERLAPPEDWINDOW;
    }
    return dwStyle;
}

bool WindowsDisplayContext::SetDisplayMode() {
    if (m_fullscreen) {
        DEVMODE dmScreenSettings;                                // Device Mode
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));    // Makes Sure Memory's Cleared
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);        // Size Of The Devmode Structure
        dmScreenSettings.dmPelsWidth    = GetWidth();            // Selected Screen Width
        dmScreenSettings.dmPelsHeight   = GetHeight();           // Selected Screen Height
        dmScreenSettings.dmBitsPerPel   = m_bits;                // Selected Bits Per Pixel
        dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

        // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
        // ChangeDisplaySettingsEx(NULL, &dmScreenSettings, m_hWnd, CDS_FULLSCREEN, NULL)
        long status = ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
        if (status != DISP_CHANGE_SUCCESSFUL) {
            // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
            DumpDisplayError(status);
            if (MessageBox(NULL,L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",L"ERROR",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
            {
                m_fullscreen=FALSE;        // Windowed Mode Selected.  Fullscreen = FALSE
            }
            else
            {
                // Pop Up A Message Box Letting User Know The Program Is Closing.
                MessageBox(NULL,L"Program Will Now Close.",L"ERROR",MB_OK|MB_ICONSTOP);
                return FALSE;
            }
        }
    } else {
        if (ChangeDisplaySettings(0, 0) != DISP_CHANGE_SUCCESSFUL) {
            MessageBox(NULL,L"Failed to clear display settings.",L"ERROR",MB_OK|MB_ICONSTOP);
            return FALSE;
        }
    }

    return TRUE;
}

bool WindowsDisplayContext::Init(const wchar_t * title, int bits) {
    WNDCLASS    wc;                        // Windows Class Structure
    HINSTANCE   hInstance;
    HGLRC       tempContext;

    hInstance           = GetModuleHandle(NULL);                // Grab An Instance For Our Window
    wc.style            = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;   // Redraw On Size, And Own DC For Window.
    wc.lpfnWndProc      = (WNDPROC) WndProc;                    // WndProc Handles Messages
    wc.cbClsExtra       = 0;                                    // No Extra Window Data
    wc.cbWndExtra       = 0;                                    // No Extra Window Data
    wc.hInstance        = hInstance;                            // Set The Instance
    wc.hIcon            = LoadIcon(NULL, IDI_WINLOGO);          // Load The Default Icon
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
    wc.hbrBackground    = NULL;                                 // No Background Required For GL
    wc.lpszMenuName     = NULL;                                 // We Don't Want A Menu
    wc.lpszClassName    = L"OpenGL";                            // Set The Class Name

    if (!RegisterClass(&wc))                                    // Attempt To Register The Window Class
    {
        MessageBox(NULL,L"Failed To Register the OpenGL Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                                            // Return FALSE
    }

    // Set bits per pixel and set display mode
    m_bits = bits;
    if (!SetDisplayMode()) {
        return FALSE;
    }

    ShowCursor(FALSE);                                           // Hide Mouse Pointer

    // Create The Window
    if (!(m_hWnd=CreateWindowEx(  GetdwExStyle(),                     // Extended Style For The Window
                                  L"OpenGL",                          // Class Name
                                  title,                              // Window Title
                                  GetdwStyle(),
                                  GetOffsetX(), GetOffsetY(),         // Window Position
                                  GetWidth(),                         // Calculate Window Width
                                  GetHeight(),                        // Calculate Window Height
                                  NULL,                               // No Parent Window
                                  NULL,                               // No Menu
                                  hInstance,                          // Instance
                                  NULL)))                             // Dont Pass Anything To WM_CREATE
    {
        KillGLWindow();                                // Reset The Display
        MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                                // Return FALSE
    }

    GLuint  PixelFormat;                            // Holds The Results After Searching For A Match

    static    PIXELFORMATDESCRIPTOR pfd=                // pfd Tells Windows How We Want Things To Be
    {
        sizeof(PIXELFORMATDESCRIPTOR),                // Size Of This Pixel Format Descriptor
        1,                                            // Version Number
        PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
        PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
        PFD_DOUBLEBUFFER,                            // Must Support Double Buffering
        PFD_TYPE_RGBA,                                // Request An RGBA Format
        m_bits,                                        // Select Our Color Depth
        0, 0, 0, 0, 0, 0,                            // Color Bits Ignored
        0,                                            // No Alpha Buffer
        0,                                            // Shift Bit Ignored
        0,                                            // No Accumulation Buffer
        0, 0, 0, 0,                                    // Accumulation Bits Ignored
        32,                                            // 32Bit Z-Buffer (Depth Buffer)  
        0,                                            // No Stencil Buffer
        0,                                            // No Auxiliary Buffer
        PFD_MAIN_PLANE,                                // Main Drawing Layer
        0,                                            // Reserved
        0, 0, 0                                        // Layer Masks Ignored
    };
    
    if (!(m_hDC=GetDC(m_hWnd)))                            // Did We Get A Device Context?
    {
        KillGLWindow();                                // Reset The Display
        MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                                // Return FALSE
    }

    if (!(PixelFormat=ChoosePixelFormat(m_hDC,&pfd)))    // Did Windows Find A Matching Pixel Format?
    {
        KillGLWindow();                                // Reset The Display
        MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                                // Return FALSE
    }

    if(!SetPixelFormat(m_hDC,PixelFormat,&pfd))        // Are We Able To Set The Pixel Format?
    {
        KillGLWindow();                                // Reset The Display
        MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                                // Return FALSE
    }

    // Create a temporary context for setting up GLEW
    if (!(tempContext=wglCreateContext(m_hDC)))
    {
        KillGLWindow();
        MessageBox(NULL,L"Can't create a temporary GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
    if(!wglMakeCurrent(m_hDC, tempContext))
    {
        KillGLWindow();
        MessageBox(NULL,L"Can't activate the temporary GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }

    // Now initialise GLEW and try to create a new OpenGL 3.1 context
    if (glewInit() != GLEW_OK)
    {
        KillGLWindow();
        MessageBox(NULL,L"Can't initialise GLEW.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;
    }
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, // WGL_CONTEXT_DEBUG_BIT_ARB
        //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
        0
    };

    if(wglewIsSupported("WGL_ARB_create_context") == 1)
    {
        if (!(m_hRC=wglCreateContextAttribsARB(m_hDC, 0, attribs)))
        {
            KillGLWindow();
            wglDeleteContext(tempContext);
            MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
            return FALSE;
        }
        wglMakeCurrent(NULL,NULL);
        wglDeleteContext(tempContext);
        if(!wglMakeCurrent(m_hDC, m_hRC))
        {
            KillGLWindow();
            wglDeleteContext(tempContext);
            MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
            return FALSE;
        }
        //int OpenGLVersion[2];
        //glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
        //glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);

        //MessageBox(NULL,TEXT("Arse"),TEXT("OpenGL status"),MB_OK|MB_ICONINFORMATION);
    }
    else
    {
        MessageBox(NULL,L"WGL_ARB_create_context not supported, using temp context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        m_hRC = tempContext;
    }

    // Userdata in m_hWnd is a void pointer to WindowsDisplayContext it's a member of  
    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    ShowWindow(m_hWnd,SW_SHOW);                       // Show The Window
    SetForegroundWindow(m_hWnd);                      // Slightly Higher Priority
    SetFocus(m_hWnd);                                 // Sets Keyboard Focus To The Window
    ReSizeGLScene();                                  // Set Up Our Perspective GL Screen

    //glShadeModel(GL_SMOOTH);                            // Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                // Black Background
    //glDepthRange(0.0f, 1.0f);
    glClearDepthf(1.0f);                                    // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                                // The Type Of Depth Testing To Do
    glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);

    if (!CheckForOpenGLErrors()) {
        return FALSE;
    }

    return TRUE;                                    // Success
}

void WindowsDisplayContext::KillGLWindow()                                // Properly Kill The Window
{
    HINSTANCE hInstance = GetModuleHandle(NULL);                // Grab An Instance For Our Window

    if (m_fullscreen)                                        // Are We In Fullscreen Mode?
    {
        ChangeDisplaySettings(NULL,0);                    // If So Switch Back To The Desktop
    }
    ShowCursor(TRUE);                                     // Show Mouse Pointer

    if (m_hRC)                                            // Do We Have A Rendering Context?
    {
        if (!wglMakeCurrent(NULL,NULL))                    // Are We Able To Release The DC And RC Contexts?
        {
            MessageBox(NULL,L"Release Of DC And RC Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(m_hRC))                        // Are We Able To Delete The RC?
        {
            MessageBox(NULL,L"Release Rendering Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        }
        m_hRC=NULL;                                        // Set RC To NULL
    }

    if (m_hDC && !ReleaseDC(m_hWnd,m_hDC))                    // Are We Able To Release The DC
    {
        MessageBox(NULL,L"Release Device Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        m_hDC=NULL;                                        // Set DC To NULL
    }

    if (m_hWnd && !DestroyWindow(m_hWnd))                    // Are We Able To Destroy The Window?
    {
        MessageBox(NULL,L"Could Not Release hWnd.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        m_hWnd=NULL;                                        // Set hWnd To NULL
    }

    if (!UnregisterClass(L"OpenGL",hInstance))            // Are We Able To Unregister Class
    {
        MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
    }
}

LRESULT CALLBACK WindowsDisplayContext::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void WindowsDisplayContext::SwapBuff()
{
    if (!SwapBuffers(m_hDC)) {
        MessageBox(NULL,TEXT("Failed to swap buffers."),TEXT("ERROR"),MB_OK|MB_ICONEXCLAMATION);
    }
}

bool WindowsDisplayContext::RegisterInputs() {
    RAWINPUTDEVICE Rid[1];
        
    Rid[0].usUsagePage = 0x01; 
    Rid[0].usUsage = 0x02; 
    Rid[0].dwFlags = RIDEV_NOLEGACY |       // adds HID mouse and also ignores legacy mouse messages
                     RIDEV_CAPTUREMOUSE;    // Don't go to another window if click is outside this one
    Rid[0].hwndTarget = m_hWnd;

    if (RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])) == FALSE) {
        //registration failed. Call GetLastError for the cause of the error
        MessageBox(NULL,L"Failed register raw input devices.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return false;
    }
    return true;
}


#endif

#ifdef _EGL
EGLBoolean EGLDisplayContext::Init(const char * title, const EGLint l_attribList[], const bool always_native) {
    EGLint numConfigs;
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    EGLConfig config;
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

    m_Xdisplay = NULL;

    if (!always_native)
    {
        m_Xdisplay = XOpenDisplay(NULL);
    }
    if (always_native || m_Xdisplay == NULL)
    {
        int32_t success = 0;
   
        static EGL_DISPMANX_WINDOW_T nativewindow;

        DISPMANX_ELEMENT_HANDLE_T dispman_element;
        DISPMANX_DISPLAY_HANDLE_T dispman_display;
        DISPMANX_UPDATE_HANDLE_T dispman_update;
        VC_RECT_T dst_rect;
        VC_RECT_T src_rect;
   
        if (!always_native)
        {
            cout << "Failed to connect to XDisplay, using native display" << endl;
        }
    m_XWindows = GL_FALSE;

        bcm_host_init();

        // create an EGL window surface, passing context width/height
        success = graphics_get_display_size(0 /* LCD */, &m_width, &m_height);
        if ( success < 0 )
        {
           cerr << "Failed to create window context" << endl;
           return EGL_FALSE;
        }
   
        dst_rect.x = 0;
        dst_rect.y = 0;
        dst_rect.width = m_width;
        dst_rect.height = m_height;

        src_rect.x = 0;
        src_rect.y = 0;
        src_rect.width = m_width << 16;
        src_rect.height = m_height << 16;   

        dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
        dispman_update = vc_dispmanx_update_start( 0 );
         
        dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
            0/*layer*/, &dst_rect, 0/*src*/,
            &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, static_cast<DISPMANX_TRANSFORM_T>(0)/*transform*/);
      
        nativewindow.element = dispman_element;
        nativewindow.width = m_width;
        nativewindow.height = m_height;
        vc_dispmanx_update_submit_sync( dispman_update );
   
        m_hWnd = &nativewindow;
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    } else {
        cout << "Using XDisplay" << endl;
        m_XWindows = GL_TRUE;

        Window root;
        XSetWindowAttributes swa;
        XSetWindowAttributes  xattr;
        Atom wm_state;
        XWMHints hints;
        XEvent xev;
        EGLConfig ecfg;
        EGLint num_config;
        Window win;

        root = DefaultRootWindow(m_Xdisplay);

        // Fixed window size initially
        m_width = 320;
        m_height = 240;

        swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;
        win = XCreateWindow(
                   m_Xdisplay, root,
                   0, 0, m_width, m_height, 0,
                   CopyFromParent, InputOutput,
                   CopyFromParent, CWEventMask,
                   &swa );

        xattr.override_redirect = GL_FALSE;
        XChangeWindowAttributes (m_Xdisplay, win, CWOverrideRedirect, &xattr);

        hints.input = GL_TRUE;
        hints.flags = InputHint;
        XSetWMHints(m_Xdisplay, win, &hints);

        // make the window visible on the screen
        XMapWindow (m_Xdisplay, win);
        XStoreName (m_Xdisplay, win, title);

        // get identifiers for the provided atom name strings
        wm_state = XInternAtom (m_Xdisplay, "_NET_WM_STATE", GL_FALSE);

        memset ( &xev, 0, sizeof(xev) );
        xev.type                 = ClientMessage;
        xev.xclient.window       = win;
        xev.xclient.message_type = wm_state;
        xev.xclient.format       = 32;
        xev.xclient.data.l[0]    = 1;
        xev.xclient.data.l[1]    = GL_FALSE;
        XSendEvent (
            m_Xdisplay,
            DefaultRootWindow (m_Xdisplay),
            GL_FALSE,
            SubstructureNotifyMask,
            &xev );

        m_hWnd = (EGLNativeWindowType) win;
        display = eglGetDisplay((EGLNativeDisplayType)m_Xdisplay);
    }

    if (display == EGL_NO_DISPLAY)
    {
        cerr << "Failed to get display" << endl;
        DumpError(eglGetError());
        return GL_FALSE;
    }
   
    if (!eglInitialize(display, &majorVersion, &minorVersion))
    {
        cerr << "Failed to initialise display" << endl;
        DumpError(eglGetError());
        return GL_FALSE;
    }
     
    if (!eglGetConfigs(display, NULL, 0, &numConfigs))
    {
        cerr << "Failed to get configs" << endl;
        DumpError(eglGetError());
        return GL_FALSE;
    }

    if (!eglChooseConfig(display, l_attribList, &config, 1, &numConfigs))
    {
        cerr << "Failed to choose config" << endl;
        DumpError(eglGetError());
        return GL_FALSE;
    }

    surface = eglCreateWindowSurface(display, config, static_cast<EGLNativeWindowType>(m_hWnd), NULL);
    if (surface == EGL_NO_SURFACE)
    {
        cerr << "Failed to create surface" << endl;
        DumpError(eglGetError());
        return GL_FALSE;
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT)
    {
        cerr << "Failed to create context" << endl;
        DumpError(eglGetError());
        return GL_FALSE;
    }

    if (!eglMakeCurrent(display, surface, surface, context))
    {
        cerr << "Failed to make display current" << endl;
        DumpError(eglGetError());
        return GL_FALSE;
    }
   
    // We got through all the above, therefore success! 
    m_eglDisplay = auto_ptr<EGLDisplay>(new EGLDisplay(display));
    m_eglContext = auto_ptr<EGLDisplay>(new EGLContext(context));
    m_eglSurface = auto_ptr<EGLSurface>(new EGLSurface(surface));

    CalcPerspective();

    return GL_TRUE;  
}

void EGLDisplayContext::SwapBuff() {
    eglSwapBuffers(*m_eglDisplay.get(), *m_eglContext.get());
}

int DisplayContext::TakeScreenShot(const char * filename)
{
    // Try opening a file for writing first, as that's a likely failure mechanism
    FILE* outfile;
    if ((outfile = fopen(filename, "wb")) == NULL)
    {
        cerr << "ERROR: can't open" << filename << endl;
        return 1;
    }

    // Good to go so do the bulk of the init now
    int quality = 50;
    struct jpeg_compress_struct cinfo;    // basic info for JPEG properties
    struct jpeg_error_mgr jerr;           // in case of error
    JSAMPROW row_pointer[1];              // pointer to JSAMPLE row[s]
    int row_stride;                       // physical row width in image buffer

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);
 
    cinfo.image_width = m_width;
    cinfo.image_height = m_height;
    cinfo.input_components = 3;         // number of color components per pixel
    cinfo.in_color_space = JCS_RGB;     // colorspace of input image as RGB
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    // Read the pixels from the frame buffer into an array
    unsigned char pixels[cinfo.image_width * cinfo.image_height * 3];
    glReadPixels(0, 0, cinfo.image_width, cinfo.image_height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Now the science bit
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = cinfo.image_width * 3;        // JSAMPLEs per row in image_buffer
 
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &pixels[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
        (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
            // NOTE: jpeg_write_scanlines expects an array of pointers to scanlines.
            //       Here the array is only one element long, but you could pass
            //       more than one scanline at a time if that's more convenient.
 
    //## FINISH COMPRESSION AND CLOSE FILE:
 
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
 
    clog << "Dumped OpenGL framebuffer to " << filename << endl;
 
    return 0;
}
#endif

#ifdef _EGL
void DumpError(EGLint l_error)
{
    switch (l_error)
    {
        case EGL_SUCCESS:
            cerr << "The last function succeeded without error." << endl;
            break;
        case EGL_NOT_INITIALIZED:
            cerr << "EGL is not initialized, or could not be initialized, for the specified EGL display connection." << endl;
            break;
        case EGL_BAD_ACCESS:
            cerr << "EGL cannot access a requested resource (for example a context is bound in another thread)." << endl;
            break;
        case EGL_BAD_ALLOC:
            cerr << "EGL failed to allocate resources for the requested operation." << endl;
            break;
        case EGL_BAD_ATTRIBUTE:
            cerr << "An unrecognized attribute or attribute value was passed in the attribute list." << endl;
            break;
        case EGL_BAD_CONTEXT:
            cerr << "An EGLContext argument does not name a valid EGL rendering context." << endl;
            break;
        case EGL_BAD_CONFIG:
            cerr << "An EGLConfig argument does not name a valid EGL frame buffer configuration." << endl;
            break;
        case EGL_BAD_CURRENT_SURFACE:
            cerr << "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid." << endl;
            break;
        case EGL_BAD_DISPLAY:
            cerr << "An EGLDisplay argument does not name a valid EGL display connection." << endl;
            break;
        case EGL_BAD_SURFACE:
            cerr << "An EGLSurface argument does not name a valid surface (window, pixel buffer or pixmap) configured for GL rendering." << endl;
            break;
        case EGL_BAD_MATCH:
            cerr << "Arguments are inconsistent (for example, a valid context requires buffers not supplied by a valid surface)." << endl;
            break;
        case EGL_BAD_PARAMETER:
            cerr << "One or more argument values are invalid." << endl;
            break;
        case EGL_BAD_NATIVE_PIXMAP:
            cerr << "A NativePixmapType argument does not refer to a valid native pixmap." << endl;
            break;
        case EGL_BAD_NATIVE_WINDOW:
            cerr << "A NativeWindowType argument does not refer to a valid native window." << endl;
            break;
        default:
            cerr << "Couldn't decode error (" << l_error << ")" << endl;
            break;
    }

}
#endif

#ifndef _WINDOWS
struct my_error_mgr
{
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message) (cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}
#endif


GLuint CreateTextureFromFile(const char * filename) {
    GLuint textureId;

#ifdef _WINDOWS
    HDC            hdcTemp;                                             // The DC To Hold Our Bitmap
    HBITMAP        hbmpTemp;                                            // Holds The Bitmap Temporarily
    OLECHAR szPath[MAX_PATH+1];                                         // Full Path To Picture
    OLECHAR wfilename[MAX_PATH+1];
    IPicture *pPicture;                                                 // IPicture Interface
    long        lWidth;                                                 // Width In Logical Units
    long        lHeight;                                                // Height In Logical Units
    long        lWidthPixels;                                           // Width In Pixels
    long        lHeightPixels;                                          // Height In Pixels
    GLint        glMaxTexDim ;                                          // Holds Maximum Texture Size

    GetCurrentDirectory(MAX_PATH, szPath);
    wcscat_s(szPath, TEXT("\\"));                                            // Append "\" After The Working Directory
    MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, MAX_PATH);
    wcscat_s(szPath, wfilename);
    HRESULT hr = OleLoadPicturePath(szPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

    if (FAILED(hr)) {                                                   // If Loading Failed
        MessageBox(NULL,L"Failed to load picture.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;                                                    // Return False
    }

    hdcTemp = CreateCompatibleDC(GetDC(0));                                // Create The Windows Compatible Device Context
    if(!hdcTemp)                                                        // Did Creation Fail?
    {
        pPicture->Release();                                            // Decrements IPicture Reference Count
        return FALSE;                                                    // Return False (Failure)
    }

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);                    // Get Maximum Texture Size Supported
    
    pPicture->get_Width(&lWidth);                                        // Get IPicture Width (Convert To Pixels)
    lWidthPixels    = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
    pPicture->get_Height(&lHeight);                                        // Get IPicture Height (Convert To Pixels)
    lHeightPixels    = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

    // Resize Image To Closest Power Of Two
    if (lWidthPixels <= glMaxTexDim) // Is Image Width Less Than Or Equal To Cards Limit
        lWidthPixels = 1 << (int)floor((log((double)lWidthPixels)/log(2.0f)) + 0.5f); 
    else  // Otherwise  Set Width To "Max Power Of Two" That The Card Can Handle
        lWidthPixels = glMaxTexDim;
 
    if (lHeightPixels <= glMaxTexDim) // Is Image Height Greater Than Cards Limit
        lHeightPixels = 1 << (int)floor((log((double)lHeightPixels)/log(2.0f)) + 0.5f);
    else  // Otherwise  Set Height To "Max Power Of Two" That The Card Can Handle
        lHeightPixels = glMaxTexDim;
    
    //    Create A Temporary Bitmap
    BITMAPINFO    bi = {0};                                                // The Type Of Bitmap We Request
    DWORD        *pBits = 0;                                                // Pointer To The Bitmap Bits

    bi.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);                // Set Structure Size
    bi.bmiHeader.biBitCount        = 32;                                    // 32 Bit
    bi.bmiHeader.biWidth        = lWidthPixels;                            // Power Of Two Width
    bi.bmiHeader.biHeight        = lHeightPixels;                        // Make Image Top Up (Positive Y-Axis)
    bi.bmiHeader.biCompression    = BI_RGB;                                // RGB Encoding
    bi.bmiHeader.biPlanes        = 1;                                    // 1 Bitplane

    //    Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
    hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);
    
    if(!hbmpTemp)                                                        // Did Creation Fail?
    {
        DeleteDC(hdcTemp);                                                // Delete The Device Context
        pPicture->Release();                                            // Decrements IPicture Reference Count
        return FALSE;                                                    // Return False (Failure)
    }

    SelectObject(hdcTemp, hbmpTemp);                                    // Select Handle To Our Temp DC And Our Temp Bitmap Object

    // Render The IPicture On To The Bitmap
    pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);
    //pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, 0, lWidth, 0, 0);

    // Convert From BGR To RGB Format And Add An Alpha Value Of 255
    for(long i = 0; i < lWidthPixels * lHeightPixels; i++)                // Loop Through All Of The Pixels
    {
        BYTE* pPixel    = (BYTE*)(&pBits[i]);                            // Grab The Current Pixel
        BYTE  temp        = pPixel[0];                                    // Store 1st Color In Temp Variable (Blue)
        pPixel[0]        = pPixel[2];                                    // Move Red Value To Correct Position (1st)
        pPixel[2]        = temp;                                            // Move Temp Value To Correct Blue Position (3rd)

        // This Will Make Any Black Pixels, Completely Transparent        (You Can Hardcode The Value If You Wish)
        if ((pPixel[0]==0) && (pPixel[1]==0) && (pPixel[2]==0))            // Is Pixel Completely Black
            pPixel[3]    =   0;                                            // Set The Alpha Value To 0
        else                                                            // Otherwise
            pPixel[3]    = 255;                                            // Set The Alpha Value To 255
    }

#else
    unsigned char * big_buff;
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;

    FILE * infile;        
    JSAMPARRAY buffer;    
    int row_stride;        

    if ((infile = fopen(filename, "rb")) == NULL)
    {
        cerr << "can't open " << filename << endl;
        return 0;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) 
    {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        cerr << "jpeg error" << endl;
        return 0;
    }
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    (void) jpeg_read_header(&cinfo, TRUE);
    (void) jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray)
             ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
    big_buff = (unsigned char *) malloc(cinfo.output_height * cinfo.output_width * cinfo.output_components);

    while (cinfo.output_scanline < cinfo.output_height)
    {
        JDIMENSION read_now = jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(&big_buff[(cinfo.output_height - (cinfo.output_scanline - read_now) - 1) * cinfo.output_width * cinfo.output_components], buffer[0], row_stride);
        //memcpy(&big_buff[(cinfo.output_scanline - read_now) * cinfo.output_width * cinfo.output_components], buffer[0], row_stride);
    }
#endif

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

#ifdef _WINDOWS
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);

    DeleteObject(hbmpTemp);                                                // Delete The Object
    DeleteDC(hdcTemp);                                                    // Delete The Device Context

    pPicture->Release();                                                // Decrements IPicture Reference Count
#else
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, cinfo.output_width, cinfo.output_height, 0, GL_RGB, GL_UNSIGNED_BYTE, big_buff);

    free(big_buff);    

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
#endif

    glGenerateMipmap (GL_TEXTURE_2D);

    if (textureId == 0)
    {
        cerr << "Failed to create texture" << endl;
#ifdef _WINDOWS
        MessageBox(NULL,TEXT("Failed to create texture."),TEXT("ERROR"),MB_OK|MB_ICONEXCLAMATION);
#endif
        return GL_FALSE;
    }

    return textureId;

}


//
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
    GLuint shader;
    GLint compiled;
   
    // Create the shader object
    shader = glCreateShader ( type );

    if ( shader == 0 )
        return 0;

    // Load the shader source
    glShaderSource ( shader, 1, &shaderSrc, NULL );
   
    // Compile the shader
    glCompileShader ( shader );

    // Check the compile status
    glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

    if ( !compiled ) 
    {
        GLint infoLen = 0;

        glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
      
        if ( infoLen > 1 )
        {
            GLchar* infoLog = (GLchar*) malloc (sizeof(GLchar) * infoLen );
            glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
            cout << "Error compiling shader:" << endl;
            cout << infoLog << endl;

#ifdef _WINDOWS
            OLECHAR winfo[MAX_PATH+1];
            MultiByteToWideChar(CP_ACP, 0, infoLog, -1, winfo, MAX_PATH);

            MessageBox(NULL,winfo,TEXT("ERROR"),MB_OK|MB_ICONEXCLAMATION);

#endif
            free ( infoLog );
        }
#ifdef _WINDOWS
        else {
            MessageBox(NULL,L"Failed to compile program.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
        }
#endif

        glDeleteShader ( shader );
        return 0;
    }

   return shader;

}

GLuint LoadProgram(const string& vShaderStr, const string& fShaderStr, const ParamBindings& AttrBindings)
{
   GLuint programObject;
   GLuint vertexShader;
   GLuint fragmentShader;
   GLint linked;

   // Load the vertex/fragment shaders
   vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr.data() );
   fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr.data() );

   // Create the program object
   programObject = glCreateProgram ( );
   
   if ( programObject == 0 )
      return 0;

   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   // Bind attributes
   if (!AttrBindings.empty()) {
       map<int, string>::const_iterator it;
       for (it = AttrBindings.begin(); it != AttrBindings.end(); it++) {
           glBindAttribLocation(programObject, it->first, it->second.data());
       }
   }

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked ) 
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         GLchar* infoLog = (GLchar*) malloc (sizeof(GLchar) * infoLen );
         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         cout << "Error linking program:" << endl;
         cout << infoLog << endl;                
         
         free ( infoLog );
      }

#ifdef _WINDOWS
     MessageBox(NULL,L"Failed to link program.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
#endif

      glDeleteProgram ( programObject );
      return 0;
   }

   return programObject;
}

} // namespace OpenGLUtils
