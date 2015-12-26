#include "stdafx.h"
#include "EditorView.h"
#include "SelectHelper.h"
#include "NodeFactory.h"
#include "keymap.cpp"

USING_NS_CC;

class MyGLView : public GLView
{
public:
    HWND glHwnd;
    HDC  hDC;
    HGLRC hRC;
    MyGLView():glHwnd(NULL),hDC(NULL),hRC(NULL) {}

    bool Init()
    {
        setViewName("MyGLView");

        static PIXELFORMATDESCRIPTOR pfd =              // pfd Tells Windows How We Want Things To Be
        {
            sizeof(PIXELFORMATDESCRIPTOR),              // Size Of This Pixel Format Descriptor
            1,                                          // Version Number
            PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
            PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,                           // Must Support Double Buffering
            PFD_TYPE_RGBA,                              // Request An RGBA Format
            32,                                         // Select Our Color Depth
            0, 0, 0, 0, 0, 0,                           // Color Bits Ignored
            0,                                          // No Alpha Buffer
            0,                                          // Shift Bit Ignored
            0,                                          // No Accumulation Buffer
            0, 0, 0, 0,                                 // Accumulation Bits Ignored
            24,                                         // 24Bit Z-Buffer (Depth Buffer)
            8,                                          // No Stencil Buffer
            0,                                          // No Auxiliary Buffer
            PFD_MAIN_PLANE,                             // Main Drawing Layer
            0,                                          // Reserved
            0, 0, 0                                     // Layer Masks Ignored
        };

        if (!(hDC=GetDC(glHwnd)))                           // Did We Get A Device Context?
        {
            return false;                               // Return FALSE
        }

        int PixelFormat;
        if (!(PixelFormat=ChoosePixelFormat(hDC, &pfd)))    // Did Windows Find A Matching Pixel Format?
        {
            return false;
        }

        if(!SetPixelFormat(hDC,PixelFormat,&pfd))       // Are We Able To Set The Pixel Format?
        {
            return false;
        }

        if (!(hRC=wglCreateContext(hDC)))               // Are We Able To Get A Rendering Context?
        {
            return false;                               // Return FALSE
        }

        if(!wglMakeCurrent(hDC,hRC))                    // Try To Activate The Rendering Context
        {
            wglDeleteContext(hRC);
            return false;                               // Return FALSE
        }

        initGlew();

        // Enable point size by default.
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        return true;
    }

    virtual void end() 
    {
        if(hRC)
        {
            wglMakeCurrent(NULL,NULL);
            wglDeleteContext(hRC);
            hRC = NULL;
        }
        if(hDC){
            ReleaseDC(glHwnd, hDC);
            hDC = NULL;
        }
    }

    /** Get whether opengl render system is ready, subclass must implement this method. */
    virtual bool isOpenGLReady() { return true; }

    /** Exchanges the front and back buffers, subclass must implement this method. */
    virtual void swapBuffers() 
    {
        SwapBuffers(hDC);
    }

    virtual void setIMEKeyboardState(bool open){ }

    virtual HWND getWin32Window() { return glHwnd; };


#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    bool glew_dynamic_binding()
    {
        const char *gl_extensions = (const char*)glGetString(GL_EXTENSIONS);

        // If the current opengl driver doesn't have framebuffers methods, check if an extension exists
        if (glGenFramebuffers == nullptr)
        {
            log("OpenGL: glGenFramebuffers is nullptr, try to detect an extension");
            if (strstr(gl_extensions, "ARB_framebuffer_object"))
            {
                log("OpenGL: ARB_framebuffer_object is supported");

                glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbuffer");
                glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbuffer");
                glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffers");
                glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffers");
                glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorage");
                glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameteriv");
                glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebuffer");
                glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebuffer");
                glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffers");
                glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffers");
                glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatus");
                glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1D");
                glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2D");
                glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3D");
                glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbuffer");
                glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
                glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmap");
            }
            else
                if (strstr(gl_extensions, "EXT_framebuffer_object"))
                {
                    log("OpenGL: EXT_framebuffer_object is supported");
                    glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC) wglGetProcAddress("glIsRenderbufferEXT");
                    glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC) wglGetProcAddress("glBindRenderbufferEXT");
                    glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC) wglGetProcAddress("glDeleteRenderbuffersEXT");
                    glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC) wglGetProcAddress("glGenRenderbuffersEXT");
                    glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC) wglGetProcAddress("glRenderbufferStorageEXT");
                    glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) wglGetProcAddress("glGetRenderbufferParameterivEXT");
                    glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC) wglGetProcAddress("glIsFramebufferEXT");
                    glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC) wglGetProcAddress("glBindFramebufferEXT");
                    glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC) wglGetProcAddress("glDeleteFramebuffersEXT");
                    glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC) wglGetProcAddress("glGenFramebuffersEXT");
                    glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) wglGetProcAddress("glCheckFramebufferStatusEXT");
                    glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC) wglGetProcAddress("glFramebufferTexture1DEXT");
                    glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC) wglGetProcAddress("glFramebufferTexture2DEXT");
                    glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC) wglGetProcAddress("glFramebufferTexture3DEXT");
                    glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) wglGetProcAddress("glFramebufferRenderbufferEXT");
                    glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
                    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC) wglGetProcAddress("glGenerateMipmapEXT");
                }
                else
                {
                    log("OpenGL: No framebuffers extension is supported");
                    log("OpenGL: Any call to Fbo will crash!");
                    return false;
                }
        }
        return true;
    }
#endif

    // helper
    bool initGlew()
    {
#if (CC_TARGET_PLATFORM != CC_PLATFORM_MAC)
        GLenum GlewInitResult = glewInit();
        if (GLEW_OK != GlewInitResult)
        {
            MessageBox((char *)glewGetErrorString(GlewInitResult), "OpenGL error");
            return false;
        }

        if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
        {
            log("Ready for GLSL");
        }
        else
        {
            log("Not totally ready :(");
        }

        if (glewIsSupported("GL_VERSION_2_0"))
        {
            log("Ready for OpenGL 2.0");
        }
        else
        {
            log("OpenGL 2.0 not supported");
        }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        if(glew_dynamic_binding() == false)
        {
            MessageBox("No OpenGL framebuffer support. Please upgrade the driver of your video card.", "OpenGL error");
            return false;
        }
#endif

#endif // (CC_TARGET_PLATFORM != CC_PLATFORM_MAC)
        return true;
    }
};



EditorView::EditorView(void)
{
    sceneRoot = NULL;
    mx = 0;
    my = 0;
    mlb = 0;
}


EditorView::~EditorView(void)
{
}

void EditorView::OnInitialUpdate()
{

}

void EditorView::PreCreate(CREATESTRUCT& cs)
{
    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
}

LRESULT EditorView::OnSize(WPARAM wParam, LPARAM lParam)
{
    Director::getInstance()->getOpenGLView()->setDesignResolutionSize(1024, 615, ResolutionPolicy::FIXED_HEIGHT);
    return 0L;
}

LRESULT EditorView::OnWindowPosChanged(WPARAM wParam, LPARAM lParam)
{
    CRect rect = GetWindowRect();
    Director::getInstance()->getOpenGLView()->setFrameSize((rect.right - rect.left), (rect.bottom - rect.top));
    Director::getInstance()->getOpenGLView()->setDesignResolutionSize((rect.right - rect.left), (rect.bottom - rect.top), ResolutionPolicy::FIXED_HEIGHT);
    //Director::getInstance()->getOpenGLView()->setScissorInPoints(0,0, (rect.right - rect.left), (rect.bottom - rect.top));
    if (sceneRoot)
    {
        sceneRoot->MoveToScreenCenter();
    }
    return 0L;
}

LRESULT EditorView::OnTime(WPARAM wParam, LPARAM lParam)
{
    auto director = Director::getInstance();
    QueryPerformanceCounter(&nNow);
    if (nNow.QuadPart - nLast.QuadPart > _animationInterval.QuadPart)
    {
        nLast.QuadPart = nNow.QuadPart;

        director->mainLoop();
        director->getOpenGLView()->pollEvents();
    }
    return 0L;
}

LRESULT EditorView::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    mlb = (int)EventMouse::MouseEventType::MOUSE_DOWN;
    //EventMouse event(EventMouse::MouseEventType::MOUSE_DOWN);
    //event.setCursorPosition(mx, my);
    //event.setMouseButton(mlb);
    //Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    intptr_t id = 0;
    glview->handleTouchesBegin(1, &id, &mx, &my);
    SetFocus();
    return 0L;
}

LRESULT EditorView::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    mlb = (int)EventMouse::MouseEventType::MOUSE_UP;
    //EventMouse event(EventMouse::MouseEventType::MOUSE_UP);
    //event.setCursorPosition(mx, my);
    //event.setMouseButton(mlb);
    //Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    intptr_t id = 0;
    glview->handleTouchesEnd(1, &id, &mx, &my);
    return 0L;
}

LRESULT EditorView::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    int x_pos = LOWORD(lParam);
    int y_pos = HIWORD(lParam);
    mx = (float)x_pos;
    my = (float)y_pos;
    //EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);
    //event.setCursorPosition(mx, my);
    //Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    if (mlb == (int)EventMouse::MouseEventType::MOUSE_DOWN)
    {
        auto director = Director::getInstance();
        auto glview = director->getOpenGLView();
        intptr_t id = 0;
        //static char buff[256];
        //sprintf(buff,"%f,%f\n",mx, my);
        //TRACE(buff);
        glview->handleTouchesMove(1, &id, &mx, &my);
    }
    return 0L;
}

LRESULT EditorView::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
    int Key = (int)wParam;
    const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.
    if ( Key == VK_SHIFT )
    {
        // this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
        Key = (int)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
    }
    if ( Key == VK_CONTROL )
    {
        Key = (int)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
        // some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
        //if (lParam & 0x1000000)
        //    Key = VK_CONTROL;
    }
    if ( Key == VK_MENU )
    {
        Key = (int)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
        //if (lParam & 0x1000000)
        //    event.KeyInput.Key = irr::KEY_RMENU;
    }

    //event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
    //event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);

    EventKeyboard event(g_EditorKeyCodeMap[Key],true);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&event);
    return 0L;
}

LRESULT EditorView::OnKeyUp(WPARAM wParam, LPARAM lParam)
{

    int Key = (int)wParam;
    const UINT MY_MAPVK_VSC_TO_VK_EX = 3; // MAPVK_VSC_TO_VK_EX should be in SDK according to MSDN, but isn't in mine.
    if ( Key == VK_SHIFT )
    {
        // this will fail on systems before windows NT/2000/XP, not sure _what_ will return there instead.
        Key = (int)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
    }
    if ( Key == VK_CONTROL )
    {
        Key = (int)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
        // some keyboards will just return LEFT for both - left and right keys. So also check extend bit.
        //if (lParam & 0x1000000)
        //    Key = VK_CONTROL;
    }
    if ( Key == VK_MENU )
    {
        Key = (int)MapVirtualKey( ((lParam>>16) & 255), MY_MAPVK_VSC_TO_VK_EX );
        //if (lParam & 0x1000000)
        //    event.KeyInput.Key = irr::KEY_RMENU;
    }
    //event.KeyInput.Shift = ((allKeys[VK_SHIFT] & 0x80)!=0);
    //event.KeyInput.Control = ((allKeys[VK_CONTROL] & 0x80)!=0);
    EventKeyboard event(g_EditorKeyCodeMap[Key],false);
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    dispatcher->dispatchEvent(&event);
    return 0L;
}

LRESULT EditorView::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Override this function in your class derived from CWnd to handle
    //  window messages. A typical function might look like this:
    switch(uMsg)
    {
        case WM_SIZE:				return OnSize(wParam, lParam);
        case WM_WINDOWPOSCHANGED:	return OnWindowPosChanged(wParam, lParam);
        case WM_TIMER:              return OnTime(wParam, lParam);
        case WM_LBUTTONDBLCLK:		return OnLButtonDown(wParam, lParam);
        case WM_LBUTTONDOWN:		return OnLButtonDown(wParam, lParam);
        case WM_LBUTTONUP:			return OnLButtonUp(wParam, lParam);
        case WM_MOUSEMOVE:			return OnMouseMove(wParam, lParam);
        case WM_KEYDOWN:            return OnKeyDown(wParam, lParam);
        case WM_KEYUP:              return OnKeyUp(wParam,lParam);
        //case WM_MOUSEWHEEL:
        //    {
        //        if(sceneRoot)
        //        {
        //            short wheel = (short)HIWORD(wParam) / WHEEL_DELTA;
        //            if (wheel > 0){
        //                sceneRoot->setScale(MIN(4.0,sceneRoot->getScale() + 0.02f));
        //            }
        //            else{
        //                sceneRoot->setScale(MAX(0.1f,(sceneRoot->getScale() - 0.02f)));
        //            }
        //        }

        //    }break;
    }

    // The message functions should return a value recommended by the Windows API documentation.
    // Alternatively, return FinalWindowProc to continue with default processing.

    return CWnd::WndProc(uMsg, wParam, lParam);
}

void EditorView::OnDestroy()
{
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (glview->isOpenGLReady())
    {
        director->end();
        director->mainLoop();
    }
    delete glview;
    PostQuitMessage(0);
}

void EditorView::CreateCocosScene()
{
    // set other opengl default values
    //glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    CRect rect = GetWindowRect();
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {

        MyGLView* view = new MyGLView();
        view->glHwnd = GetHwnd();
        if(view->Init())
        {
            view->setViewPortInPoints(0,0,(rect.right - rect.left), (rect.bottom - rect.top));
            view->setFrameSize((rect.right - rect.left), (rect.bottom - rect.top));
            //view->setDesignResolutionSize((rect.right - rect.left), (rect.bottom - rect.top), ResolutionPolicy::FIXED_HEIGHT);
            director->setOpenGLView(view);
            director->setDisplayStats(true);
            // set FPS. the default value is 1.0/60 if you don't call this
            director->setAnimationInterval(1.0 / 60);
        }
    }

    LARGE_INTEGER nFreq;
    QueryPerformanceFrequency(&nFreq);
    QueryPerformanceCounter(&nLast);

    // create a scene. it's an autorelease object
    auto scene = Scene::create();
    // 'layer' is an autorelease object
    sceneRoot = NodeFactory::CreateRoot();
    // add layer as a child to scene
    scene->addChild(sceneRoot->GetNode());
    // run
    Director::getInstance()->runWithScene(scene);

    gSelectHepler.Init(scene);

    UpdateTopology();

    gSelectHepler.SetSelectNode(sceneRoot, sceneRoot->GetType());

    SetTimer(1, 10, NULL);

    InitEditorKeyCodeMap();
}

void EditorView::UpdateTopology()
{
    GetMyApp()->m_Frame.UpdateTopology();
}


