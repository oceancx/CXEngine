#include "window.h"

#include "file_system.h"
#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "time/time.h"
#include <script_system.h>
#include "file_loading.h"
#include "logger.h"
#include "cxlua.h"
#include <render\render.h>

#define GAME_SCREEN_WIDTH 800
#define GAME_SCREEN_HEIGHT 600
static const float MS_PER_UPDATE = 1000 / 60.f / 1000;
int m_Width = 0;
int m_Height = 0;
int m_WindowWidth;
int m_WindowHeight;
float g_DeltaTime = 0;
float g_LastTime = 0;
GLFWwindow* m_pWindow = nullptr;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



GLFWwindow* Window::GetGLFWwindow() { return m_pWindow; };
int Window::GetWidth() { return m_Width; };
int Window::GetHeight() { return m_Height; };
int Window::GetWindowWidth() { return m_WindowWidth; };
int Window::GetWindowHeight() { return m_WindowHeight; };
float Window::GetCenterX() { return GetWidth() / 2.f; }
float Window::GetCenterY() { return GetHeight() / 2.f; }
float Window::GetFPS() { return g_DeltaTime; }

static void glfw_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	m_WindowWidth = width;
	m_WindowHeight = height;
	m_Width = width;
	m_Height = height;

	cxluax3d::render_on_frame_buffer_size_changed(width, height);
}

static void glfw_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mode);
}
static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

static void glfw_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
}

static void glfw_character_callback(GLFWwindow* window, unsigned int charcode)
{
	ImGui_ImplGlfw_CharCallback(window, charcode);
}

static bool s_WindowFocused = false;

static std::vector<CXString>  s_DropFiles;
static bool s_DropTriggered = false;
static void glfw_drog_function(GLFWwindow* window, int path_count, const char* paths[])
{
	s_DropTriggered = true;
	s_DropFiles.clear();
	//printf("%d \n", path_count);
	for (int i = 0; i < path_count; i++) {
		//printf("%s \n", paths[i]);
		s_DropFiles.push_back(paths[i]);
	}
	if (!s_WindowFocused) {
		glfwFocusWindow(window);
	}
}

static void glfw_focus_function(GLFWwindow* window, int focused) {
	s_WindowFocused = focused != 0;
}


static void glfw_error_callback(int error, const char* description) {
	cxlog_err("Error: %d %s\n", error, description);
}

Window::Window() {}

Window::~Window() {}

void Window::Init(int w, int h) {}

void Window::Destroy() {}

void Window::Show() {}

float Window::GetDeltaTime() { return g_DeltaTime; }


float Window::GetDeltaTimeMilliseconds() { return g_DeltaTime * 1000.f; }

void Window::OnFrameBufferSizeCallback(int width, int height) {}

void iw_set_font(const char* path, int size) {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(path, size, NULL, io.Fonts->GetGlyphRangesChineseFull());
}

const char* sprite_fs = R"(#version 100
precision lowp float;
varying vec2 v_texcoord;
varying vec4 v_color;
varying vec4 v_additive;
uniform sampler2D texture0;

void main() {
	gl_FragColor = v_color;
	/*vec4 tmp = texture2D(texture0, v_texcoord);
	gl_FragColor.xyz = tmp.xyz * v_color.xyz;
	gl_FragColor.w = tmp.w;
	gl_FragColor *= v_color.w;
	gl_FragColor.xyz += v_additive.xyz * tmp.w;*/
})";

const char* sprite_vs = R"(#version 100
precision lowp float;
attribute vec4 position;
attribute vec2 texcoord;
attribute vec4 color;
attribute vec4 additive;

varying vec2 v_texcoord;
varying vec4 v_color;
varying vec4 v_additive;

void main() {
	gl_Position = position; //+ vec4(-1.0, 1.0, 0, 0);
	v_texcoord = texcoord;
	v_color = color;
	v_additive = additive;
})";




struct material;
void iw_init(int w, int h)
{

	if (!glfwInit()) {
		cxlog_err("glfwInit error!");
		exit(EXIT_FAILURE);
	}
	glfwSetErrorCallback(glfw_error_callback);

#if __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	std::string floatConfig = command_arg_opt_str("window_float", "0");;
	glfwWindowHint(GLFW_FLOATING, floatConfig == "1");

	m_pWindow = glfwCreateWindow(w, h, "cxlua3d", nullptr, nullptr);
	if (m_pWindow == nullptr)
	{
		cxlog_err("glfwCreateWindow failed!");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwGetWindowSize(m_pWindow, &m_WindowWidth, &m_WindowHeight);
	m_Width = m_WindowWidth;
	m_Height = m_WindowHeight;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int monitorW = 0, monitorH = 0;
	glfwGetMonitorWorkarea(monitor, nullptr, nullptr, &monitorW, &monitorH);
	glfwSetWindowPos(m_pWindow, (monitorW - m_WindowWidth) / 2, (monitorH - m_WindowHeight) / 2);

	glfwMakeContextCurrent(m_pWindow);
	glfwSwapInterval(1);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		cxlog_err("glewInit error! %s\n", glewGetErrorString(err));
		return;
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(m_pWindow, false);
	ImGui_ImplOpenGL3_Init(glsl_version);


	glfwSetFramebufferSizeCallback(m_pWindow, glfw_framebuffer_size_callback);
	glfwSetCursorPosCallback(m_pWindow, glfw_mouse_callback);
	glfwSetMouseButtonCallback(m_pWindow, glfw_button_callback);
	glfwSetKeyCallback(m_pWindow, glfw_key_callback);
	glfwSetScrollCallback(m_pWindow, glfw_scroll_callback);
	glfwSetCharCallback(m_pWindow, glfw_character_callback);
	glfwSetDropCallback(m_pWindow, glfw_drog_function);
	glfwSetWindowFocusCallback(m_pWindow, glfw_focus_function);

	glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetWindowOpacity(m_pWindow, 1.0f);

	cxluax3d::render_init();
}

void window_system_set_floating(int opt, int value)
{
	WINDOW_INSTANCE;
	glfwWindowHint(opt, value);
}

float delta_time()
{
	return WINDOW_INSTANCE->GetDeltaTimeMilliseconds();
}

float window_system_get_fps()
{
	return WINDOW_INSTANCE->GetFPS();
}

int game_get_width() {
	return WINDOW_INSTANCE->GetWidth();
}
int game_get_height() {
	return WINDOW_INSTANCE->GetHeight();
}

bool iw_should_close()
{
	return glfwWindowShouldClose(WINDOW_INSTANCE->GetGLFWwindow());
}

void iw_deinit() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_pWindow);
	glfwTerminate();

	auto* ne_thread = file_loading_thread();
	delete ne_thread;
	ne_thread = nullptr;
}


int iw_begin_render(lua_State* L)
{
	auto now = glfwGetTime();
	if (g_LastTime == 0) {
		g_LastTime = now;
	}
	g_DeltaTime = (float)(now - g_LastTime);
	g_LastTime = now;

	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking);
	ImGui::PopStyleVar();

	cxluax3d::render_begin(m_Width, m_Height);
	return 0;
}

int iw_end_render(lua_State* L)
{
	cxluax3d::render_end();

	ImGui::End();
	ImGui::Render();


	int display_w, display_h;
	glfwGetFramebufferSize(m_pWindow, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	glfwSwapBuffers(m_pWindow);
	return 0;
}

int iw_render(lua_State* L)
{
	lua_pushvalue(L, 1);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	while (!glfwWindowShouldClose(m_pWindow))
	{
		iw_begin_render(L);

		cxluax3d::render_update();
		if (ref != -1) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			int res = lua_pcall(L, 0, 0, 0);
			check_lua_error(L, res);
		}
		iw_end_render(L);
	}

	luaL_unref(L, LUA_REGISTRYINDEX, ref);
	ref = -1;
	return 0;

}

bool iw_is_dropped() {
	return s_DropTriggered;
}

void iw_set_dropped(bool dropped) {
	s_DropTriggered = dropped;

}

int iw_get_drop_files(lua_State* L) {
	lua_newtable(L);
	for (int i = 0; i < s_DropFiles.size(); i++) {
		lua_pushlstring(L, s_DropFiles[i].c_str(), s_DropFiles[i].size());
		lua_seti(L, -2, i + 1);
	}
	return 1;
}



void luaopen_window(lua_State* L)
{
	script_system_register_function(L, window_system_set_floating);
	script_system_register_function(L, delta_time);
	script_system_register_function(L, window_system_get_fps);

	script_system_register_function(L, game_get_width);
	script_system_register_function(L, game_get_height);


	script_system_register_function(L, iw_deinit);
	script_system_register_function(L, iw_should_close);

	script_system_register_function(L, iw_set_font);

	script_system_register_function(L, iw_init);

	script_system_register_luac_function(L, iw_render);

	script_system_register_luac_function(L, iw_begin_render);
	script_system_register_luac_function(L, iw_end_render);



	script_system_register_function(L, iw_is_dropped);
	script_system_register_function(L, iw_set_dropped);
	script_system_register_luac_function(L, iw_get_drop_files);
}
