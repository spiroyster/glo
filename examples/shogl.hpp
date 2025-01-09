#ifndef SHOGL_HPP
#define SHOGL_HPP

#include <functional>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>

// Forward declarations...
void shogl_init_impl();
class shogl_window; 
extern shogl_window* shogl();

// Platform macros...
#ifndef SHOGL_WIN
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define SHOGL_WIN
#endif
#endif

#ifndef SHOGL_X
#if defined(__linux__) || defined(__unix__)
#define SHOGL_X
#endif
#endif


#ifdef SHOGL_X

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>

// GL Function macros...
#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))glXGetProcAddress((const GLubyte *)#name);
#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))glXGetProcAddress((const GLubyte *)#name);

//#define SHOGL_QUIT(value) PostQuitMessage(value);
#define SHOGL_CONTEXT dynamic_cast<x_context*>(shogl()->window_context());

#endif // SHOGL_X


#ifdef SHOGL_WIN

#pragma comment(lib, "opengl32.lib")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

#include <Windows.h>
#include <gl/GL.h>
#include "glext.h"
#include "wglext.h"

// GL Function macros...
#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);
#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);

#define SHOGL_QUIT(value) PostQuitMessage(value);
#define SHOGL_CONTEXT dynamic_cast<win_context*>(shogl()->window_context());

#endif


// Macros...


#define SHOGL_CLASS(shogl_window_class) SHOGL_IMPL \
namespace { static std::unique_ptr<shogl_window_class> instance_; } \
shogl_window* shogl() { if (!instance_) { instance_ = std::make_unique<shogl_window_class>(); } return instance_.get(); } \
void shogl_init_impl()

// Window no class (callbacks)...
#define SHOGL() SHOGL_IMPL \
namespace { static std::unique_ptr<shogl_window> instance_; } \
shogl_window* shogl() { if (!instance_) { instance_ = std::make_unique<shogl_window>(); } return instance_.get(); } \
void shogl_init_impl()


// GL versions...
#ifdef SHOGL_OPENGL_10 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_11 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_12 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 2;
#elif defined SHOGL_OPENGL_13 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 3;
#elif defined SHOGL_OPENGL_14 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 4;
#elif defined SHOGL_OPENGL_15 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 5;
#elif defined SHOGL_OPENGL_20 
constexpr int shogl_version_major = 2; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_21 
constexpr int shogl_version_major = 2; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_30 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_31 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_32 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 2;
#elif defined SHOGL_OPENGL_33 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 3;
#elif defined SHOGL_OPENGL_40 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_41 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_42 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 2;
#elif defined SHOGL_OPENGL_43 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 3;
#elif defined SHOGL_OPENGL_44 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 4;
#elif defined SHOGL_OPENGL_45 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 5;
#elif defined SHOGL_OPENGL_46 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 6;
#else 
constexpr int shogl_version_major = 0; constexpr int shogl_version_minor = 0;
#endif


class shogl_window
{
    std::string window_title_;
    int window_width_ = 300;
    int window_height_ = 300;
    bool fullscreen_ = false;
    int quit_ = -1;
    int fps_ = 0;
    std::vector<std::string> args_;
    
public:
    
    enum mouse_button
    {
        none = 0,
        left,
        middle,
        right
    };

    class context 
    {
    protected:
        int major_;
        int minor_;
    public:
        virtual void make_current(void*) = 0;
        virtual void free() = 0;

        int major() const { return major_; }
        int minor() const { return minor_; }

        virtual void window_title(const std::string& title) = 0;
        virtual void window_size(int width, int height) = 0;
        virtual void window_redraw() = 0;
        virtual void window_quit(int exit_code) = 0;
    };

    typedef std::function<void()> idle_fn;
    typedef std::function<void()> draw_fn;
    typedef std::function<void()> kill_fn;
    typedef std::function<void(int, int)> resize_fn;
    typedef std::function<void(int, int)> mouse_move_fn;
    typedef std::function<void(int, int, mouse_button)> mouse_down_fn;
    typedef std::function<void(int, int, mouse_button)> mouse_up_fn;
    typedef std::function<void(int, int, int)> mouse_scroll_fn;
    typedef std::function<void(int, int, unsigned int)> key_up_fn;
    typedef std::function<void(int, int, unsigned int)> key_down_fn;
    typedef std::function<void(std::vector<std::wstring>)> drag_drop_fn;
    
private:
    idle_fn idle_fn_ = nullptr;
    draw_fn draw_fn_ = nullptr;
    kill_fn kill_fn_ = nullptr;
    resize_fn resize_fn_ = nullptr;
    mouse_move_fn mouse_move_fn_ = nullptr;
    mouse_up_fn mouse_up_fn_ = nullptr;
    mouse_down_fn mouse_down_fn_ = nullptr;
    mouse_scroll_fn mouse_scroll_fn_ = nullptr;
    key_up_fn key_up_fn_ = nullptr;
    key_down_fn key_down_fn_ = nullptr;
    drag_drop_fn drag_drop_fn_ = nullptr;
    std::shared_ptr<context> context_;
    std::chrono::steady_clock::time_point fps_time_stamp_ = std::chrono::steady_clock::now();
    
public:
    shogl_window() {}

    shogl_window(const std::string& title, int width, int height)
        :   window_title_(title), window_width_(width), window_height_(height)
    {}
    
    // Events...

    void idle(idle_fn fn) { idle_fn_ = fn; }
    virtual void idle() { idle_fn_ ? idle_fn_() : (void)0; }
    
    void draw(draw_fn fn) { draw_fn_ = fn; }
    virtual void draw() { draw_fn_ ? draw_fn_() : (void)0; }
    
    void kill(kill_fn fn) { kill_fn_ = fn; }
    virtual void kill() { kill_fn_ ? kill_fn_() : (void)0; }
    
    void resize(resize_fn fn) { resize_fn_ = fn; }
    virtual void resize(int width, int height) { resize_fn_ ? resize_fn_(width, height) : (void)0; }
    
    void mouse_move(resize_fn fn) { mouse_move_fn_ = fn; }
    virtual void mouse_move(int x, int y) { mouse_move_fn_ ? mouse_move_fn_(x, y) : (void)0; }
    
    void mouse_down(mouse_down_fn fn) { mouse_down_fn_ = fn; }
    virtual void mouse_down(int x, int y, mouse_button button) { mouse_down_fn_ ? mouse_down_fn_(x, y, button) : (void)0; }
    
    void mouse_up(mouse_up_fn fn) { mouse_up_fn_ = fn; }
    virtual void mouse_up(int x, int y, mouse_button button) { mouse_up_fn_ ? mouse_up_fn_(x, y, button) : (void)0; }
    
    void mouse_scroll(mouse_scroll_fn fn) { mouse_scroll_fn_ = fn; }
    virtual void mouse_scroll(int x, int y, int value) { mouse_scroll_fn_ ? mouse_scroll_fn_(x, y, value) : (void)0; }
    
    void key_down(key_down_fn fn) { key_down_fn_ = fn; }
    virtual void key_down(int x, int y, unsigned int key) { key_down_fn_ ? key_down_fn_(x, y, key) : (void)0; }
    
    void key_up(key_up_fn fn) { key_up_fn_ = fn; }
    virtual void key_up(int x, int y, unsigned int key) { key_up_fn_ ? key_up_fn_(x, y, key) : (void)0; }

    void drag_drop(drag_drop_fn fn) { drag_drop_fn_ = fn; }
    virtual void drag_drop(std::vector<std::wstring> files) { drag_drop_fn_ ? drag_drop_fn_(files) : (void)0; }

    // Attributes (these can be set at any time)...
    
    const std::string& window_title() const { return window_title_; }
    void window_title(const std::string& title) 
    { 
        window_title_ = title;
        if (context_)
            context_->window_title(title);
    }

    bool window_fullscreen() const { return fullscreen_; }
    void window_fullscreen(bool fs) { fullscreen_ = fs; }
    
    int window_height() const { return window_height_; }
    int window_width() const { return window_width_; }
    
    void window_size(int width, int height)
    {
        if (context_)
            context_->window_size(width, height);
        assign_window_size(width, height);
    }

    void assign_window_size(int width, int height) { window_width_ = width; window_height_ = height; }

    int window_fps() const { return fps_; }
    void window_fps(int fps) { fps_ = fps; }

    void window_context(std::shared_ptr<context> ctxt) { context_ = ctxt; }
    context* window_context() { return context_.get(); }

    void redraw() { if (context_) { context_->window_redraw(); } }
    bool frame_limiter()
    {
        if (!fps_)
            return true;

        auto fps_now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(fps_now - fps_time_stamp_).count() > (1000 / fps_))
        {
            fps_time_stamp_ = fps_now;
            return true;
        }
        return false;
    }

    void window_quit(int exit_code) 
    { 
        quit_ = exit_code; 
        if (context_)
            context_->window_quit(exit_code);
    }
    bool window_quit() const { return quit_ >= 0; }
};


#ifdef SHOGL_X

class x_context : public shogl_window::context
{
public:

    GLXContext              glc_;
    Display*                dpy_;
    Window                  root_;
    XVisualInfo*            vi_;
    Colormap                cmap_;
    XSetWindowAttributes    swa_;
    Window                  win_;
    XWindowAttributes       gwa_;
    XEvent                  xev_;

    x_context()
    {
        dpy_ = XOpenDisplay(NULL);
        if(!dpy_) 
            throw std::runtime_error("cannot connect to X server");
        
        root_ = DefaultRootWindow(dpy_);
        GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        vi_ = glXChooseVisual(dpy_, 0, att);
        if(!vi_) 
            throw std::runtime_error("no appropriate visual found");
        
        cmap_ = XCreateColormap(dpy_, root_, vi_->visual, AllocNone);
        swa_.colormap = cmap_;
        swa_.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPress | ButtonReleaseMask;
        win_ = XCreateWindow(dpy_, root_, 0, 0, 300, 300, 0, vi_->depth, InputOutput, vi_->visual, CWColormap | CWEventMask, &swa_);
        XMapWindow(dpy_, win_);
        
        glc_ = glXCreateContext(dpy_, vi_, NULL, GL_TRUE);
        make_current(nullptr);

        printf("OpenGL %s\n", glGetString(GL_VERSION));
    }

    void make_current(void*)
    {
        glXMakeCurrent(dpy_, win_, glc_);
    }
    
    void free()
    {
        glXMakeCurrent(dpy_, None, NULL);
        glXDestroyContext(dpy_, glc_);
    }

    void window_title(const std::string& title)
    {
        XStoreName(dpy_, win_, title.c_str());
    }
        
    void window_size(int width, int height)
    {
        XResizeWindow(dpy_, win_, width, height);
    }
        
    void window_redraw() {}

    void window_quit(int exit_code) {}
};

static void process_event(XEvent& event)
{
    switch (event.type)
    {
    case MapNotify:
        break;
    case DestroyNotify:
        shogl()->kill();
        break;
    case ButtonPress:
        {
            switch (event.xbutton.button)
            {
            case 1:
                shogl()->mouse_down(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::left);
                break;
            case 2:
                shogl()->mouse_down(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::middle);
                break;
            case 3:
                shogl()->mouse_down(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::right);
                break;
            case 4:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, 1);
                break;
            case 5:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, -1);
                break;
            default:
                break;
            }
        }
        break;
    case ButtonRelease:
        {
            switch (event.xbutton.button)
            {
            case 1:
                shogl()->mouse_up(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::left);
                break;
            case 2:
                shogl()->mouse_up(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::middle);
                break;
            case 3:
                shogl()->mouse_up(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::right);
                break;
            case 4:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, 1);
                break;
            case 5:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, -1);
                break;
            default:
                break;
            }
        }
        break;
    case MotionNotify:
        {
            switch (event.xbutton.button)
            {
            case 1:
                shogl()->mouse_move(event.xbutton.x, event.xbutton.y);
                break;
            case 2:
                shogl()->mouse_move(event.xbutton.x, event.xbutton.y);
                break;
            case 3:
                shogl()->mouse_move(event.xbutton.x, event.xbutton.y);
                break;
            default:
                break;
            }
        }
        break;
    case KeyPress:
        {
            shogl()->key_down(event.xbutton.x, event.xbutton.y, event.xkey.keycode);
        }
        break;
    case KeyRelease:
        {
            shogl()->key_up(event.xbutton.x, event.xbutton.y, event.xkey.keycode);
        }
        break;
    default:
        break;
    }
}

static int run_x()
{
    auto ctx = dynamic_cast<x_context*>(shogl()->window_context());
    XStoreName(ctx->dpy_, ctx->win_, shogl()->window_title().c_str());
    if (shogl()->window_fps())
    {
        while (!shogl()->window_quit())
        {
            // Check resize...
            XGetWindowAttributes(ctx->dpy_, ctx->win_, &ctx->gwa_);
            if (shogl()->window_width() != ctx->gwa_.width || shogl()->window_height() != ctx->gwa_.height)
            {
                shogl()->resize(ctx->gwa_.width, ctx->gwa_.height);
                shogl()->assign_window_size(ctx->gwa_.width, ctx->gwa_.height);
            }

            // Process event(s) if any...
            if(XCheckWindowEvent(ctx->dpy_, ctx->win_, KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask, &ctx->xev_))
                process_event(ctx->xev_);
            
            // Draw if time for frame...
            if (shogl()->frame_limiter())
            {
                shogl()->draw();
                glXSwapBuffers(ctx->dpy_, ctx->win_);
            }

            // Idling...
            shogl()->idle();
        }
    }
    else
    {
        while (!shogl()->window_quit())
        {
            XNextEvent(ctx->dpy_, &ctx->xev_);
            XGetWindowAttributes(ctx->dpy_, ctx->win_, &ctx->gwa_);
            if (shogl()->window_width() != ctx->gwa_.width || shogl()->window_height() != ctx->gwa_.height)
            {
                shogl()->resize(ctx->gwa_.width, ctx->gwa_.height);
                shogl()->assign_window_size(ctx->gwa_.width, ctx->gwa_.height);
            }

            // Process event(s) if any...
            process_event(ctx->xev_);
            
            shogl()->draw();
            glXSwapBuffers(ctx->dpy_, ctx->win_);
            
            // Idling...
            shogl()->idle();
        }
    }
    shogl()->kill();
    ctx->free();
    XDestroyWindow(ctx->dpy_, ctx->win_);
    XCloseDisplay(ctx->dpy_);
    return 0;
}

// For this to work, we first need to create the context since GL functions maybe members of shogl_window so valid 
// context will need to exist before resolving them via shogl_window instantiation...
#define SHOGL_IMPL int main(int argc, char **argv) \
{ \
    std::shared_ptr<x_context> ctx = std::make_shared<x_context>(); \
    shogl_init_impl(); \
    shogl()->window_context(ctx); \
    return run_x(); \
} 

#endif // SHOGL_X


#ifdef SHOGL_WIN

static std::string get_last_error_as_string()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) { return std::string(); }//No error message has been recorded
    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}

static std::wstring ansi_to_wide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], count);
    return wstr;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        shogl()->window_quit(0);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        shogl()->draw();
        SwapBuffers(GetDC(hWnd));
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SIZE:
    {
        shogl()->resize(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
        shogl()->assign_window_size(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
        break;
    }
    case WM_MOUSEWHEEL:
    {
        POINT pt; pt.x = LOWORD(lParam); pt.y = HIWORD(lParam); ScreenToClient(hWnd, &pt);
        shogl()->mouse_scroll(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    }
    case WM_MOUSEMOVE:
    {
        shogl()->mouse_move(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam));
        break;
    }
    case WM_LBUTTONDOWN:
    {
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::left);
        SetCapture(hWnd);
        break;
    }
    case WM_LBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        shogl()->mouse_up(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::left);
        ReleaseCapture();
        break;
    }
    case WM_MBUTTONDOWN:
    {
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::middle);
        SetCapture(hWnd);
        break;
    }
    case WM_MBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        shogl()->mouse_up(x, shogl()->window_height() - y, shogl_window::mouse_button::middle);
        ReleaseCapture();
        break;
    }
    case WM_RBUTTONDOWN:
    {
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::right);
        SetCapture(hWnd);
        break;
    }
    case WM_RBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        shogl()->mouse_up(x, shogl()->window_height() - y, shogl_window::mouse_button::right);
        ReleaseCapture();
        break;
    }
    case WM_KEYDOWN:
    {
        int ll = LOWORD(lParam);
        int hl = HIWORD(lParam);
        int lw = LOWORD(wParam);
        int hw = HIWORD(wParam);
        POINT pt; GetCursorPos(&pt); ScreenToClient(hWnd, &pt);
        shogl()->key_down(pt.x, shogl()->window_height() - pt.y, static_cast<unsigned int>(wParam));
        break;
    }
    case WM_KEYUP:
    {
        POINT pt; GetCursorPos(&pt); ScreenToClient(hWnd, &pt);
        shogl()->key_up(pt.x, shogl()->window_height() - pt.y, static_cast<unsigned int>(wParam));
        break;
    }
    case WM_DROPFILES:
    {
        TCHAR szName[MAX_PATH];
        HDROP hDrop = (HDROP)wParam;
        int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

        std::vector<std::wstring> files;
        for (int i = 0; i < count; i++)
        {
            DragQueryFile(hDrop, i, szName, MAX_PATH);
            files.emplace_back(szName);
        }
        DragFinish(hDrop);
        shogl()->drag_drop(files);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

class win_context : public shogl_window::context
{
public:

    HWND hWnd_;
    HGLRC hGLrc_;
    HINSTANCE hInst_;
    WNDCLASSEX wcex_;

    win_context(HINSTANCE hInstance)
    {
        CoInitialize(nullptr);

        wcex_.cbSize = sizeof(WNDCLASSEX);
        wcex_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex_.lpfnWndProc = WndProc;
        wcex_.cbClsExtra = 0;
        wcex_.cbWndExtra = 0;
        wcex_.hInstance = hInst_;
        wcex_.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex_.lpszMenuName = NULL;
        wcex_.lpszClassName = L"glwindow";

        if (!RegisterClassEx(&wcex_))
            throw std::runtime_error("Unable to register Window class");

        hWnd_ = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW | WS_EX_ACCEPTFILES, wcex_.lpszClassName, L"glwindow", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, NULL, NULL, hInst_, NULL);

        if (!hWnd_)
            throw std::runtime_error(get_last_error_as_string());

        hInst_ = hInstance;
        
        HDC hDC = static_cast<HDC>(GetDC(hWnd_));
        if (!hDC)
            throw std::runtime_error("Unable to get handle device context.");

        // we then need to get a valid DC...
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int nPixelFormat = ChoosePixelFormat(hDC, &pfd);

        if (nPixelFormat == 0)
            throw std::runtime_error("Unable to choose pixel format.");

        if (!SetPixelFormat(hDC, nPixelFormat, &pfd))
            throw std::runtime_error("Unable to set pixel format.");

        hGLrc_ = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hGLrc_);

        // Get a valid context for requested version...
        std::vector<std::vector<int>> gl_version = { { shogl_version_major, shogl_version_minor } };
        
        // If GL version is not set, attempt all...
        if (!shogl_version_major)
            gl_version = 
            {
                { 4, 6 }, { 4, 5 }, { 4, 4 }, { 4, 3 }, { 4, 2 }, { 4, 1 }, { 4, 0 },
                { 3, 3 }, { 3, 2 }, { 3, 1 }, { 3, 0 },
                { 2, 2 }, { 2, 1 }, { 2, 0 },
                { 1, 5 }, { 1, 4 }, { 1, 3 }, { 1, 2 }, { 1, 1 }
            };
        
        PFNWGLCREATECONTEXTATTRIBSARBPROC glwindow_context_extended_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        if (glwindow_context_extended_wglCreateContextAttribsARB)
        {
            auto tmpContext = hGLrc_;

            // Keep trying to make a valid context...
            hGLrc_ = NULL;
            for (unsigned int ver = 0; ver < gl_version.size() && !hGLrc_; ++ver)
            {
                int attribs[] =
                {
                    WGL_CONTEXT_MAJOR_VERSION_ARB, gl_version[ver][0],
                    WGL_CONTEXT_MINOR_VERSION_ARB, gl_version[ver][1],
                    WGL_CONTEXT_FLAGS_ARB, 0,
                    0
                };
                hGLrc_ = glwindow_context_extended_wglCreateContextAttribsARB(hDC, 0, attribs);
                major_ = gl_version[ver][0];
                minor_ = gl_version[ver][1];
            }

            if (!hGLrc_)
                throw std::runtime_error("Unable to make current.");

            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(tmpContext);

            if (!wglMakeCurrent(hDC, hGLrc_))
                throw std::runtime_error("Unable to make current.");
        }
    }

    void make_current(void* hdc) { wglMakeCurrent(static_cast<HDC>(hdc), hGLrc_); }
    void free() { wglDeleteContext(hGLrc_); }
    void window_title(const std::string& title) { SetWindowText(hWnd_, ansi_to_wide(title).c_str()); }
    void window_redraw()
    {
        InvalidateRect(hWnd_, NULL, NULL);
        UpdateWindow(hWnd_);
    }
    void window_size(int width, int height)
    {
        RECT rect;
        GetWindowRect(hWnd_, &rect);
        SetWindowPos(hWnd_, HWND_TOP, rect.left, rect.top, width, height, NULL);
    }
    void window_quit(int exit_code) 
    {
        PostQuitMessage(exit_code);
    }
};

static int run_win(int nCmdShow)
{
    try
    {
        auto ctx = dynamic_cast<win_context*>(shogl()->window_context());

        // Set the window title...
        ctx->window_title(shogl()->window_title());
        
        // Resize the window to desired width height...
        ctx->window_size(shogl()->window_width(), shogl()->window_height());

        // Main loop...
        MSG msg; 
        ShowWindow(ctx->hWnd_, nCmdShow);
        UpdateWindow(ctx->hWnd_);

        if (shogl()->window_fps())
        {
            while (!shogl()->window_quit())
            {
                if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    if (msg.message == WM_QUIT)
                        shogl()->window_quit();

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                if (shogl()->frame_limiter())
                    shogl()->redraw();
                
                shogl()->idle();
            }
        }
        else
        {
            while (GetMessage(&msg, NULL, 0, 0))\
            {
                shogl()->idle();
                shogl()->redraw();
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        // Kill
        shogl()->kill();
        ctx->free();
        
        return (int)msg.wParam;
    }
    catch (const std::exception& e) { MessageBox(NULL, ansi_to_wide(e.what()).c_str(), L"GLwindow", MB_ICONERROR); }
    catch (...) { MessageBox(NULL, L"Unknown error", L"GLwindow", MB_ICONERROR); }
    return 0;
}

#define SHOGL_IMPL int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \
{ \
    std::shared_ptr<win_context> ctx = std::make_shared<win_context>(hInstance); \
    shogl_init_impl(); \
    shogl()->window_context(ctx); \
    return run_win(nCmdShow); \
}   


#endif // SHOGL_WIN

#endif // SHOGL_HPP