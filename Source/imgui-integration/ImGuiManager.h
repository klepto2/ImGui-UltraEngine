#pragma once
#include "UltraEngine.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

using namespace UltraEngine;

#define INDEX_FLOAT_RANGE 500

namespace UltraEngine::ImGuiIntegration
{
	inline map<KeyCode, ImGuiKey> keyConversion = {
		{KeyCode::KEY_TAB, ImGuiKey_Tab},
		{KeyCode::KEY_LEFT, ImGuiKey_LeftArrow},
		{KeyCode::KEY_RIGHT, ImGuiKey_RightArrow},
		{KeyCode::KEY_UP, ImGuiKey_UpArrow},
		{KeyCode::KEY_DOWN, ImGuiKey_DownArrow},
		{KeyCode::KEY_PAGEUP, ImGuiKey_PageUp},
		{KeyCode::KEY_PAGEDOWN, ImGuiKey_PageDown},
		{KeyCode::KEY_HOME, ImGuiKey_Home},
		{KeyCode::KEY_END, ImGuiKey_End},
		{KeyCode::KEY_INSERT, ImGuiKey_Insert},
		{KeyCode::KEY_DELETE, ImGuiKey_Delete},
		{KeyCode::KEY_BACKSPACE, ImGuiKey_Backspace},
		{KeyCode::KEY_SPACE, ImGuiKey_Space},
		{KeyCode::KEY_ENTER, ImGuiKey_Enter},
		{KeyCode::KEY_ESCAPE, ImGuiKey_Escape},
		{KeyCode::KEY_COMMA, ImGuiKey_Comma},
		{KeyCode::KEY_PERIOD, ImGuiKey_Period},
		{KeyCode::KEY_SLASH, ImGuiKey_Slash},
		{KeyCode::KEY_SEMICOLON, ImGuiKey_Semicolon},
		{KeyCode::KEY_EQUALS, ImGuiKey_Equal},
		{KeyCode::KEY_OPENBRACKET, ImGuiKey_LeftBracket},
		{KeyCode::KEY_BACKSLASH, ImGuiKey_Backslash},
		{KeyCode::KEY_CLOSEBRACKET, ImGuiKey_RightBracket},
		{KeyCode::KEY_CAPSLOCK, ImGuiKey_CapsLock},
		{KeyCode::KEY_NUMLOCK, ImGuiKey_NumLock},
		{KeyCode::KEY_NUMPAD0, ImGuiKey_Keypad0},
		{KeyCode::KEY_NUMPAD1, ImGuiKey_Keypad1},
		{KeyCode::KEY_NUMPAD2, ImGuiKey_Keypad2},
		{KeyCode::KEY_NUMPAD3, ImGuiKey_Keypad3},
		{KeyCode::KEY_NUMPAD4, ImGuiKey_Keypad4},
		{KeyCode::KEY_NUMPAD5, ImGuiKey_Keypad5},
		{KeyCode::KEY_NUMPAD6, ImGuiKey_Keypad6},
		{KeyCode::KEY_NUMPAD7, ImGuiKey_Keypad7},
		{KeyCode::KEY_NUMPAD8, ImGuiKey_Keypad8},
		{KeyCode::KEY_NUMPAD9, ImGuiKey_Keypad9},
		{KeyCode::KEY_NUMPADDIVIDE, ImGuiKey_KeypadDivide},
		{KeyCode::KEY_NUMPADMULTIPLY, ImGuiKey_KeypadMultiply},
		{KeyCode::KEY_NUMPADSUBTRACT, ImGuiKey_KeypadSubtract},
		{KeyCode::KEY_NUMPADADDITION, ImGuiKey_KeypadAdd},
		{KeyCode::KEY_SHIFT, ImGuiKey_LeftShift},
		{KeyCode::KEY_CONTROL, ImGuiKey_LeftCtrl},
		{KeyCode::KEY_ALT, ImGuiKey_LeftAlt},
		{KeyCode::KEY_COMMAND, ImGuiKey_LeftSuper},
		{KeyCode::KEY_SHIFT, ImGuiKey_RightShift},
		{KeyCode::KEY_CONTROL, ImGuiKey_RightCtrl},
		{KeyCode::KEY_ALT, ImGuiKey_RightAlt},
		{KeyCode::KEY_COMMAND, ImGuiKey_RightSuper},
		{KeyCode::KEY_D0, ImGuiKey_0},
		{KeyCode::KEY_D1, ImGuiKey_1},
		{KeyCode::KEY_D2, ImGuiKey_2},
		{KeyCode::KEY_D3, ImGuiKey_3},
		{KeyCode::KEY_D4, ImGuiKey_4},
		{KeyCode::KEY_D5, ImGuiKey_5},
		{KeyCode::KEY_D6, ImGuiKey_6},
		{KeyCode::KEY_D7, ImGuiKey_7},
		{KeyCode::KEY_D8, ImGuiKey_8},
		{KeyCode::KEY_D9, ImGuiKey_9},
		{KeyCode::KEY_A, ImGuiKey_A},
		{KeyCode::KEY_B, ImGuiKey_B},
		{KeyCode::KEY_C, ImGuiKey_C},
		{KeyCode::KEY_D, ImGuiKey_D},
		{KeyCode::KEY_E, ImGuiKey_E},
		{KeyCode::KEY_F, ImGuiKey_F},
		{KeyCode::KEY_G, ImGuiKey_G},
		{KeyCode::KEY_H, ImGuiKey_H},
		{KeyCode::KEY_I, ImGuiKey_I},
		{KeyCode::KEY_J, ImGuiKey_J},
		{KeyCode::KEY_K, ImGuiKey_K},
		{KeyCode::KEY_L, ImGuiKey_L},
		{KeyCode::KEY_M, ImGuiKey_M},
		{KeyCode::KEY_N, ImGuiKey_N},
		{KeyCode::KEY_O, ImGuiKey_O},
		{KeyCode::KEY_P, ImGuiKey_P},
		{KeyCode::KEY_Q, ImGuiKey_Q},
		{KeyCode::KEY_R, ImGuiKey_R},
		{KeyCode::KEY_S, ImGuiKey_S},
		{KeyCode::KEY_T, ImGuiKey_T},
		{KeyCode::KEY_U, ImGuiKey_U},
		{KeyCode::KEY_V, ImGuiKey_V},
		{KeyCode::KEY_W, ImGuiKey_W},
		{KeyCode::KEY_X, ImGuiKey_X},
		{KeyCode::KEY_Y, ImGuiKey_Y},
		{KeyCode::KEY_Z, ImGuiKey_Z},
		{KeyCode::KEY_F1, ImGuiKey_F1},
		{KeyCode::KEY_F2, ImGuiKey_F2},
		{KeyCode::KEY_F3, ImGuiKey_F3},
		{KeyCode::KEY_F4, ImGuiKey_F4},
		{KeyCode::KEY_F5, ImGuiKey_F5},
		{KeyCode::KEY_F6, ImGuiKey_F6},
		{KeyCode::KEY_F7, ImGuiKey_F7},
		{KeyCode::KEY_F8, ImGuiKey_F8},
		{KeyCode::KEY_F9, ImGuiKey_F9},
		{KeyCode::KEY_F10, ImGuiKey_F10},
		{KeyCode::KEY_F11, ImGuiKey_F11},
		{KeyCode::KEY_F12, ImGuiKey_F12}
	};

	
	class ImGuiSurface;

	class ImGuiManager : public Object
	{
		friend class ImGuiSurface;

		shared_ptr<Window> _window;
		shared_ptr<World> _world;
		shared_ptr<Framebuffer> _renderTarget;
		shared_ptr<Camera> _camera;
		vector<shared_ptr<Texture>> _textures;
		vector<shared_ptr<Model>> _models;
		map<String, shared_ptr<ImGuiSurface>> _surfaces;

		bool _initialized = false;
		int _renderLayer;
		INT64  _time;
		INT64  _ticksPerSecond;

		int PushTexture(shared_ptr<Texture> texture);
		void ReplaceTexture(shared_ptr<Texture> texture, int index);
		void Setup();
		void InitializeImGuiAndPlatForm();
		void InitializeBackend();
		void CreateFontTexture();
		void UpdateMouseCursor();
		shared_ptr<Material> CreateImGuiMaterial();
		void BuildModels();

	public:
		ImGuiManager(shared_ptr<Window> window, shared_ptr<World> world, int renderLayer = 2, shared_ptr<Framebuffer> renderTarget = NULL);
		ImTextureID RegisterTexture(shared_ptr<Texture> texture);
		void BeginFrame();
		void Sync();

		friend shared_ptr<ImGuiManager> CreateImGuiManager(shared_ptr<Window> window, shared_ptr<World> world, int renderLayer, shared_ptr<Framebuffer> renderTarget);
	};

	shared_ptr<ImGuiManager> CreateImGuiManager(shared_ptr<Window> window, shared_ptr<World> world, int renderLayer = 2, shared_ptr<Framebuffer> renderTarget = NULL);

	class ImGuiSurface : public Object
	{
		shared_ptr<ImGuiManager> _manager;
		String _name;
		shared_ptr<Model> _model;
		vector<shared_ptr<Mesh>> _meshes;
		bool _updated = false;
		int _index = 0;

		std::tuple<vector<Vertex>, vector<uint32_t>> PrepareBuffers(const ImDrawList* drawList);
	public:
		ImGuiSurface(String name, shared_ptr<ImGuiManager> manager);
		void Update(const ImDrawList* drawList, ImVec2 clip_off, int index, int max_index);
		void Hide();
	};

	shared_ptr<ImGuiSurface> CreateImGuiSurface(String name, shared_ptr<ImGuiManager> manager);
};