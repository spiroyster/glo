#ifndef GLOW_HPP
#define GLOW_HPP

#include "glop.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

//// --- Windows Platform ---
//#ifndef GLO_WIN
//
//// Check for windows...
//#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//
//// Define GLO platform...
//#define GLO_WIN
//
//// Windows includes...
//#include <Windows.h>
//#include <gl/GL.h>
//#include "../glext.h"
//#include "../wglext.h"
//
//// GL Function macros...
////#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
////#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
////#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);
////#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);
//
//#define glwindow_str std::wstring
//#define GLWINDOW_QUIT(value) PostQuitMessage(value);
//#define GLWINDOW_ARGS GetCommandLineA()
//
//// Platform specific compiler warning...
//#define GLO_WARNING(msg) 
//
//#endif
//
//#endif
//// --- Windows Platform ---






// forward declare glwindow init...
void glwindow_init_impl();
extern int glwindow_version_major();
extern int glwindow_version_minor();
class glwindow; extern glwindow* glwindow_get();

//// MS Windows Platform specific...
//#ifdef GLO_WIN
//#define glwindow_str std::wstring
//#define GLWINDOW_QUIT(value) PostQuitMessage(value);
//#define GLWINDOW_ARGS GetCommandLineA()
//#endif

// Window creation macros...
#define GLWINDOW_CLASS(major, minor, glwindow_derived) GLWINDOW_IMPL \
int glwindow_version_major() { return major; } \
int glwindow_version_minor() { return minor; } \
namespace { static std::unique_ptr<glwindow_derived> instance_; } \
glwindow* glwindow_get() { if (!instance_) { instance_ = std::make_unique<glwindow_derived>(); } return instance_.get(); } \
void glwindow_init_impl()

#define GLWINDOW(major, minor) GLWINDOW_CLASS(major, minor, glwindow)

// Window function macros...
#define GLWINDOW_KILL void glwindow_kill_impl(); struct glwindow_kill_wrapper { glwindow_kill_wrapper() { \
        glwindow_get()->kill([]() { glwindow_kill_impl(); }); \
} }; glwindow_kill_wrapper glwindow_kill_wrapper_; void glwindow_kill_impl()

#define GLWINDOW_DRAW void glwindow_draw_impl(); struct glwindow_draw_wrapper { glwindow_draw_wrapper() { \
        glwindow_get()->draw([]() { glwindow_draw_impl(); }); \
} }; glwindow_draw_wrapper glwindow_draw_wrapper_; void glwindow_draw_impl()

#define GLWINDOW_IDLE void glwindow_idle_impl(); struct glwindow_idle_wrapper { glwindow_idle_wrapper() { \
        glwindow_get()->idle([]() { glwindow_idle_impl(); }); \
} }; glwindow_idle_wrapper glwindow_idle_wrapper_; void glwindow_idle_impl()

#define GLWINDOW_RESIZE(width, height) void glwindow_resize_impl(int, int); struct glwindow_resize_wrapper { glwindow_resize_wrapper() { \
        glwindow_get()->resize([](int glw_r_w, int glw_r_h) { glwindow_resize_impl(glw_r_w, glw_r_h); }); \
} }; glwindow_resize_wrapper glwindow_resize_wrapper_; void glwindow_resize_impl(int width, int height)

#define GLWINDOW_MOUSE_MOVE(x, y) void glwindow_mouse_move_impl(int, int); struct glwindow_mouse_move_wrapper { glwindow_mouse_move_wrapper() { \
        glwindow_get()->mouse_move([](int glw_mm_x, int glw_mm_y) { glwindow_mouse_move_impl(glw_mm_x, glw_mm_y); }); \
} }; glwindow_mouse_move_wrapper glwindow_mouse_move_wrapper_; void glwindow_mouse_move_impl(int x, int y)

#define GLWINDOW_MOUSE_DOWN(x, y, button) void glwindow_mouse_down_impl(int, int, glwindow::mouse_button); struct glwindow_mouse_down_wrapper { glwindow_mouse_down_wrapper() { \
        glwindow_get()->mouse_down([](int glw_md_x, int glw_md_y, glwindow::mouse_button glw_md_b) { glwindow_mouse_down_impl(glw_md_x, glw_md_x, glw_md_b); }); \
} }; glwindow_mouse_down_wrapper glwindow_mouse_down_wrapper_; void glwindow_mouse_down_impl(int x, int y, glwindow::mouse_button button)

#define GLWINDOW_MOUSE_UP(x, y, button) void glwindow_mouse_up_impl(int, int, glwindow::mouse_button); struct glwindow_mouse_up_wrapper { glwindow_mouse_up_wrapper() { \
        glwindow_get()->mouse_up([](int glw_mu_x, int glw_mu_y, glwindow::mouse_button glw_mu_b) { glwindow_mouse_up_impl(glw_mu_x, glw_mu_x, glw_mu_b); }); \
} }; glwindow_mouse_up_wrapper glwindow_mouse_up_wrapper_; void glwindow_mouse_up_impl(int x, int y, glwindow::mouse_button button)

#define GLWINDOW_MOUSE_CLICK(x, y, button) void glwindow_mouse_click_impl(int, int, glwindow::mouse_button); struct glwindow_mouse_click_wrapper { glwindow_mouse_click_wrapper() { \
        glwindow_get()->mouse_click([](int glw_mc_x, int glw_mc_y, glwindow::mouse_button glw_mc_b) { glwindow_mouse_click_impl(glw_mc_x, glw_mc_x, glw_mc_b); }); \
} }; glwindow_mouse_click_wrapper glwindow_mouse_click_wrapper_; void glwindow_mouse_click_impl(int x, int y, glwindow::mouse_button button)

#define GLWINDOW_MOUSE_SCROLL(x, y, value) void glwindow_mouse_scroll_impl(int, int, int); struct glwindow_mouse_scroll_wrapper { glwindow_mouse_scroll_wrapper() { \
        glwindow_get()->mouse_scroll([](int glw_ms_x, int glw_ms_y, int glw_ms_v) { glwindow_mouse_scroll_impl(glw_ms_x, glw_ms_x, glw_ms_v); }); \
} }; glwindow_mouse_scroll_wrapper glwindow_mouse_scroll_wrapper_; void glwindow_mouse_scroll_impl(int x, int y, int value)

#define GLWINDOW_KEY_DOWN(x, y, key) void glwindow_key_down_impl(int, int, int); struct glwindow_key_down_wrapper { glwindow_key_down_wrapper() { \
        glwindow_get()->key_down([](int glw_kd_x, int glw_kd_y, int glw_kd_k) { glwindow_key_down_impl(glw_kd_x, glw_kd_x, glw_kd_k); }); \
} }; glwindow_key_down_wrapper glwindow_key_down_wrapper_; void glwindow_key_down_impl(int x, int y, int key)

#define GLWINDOW_KEY_UP(x, y, key) void glwindow_key_up_impl(int, int, int); struct glwindow_key_up_wrapper { glwindow_key_up_wrapper() { \
        glwindow_get()->key_up([](int glw_ku_x, int glw_ku_y, int glw_ku_k) { glwindow_key_up_impl(glw_ku_x, glw_ku_x, glw_ku_k); }); \
} }; glwindow_key_up_wrapper glwindow_key_up_wrapper_; void glwindow_key_up_impl(int x, int y, int key)

class glwindow
{
public:
    glwindow() {}

    glwindow(const glwindow_str& title)
        : window_title_(title)
    {
    }

    glwindow(const glwindow_str& title, int width, int height)
        : window_title_(title), window_width_(width), window_height_(height)
    {
    }

    glwindow(const glwindow_str& title, int width, int height, bool fullscreen)
        : window_title_(title), window_width_(width), window_height_(height), fullscreen_(fullscreen)
    {
    }

    static const glwindow_str& title() { return glwindow_get()->window_title_; }
    static bool fullscreen() { return glwindow_get()->fullscreen_; }
    static int height() { return glwindow_get()->window_height_; }
    static int width() { return glwindow_get()->window_width_; }
    static int target_fps() { return glwindow_get()->target_fps_; }
    static int click_duration() { return glwindow_get()->click_duration_; }

    static void title(const glwindow_str& title) { glwindow_get()->window_title_ = title; }
    static void fullscreen(bool fs) { glwindow_get()->fullscreen_ = fs; }
    static void height(int height) { glwindow_get()->window_height_ = height; }
    static void width(int width) { glwindow_get()->window_width_ = width; }
    static void target_fps(int fps) { glwindow_get()->target_fps_ = fps; }
    static void click_duration(int duration) { glwindow_get()->click_duration_ = duration; }

    enum mouse_button
    {
        none = 0,
        left,
        middle,
        right
    };

    typedef std::function<void()> idle_fn;
    void idle(idle_fn fn) { idle_fn_ = fn; }
    virtual void idle() { idle_fn_ ? idle_fn_() : (void)0; }

    typedef std::function<void()> draw_fn;
    void draw(draw_fn fn) { draw_fn_ = fn; }
    virtual void draw() { draw_fn_ ? draw_fn_() : (void)0; }

    typedef std::function<void()> kill_fn;
    void kill(kill_fn fn) { kill_fn_ = fn; }
    virtual void kill() { kill_fn_ ? kill_fn_() : (void)0; }

    typedef std::function<void(int, int)> resize_fn;
    void resize(resize_fn fn) { resize_fn_ = fn; }
    virtual void resize(int width, int height) { resize_fn_ ? resize_fn_(width, height) : (void)0; }

    typedef std::function<void(int, int)> mouse_move_fn;
    void mouse_move(resize_fn fn) { mouse_move_fn_ = fn; }
    virtual void mouse_move(int x, int y) { mouse_move_fn_ ? mouse_move_fn_(x, y) : (void)0; }

    typedef std::function<void(int, int, mouse_button)> mouse_down_fn;
    void mouse_down(mouse_down_fn fn) { mouse_down_fn_ = fn; }
    virtual void mouse_down(int x, int y, mouse_button button) { mouse_down_fn_ ? mouse_down_fn_(x, y, button) : (void)0; }

    typedef std::function<void(int, int, mouse_button)> mouse_up_fn;
    void mouse_up(mouse_up_fn fn) { mouse_up_fn_ = fn; }
    virtual void mouse_up(int x, int y, mouse_button button) { mouse_up_fn_ ? mouse_up_fn_(x, y, button) : (void)0; }

    typedef std::function<void(int, int, mouse_button)> mouse_click_fn;
    void mouse_click(mouse_click_fn fn) { mouse_click_fn_ = fn; }
    virtual void mouse_click(int x, int y, mouse_button button) { mouse_click_fn_ ? mouse_click_fn_(x, y, button) : (void)0; }

    typedef std::function<void(int, int, int)> mouse_scroll_fn;
    void mouse_scroll(mouse_scroll_fn fn) { mouse_scroll_fn_ = fn; }
    virtual void mouse_scroll(int x, int y, int value) { mouse_scroll_fn_ ? mouse_scroll_fn_(x, y, value) : (void)0; }

    typedef std::function<void(int, int, unsigned int)> key_down_fn;
    void key_down(key_down_fn fn) { key_down_fn_ = fn; }
    virtual void key_down(int x, int y, unsigned int key) { key_down_fn_ ? key_down_fn_(x, y, key) : (void)0; }

    typedef std::function<void(int, int, unsigned int)> key_up_fn;
    void key_up(key_up_fn fn) { key_up_fn_ = fn; }
    virtual void key_up(int x, int y, unsigned int key) { key_up_fn_ ? key_up_fn_(x, y, key) : (void)0; }

    typedef std::function<void(std::vector<std::wstring>)> drag_drop_fn;
    void drag_drop(drag_drop_fn fn) { drag_drop_fn_ = fn; }
    virtual void drag_drop(std::vector<std::wstring> files) { drag_drop_fn_ ? drag_drop_fn_(files) : (void)0; }

    class glcontext {};
    void context(std::shared_ptr<glcontext> ctxt)
    {
        context_ = ctxt;
    }
    glcontext* context() { return context_.get(); }

    // mouse click handler...
    void click_begin(mouse_button button)
    {
        timeStamp_ = std::chrono::steady_clock::now();
        mouse_button_ = button;
    }
    bool is_click(mouse_button button) const { return (button == mouse_button_ && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timeStamp_).count() < glwindow_get()->click_duration_); }

protected:

    const glwindow_str& window_title() const { return window_title_; }
    bool window_fullscreen() const { return fullscreen_; }
    int window_height() const { return window_height_; }
    int window_width() const { return window_width_; }
    int window_target_fps() const { return target_fps_; }
    int window_click_duration() const { return click_duration_; }

    void window_title(const glwindow_str& title) { window_title_ = title; }
    void window_fullscreen(bool fs) { fullscreen_ = fs; }
    void window_height(int height) { window_height_ = height; }
    void window_width(int width) { window_width_ = width; }
    void window_target_fps(int fps) { target_fps_ = fps; }
    void window_click_duration(int duration) { click_duration_ = duration; }

private:
    idle_fn idle_fn_ = nullptr;
    draw_fn draw_fn_ = nullptr;
    kill_fn kill_fn_ = nullptr;
    resize_fn resize_fn_ = nullptr;
    mouse_move_fn mouse_move_fn_ = nullptr;
    mouse_up_fn mouse_up_fn_ = nullptr;
    mouse_click_fn mouse_click_fn_ = nullptr;
    mouse_down_fn mouse_down_fn_ = nullptr;
    mouse_scroll_fn mouse_scroll_fn_ = nullptr;
    key_up_fn key_up_fn_ = nullptr;
    key_down_fn key_down_fn_ = nullptr;
    drag_drop_fn drag_drop_fn_ = nullptr;

    glwindow_str window_title_ = L"glwindow";
    int window_width_ = 800;
    int window_height_ = 600;
    bool fullscreen_ = false;
    int target_fps_ = 60;
    int click_duration_ = 200;

    // mouse click handler...
    std::chrono::steady_clock::time_point timeStamp_ = std::chrono::steady_clock::now();
    mouse_button mouse_button_ = mouse_button::none;

    std::shared_ptr<glcontext> context_;
};


// Windows platform implementation...
#ifdef GLO_WIN

static HWND hWnd_;
static HGLRC hGLrc_;
static HINSTANCE hInst_;
static WNDCLASSEX wcex_;
constexpr unsigned int IDT_FRAME_TIMER = 1;

// The contexts...
class glcontext_base : public glwindow::glcontext
{
public:
    glcontext_base(void* hdc)
    {
        if (!hdc)
            throw std::runtime_error("No device context.");

        HDC hDC = static_cast<HDC>(hdc);
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
        pfd.cDepthBits = 32;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int nPixelFormat = ChoosePixelFormat(hDC, &pfd);

        if (nPixelFormat == 0)
            throw std::runtime_error("Unable to choose pixel format.");

        if (!SetPixelFormat(hDC, nPixelFormat, &pfd))
            throw std::runtime_error("Unable to set pixel format.");

        hGLrc_ = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hGLrc_);
    }

    void free(void* hdc) { wglDeleteContext(hGLrc_); }
    void make_current(void* hdc) { wglMakeCurrent(static_cast<HDC>(hdc), hGLrc_); }
};

class glcontext_extended : public glcontext_base
{
public:
    glcontext_extended(void* hdc, int major_version, int minor_version) : glcontext_base(hdc)
    {
        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, major_version,
            WGL_CONTEXT_MINOR_VERSION_ARB, minor_version,
            WGL_CONTEXT_FLAGS_ARB, 0,
            0
        };

        PFNWGLCREATECONTEXTATTRIBSARBPROC glwindow_context_extended_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        if (glwindow_context_extended_wglCreateContextAttribsARB)
        {
            HDC hDC = static_cast<HDC>(hdc);
            if (!hDC)
                throw std::runtime_error("Unable to get handle device context.");

            auto tmpContext = hGLrc_;
            hGLrc_ = glwindow_context_extended_wglCreateContextAttribsARB(hDC, 0, attribs);
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(tmpContext);
            if (!wglMakeCurrent(hDC, hGLrc_))
                throw std::runtime_error("Unable to make current.");
        }
    }
};

// windows main callback...
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        KillTimer(hWnd, IDT_FRAME_TIMER);
        glwindow_get()->kill();
        PostQuitMessage(0);
        break;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        glwindow_get()->draw();
        SwapBuffers(GetDC(hWnd));
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SIZE:
    {
        glwindow_get()->resize(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
        glwindow_get()->height(static_cast<int>(HIWORD(lParam)));
        glwindow_get()->width(static_cast<int>(LOWORD(lParam)));
        break;
    }
    case WM_MOUSEWHEEL:
    {
        POINT pt; pt.x = LOWORD(lParam); pt.y = HIWORD(lParam); ScreenToClient(hWnd_, &pt);
        glwindow_get()->mouse_scroll(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    }
    case WM_MOUSEMOVE:
    {
        glwindow_get()->mouse_move(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_LBUTTONDOWN:
    {
        glwindow_get()->click_begin(glwindow::mouse_button::left);
        glwindow_get()->mouse_down(LOWORD(lParam), glwindow_get()->height() - HIWORD(lParam), glwindow::mouse_button::left);
        SetCapture(hWnd);
        break;
    }
    case WM_LBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (glwindow_get()->is_click(glwindow::mouse_button::left))
            glwindow_get()->mouse_click(x, glwindow_get()->height() - y, glwindow::mouse_button::left);
        glwindow_get()->mouse_up(x, glwindow_get()->height() - y, glwindow::mouse_button::left);
        ReleaseCapture();
        break;
    }
    case WM_MBUTTONDOWN:
    {
        glwindow_get()->click_begin(glwindow::mouse_button::middle);
        glwindow_get()->mouse_down(LOWORD(lParam), glwindow_get()->height() - HIWORD(lParam), glwindow::mouse_button::middle);
        SetCapture(hWnd);
        break;
    }
    case WM_MBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (glwindow_get()->is_click(glwindow::mouse_button::middle))
            glwindow_get()->mouse_click(x, glwindow_get()->height() - y, glwindow::mouse_button::middle);
        glwindow_get()->mouse_up(x, glwindow_get()->height() - y, glwindow::mouse_button::middle);
        ReleaseCapture();
        break;
    }
    case WM_RBUTTONDOWN:
    {
        glwindow_get()->click_begin(glwindow::mouse_button::right);
        glwindow_get()->mouse_down(LOWORD(lParam), glwindow_get()->height() - HIWORD(lParam), glwindow::mouse_button::right);
        SetCapture(hWnd);
        break;
    }
    case WM_RBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (glwindow_get()->is_click(glwindow::mouse_button::right))
            glwindow_get()->mouse_click(x, glwindow_get()->height() - y, glwindow::mouse_button::right);
        glwindow_get()->mouse_up(x, glwindow_get()->height() - y, glwindow::mouse_button::right);
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
        glwindow_get()->key_down(pt.x, glwindow_get()->height() - pt.y, static_cast<unsigned int>(wParam));
        break;
    }
    case WM_KEYUP:
    {
        POINT pt; GetCursorPos(&pt); ScreenToClient(hWnd, &pt);
        glwindow_get()->key_up(pt.x, glwindow_get()->height() - pt.y, static_cast<unsigned int>(wParam));
        break;
    }
    case WM_TIMER:
    {
        if (wParam == IDT_FRAME_TIMER)
        {
            InvalidateRect(hWnd_, NULL, NULL);
            UpdateWindow(hWnd_);
        }
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
        glwindow_get()->drag_drop(files);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

static std::wstring ansi_to_wide(const std::string& str)
{
    int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], count);
    return wstr;
}

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

static void init_win(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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

    // create the gl context...
    auto context = glwindow_version_major() > 1 ?
        std::make_shared<glcontext_base>(GetDC(hWnd_)) :
        std::make_shared<glcontext_extended>(GetDC(hWnd_), glwindow_version_major(), glwindow_version_minor());

    // trigger the instance to construct it (if GLWINDOW_CLASS)...
    glwindow_get()->context(context);

    // call the custom init (defined after GLWINDOW macro)...
    glwindow_init_impl();

    // set the callback frame timer...
    if (glwindow_get()->target_fps())
        SetTimer(hWnd_, IDT_FRAME_TIMER, (1000 / glwindow_get()->target_fps()), NULL);

    // Resize the window to desired...
    RECT rect;
    GetWindowRect(hWnd_, &rect);
    SetWindowPos(hWnd_, HWND_TOP, rect.left, rect.top, glwindow_get()->width(), glwindow_get()->height(), NULL);

    // Set the name of the window...
    SetWindowText(hWnd_, glwindow_get()->title().c_str());
}

#define GLWINDOW_IMPL int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \
{\
    try\
    {\
        init_win(hInstance, hPrevInstance, lpCmdLine, nCmdShow);\
        MSG msg;\
        ShowWindow(hWnd_, nCmdShow);\
        UpdateWindow(hWnd_);\
        while (GetMessage(&msg, NULL, 0, 0))\
        {\
            glwindow_get()->idle();\
            TranslateMessage(&msg);\
            DispatchMessage(&msg);\
        }\
        return (int)msg.wParam;\
    }\
    catch (const std::exception& e) { MessageBox(NULL, ansi_to_wide(e.what()).c_str(), L"GLwindow", MB_ICONERROR); }\
    catch (...) { MessageBox(NULL, L"Unknown error", L"GLwindow", MB_ICONERROR); }\
    return 0;\
}

#endif // GLWINDOW_WIN





#endif // GLWINDOW_HPP

