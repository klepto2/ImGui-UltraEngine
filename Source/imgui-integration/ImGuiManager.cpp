#include "UltraEngine.h"
#include "ImGuiManager.h"

using namespace UltraEngine;

namespace UltraEngine::ImGuiIntegration
{
	struct ImGui_ViewportData
	{
		shared_ptr<Window> window;
		shared_ptr<Framebuffer> framebuffer;
		shared_ptr<Camera> camera;
	};

	static void ImGui_CreateWindow(ImGuiViewport* viewport)
	{
		ImGui_ViewportData* vd = IM_NEW(ImGui_ViewportData)();
		viewport->PlatformUserData = vd;

		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui_ViewportData* main_viewport_data = (ImGui_ViewportData*)main_viewport->PlatformUserData;

		vd->window = CreateWindow("", viewport->Pos.x, viewport->Pos.y, viewport->Size.x, viewport->Size.y, main_viewport_data->window);
	}

	static void ImGui_DestroyWindow(ImGuiViewport* viewport)
	{
		if (ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData)
		{
			vd->camera = NULL;
			vd->framebuffer = NULL;
			vd->window->Close();
			vd->window = NULL;
		}

		viewport->PlatformUserData = viewport->PlatformHandle = nullptr;
	}

	static void ImGui_ShowWindow(ImGuiViewport* viewport)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
#if defined(_WIN32)
		HWND hwnd = (HWND)viewport->PlatformHandleRaw;

		// SDL hack: Hide icon from task bar
		// Note: SDL 3.0.0+ has a SDL_WINDOW_UTILITY flag which is supported under Windows but the way it create the window breaks our seamless transition.
		if (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon)
		{
			LONG ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);
			ex_style &= ~WS_EX_APPWINDOW;
			ex_style |= WS_EX_TOOLWINDOW;
			::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
		}

		// SDL hack: SDL always activate/focus windows :/
		if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
		{
			::ShowWindow(hwnd, SW_SHOWNA);
			return;
		}
#endif

		vd->window->SetHidden(false);
	}



	static ImVec2 ImGui_GetWindowPos(ImGuiViewport* viewport)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;

		return ImVec2((float)vd->window->GetSize().x, (float)vd->window->GetSize().y);
	}

	static void ImGui_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		auto size = vd->window->GetSize();
		vd->window->SetShape((int)pos.x, (int)pos.y, (int)size.x, (int)size.y);
	}

	static ImVec2 ImGui_GetWindowSize(ImGuiViewport* viewport)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		auto size = vd->window->GetSize();
		return ImVec2((float)size.x, (float)size.y);
	}

	static void ImGui_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		auto pos = vd->window->GetPosition();
		vd->window->SetShape((int)pos.x, (int)pos.y, (int)size.x, (int)size.y);
	}

	static void ImGui_SetWindowTitle(ImGuiViewport* viewport, const char* title)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		vd->window->SetText(title);
	}

	static void ImGui_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
	{
		// ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		 //SDL_SetWindowOpacity(vd->Window, alpha);
	}

	static void ImGui_SetWindowFocus(ImGuiViewport* viewport)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		vd->window->Activate();
	}

	static bool ImGui_GetWindowFocus(ImGuiViewport* viewport)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		return false;
	}

	static bool ImGui_GetWindowMinimized(ImGuiViewport* viewport)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
		return vd->window->Minimized();
	}

	static void ImGui_RenderWindow(ImGuiViewport* viewport, void*)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;

	}

	static void ImGui_SwapBuffers(ImGuiViewport* viewport, void*)
	{
		ImGui_ViewportData* vd = (ImGui_ViewportData*)viewport->PlatformUserData;
	}

	class ModifiableMesh : public Mesh
	{
	public:
		void ResizeVertexBuffer(shared_ptr<Mesh> base, int newSize)
		{			
			static_cast<ModifiableMesh*>(base.get())->Mesh::m_vertices.resize(newSize);
		};

		void ResizeIndexBuffer(shared_ptr<Mesh> base, int newSize)
		{
			
			static_cast<ModifiableMesh*>(base.get())->Mesh::m_indices.resize(newSize);
		};

		void Clear(shared_ptr<Mesh> base)
		{
			static_cast<ModifiableMesh*>(base.get())->Mesh::m_vertices.clear();
			static_cast<ModifiableMesh*>(base.get())->Mesh::m_indices.clear();
		};

		int VertexSize(shared_ptr<Mesh> base)
		{
			return static_cast<ModifiableMesh*>(base.get())->Mesh::m_vertices.size();
		};

		int IndexSize(shared_ptr<Mesh> base)
		{
			return static_cast<ModifiableMesh*>(base.get())->Mesh::m_indices.size();
		};
	};

	const shared_ptr<ModifiableMesh> ModMesh = std::make_shared<ModifiableMesh>();

	bool InputEventCallback(const Event& ev, shared_ptr<Object> extra)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse)
			return false;

		switch (ev.id)
		{
		case EVENT_KEYCHAR:
			io.AddInputCharacter(ev.data);
			break;
		case EVENT_KEYDOWN:
			io.AddKeyEvent(keyConversion[(KeyCode)ev.data], true);
			break;
		case EVENT_KEYUP:
			io.AddKeyEvent(keyConversion[(KeyCode)ev.data], false);
			break;
		case EVENT_MOUSEDOWN:
			switch (ev.data)
			{
			case MOUSE_LEFT:
				io.AddMouseButtonEvent(0, true);
				break;
			case MOUSE_RIGHT:
				io.AddMouseButtonEvent(1, true);
				break;
			case MOUSE_MIDDLE:
				io.AddMouseButtonEvent(2, true);
				break;
			case MOUSE_BACK:
				io.AddMouseButtonEvent(3, true);
				break;
			case MOUSE_FORWARD:
				io.AddMouseButtonEvent(4, true);
			}
			break;
		case EVENT_MOUSEUP:
			switch (ev.data)
			{
			case MOUSE_LEFT:
				io.AddMouseButtonEvent(0, false);
				break;
			case MOUSE_RIGHT:
				io.AddMouseButtonEvent(1, false);
				break;
			case MOUSE_MIDDLE:
				io.AddMouseButtonEvent(2, false);
				break;
			case MOUSE_BACK:
				io.AddMouseButtonEvent(3, false);
				break;
			case MOUSE_FORWARD:
				io.AddMouseButtonEvent(4, false);
			}
			break;
		case EVENT_MOUSEWHEEL:
			io.AddMouseWheelEvent(0.0f, -ev.data);
			break;
		}

		return false;
	}

	shared_ptr<ImGuiSurface> CreateImGuiSurface(String name, shared_ptr<ImGuiManager> manager)
	{
		return std::make_shared<ImGuiSurface>(name, manager);
	}

	shared_ptr<ImGuiManager> CreateImGuiManager(shared_ptr<Window> window, shared_ptr<World> world, int renderLayer, shared_ptr<Framebuffer> renderTarget)
	{
		return std::make_shared<ImGuiManager>(window, world, renderLayer, renderTarget);
	}

	int ImGuiManager::PushTexture(shared_ptr<Texture> texture)
	{
		_textures.push_back(texture);
		return _textures.size() - 1;
	}

	void ImGuiManager::ReplaceTexture(shared_ptr<Texture> texture, int index)
	{
		_textures[index] = texture;
	}

	void ImGuiManager::Setup()
	{
		InitializeImGuiAndPlatForm();
		InitializeBackend();

		// Create ui camera
		_camera = CreateCamera(_world, PROJECTION_ORTHOGRAPHIC);
		_camera->SetClearMode(CLEAR_DEPTH);
		_camera->SetDepthPrepass(false);
		_camera->SetRenderLayers(_renderLayer);
		_camera->SetPosition(float(_window->GetFramebuffer()->size.x) * 0.5f, float(_window->GetFramebuffer()->size.y) * 0.5f, 0);
		_camera->SetGrid(true);

		INT64 perf_frequency, perf_counter;
		
		::QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency);;
		::QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter);

		_time = perf_counter;
		_ticksPerSecond = perf_frequency;
	}

	void ImGuiManager::InitializeImGuiAndPlatForm()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		io.BackendPlatformName = "imgui_impl_ultraengine";
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		//io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;

	}

	void ImGuiManager::InitializeBackend()
	{
		ImGuiIO& io = ImGui::GetIO();
		IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

		// Setup backend capabilities flags
		ImGuiManager* bd = this;
		io.BackendRendererUserData = (void*)bd;
		io.BackendRendererName = "imgui_impl_ultraengine";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		//io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

		ListenEvent(EVENT_MOUSEDOWN, NULL, InputEventCallback);
		ListenEvent(EVENT_MOUSEUP, NULL, InputEventCallback);
		ListenEvent(EVENT_MOUSEWHEEL, NULL, InputEventCallback);
		ListenEvent(EVENT_KEYCHAR, NULL, InputEventCallback);
		ListenEvent(EVENT_KEYDOWN, NULL, InputEventCallback);
		ListenEvent(EVENT_KEYUP, NULL, InputEventCallback);

		ImGui::GetIO().IniFilename = NULL;

		// Register platform interface (will be coupled with a renderer interface)
		ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
		platform_io.Platform_CreateWindow = ImGui_CreateWindow;
		platform_io.Platform_DestroyWindow = ImGui_DestroyWindow;
		platform_io.Platform_ShowWindow = ImGui_ShowWindow;
		platform_io.Platform_SetWindowPos = ImGui_SetWindowPos;
		platform_io.Platform_GetWindowPos = ImGui_GetWindowPos;
		platform_io.Platform_SetWindowSize = ImGui_SetWindowSize;
		platform_io.Platform_GetWindowSize = ImGui_GetWindowSize;
		platform_io.Platform_SetWindowFocus = ImGui_SetWindowFocus;
		platform_io.Platform_GetWindowFocus = ImGui_GetWindowFocus;
		platform_io.Platform_GetWindowMinimized = ImGui_GetWindowMinimized;
		platform_io.Platform_SetWindowTitle = ImGui_SetWindowTitle;
		platform_io.Platform_RenderWindow = ImGui_RenderWindow;
		platform_io.Platform_SwapBuffers = ImGui_SwapBuffers;
		platform_io.Platform_SetWindowAlpha = ImGui_SetWindowAlpha;

		

		// Register main window handle (which is owned by the main application, not by us)
		// This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
		ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui_ViewportData* vd = IM_NEW(ImGui_ViewportData)();
		vd->window = _window;
		main_viewport->PlatformUserData = vd;
		main_viewport->PlatformHandle = vd->window->GetHandle();

	}

	void ImGuiManager::CreateFontTexture()
	{
		ImGuiIO& io = ImGui::GetIO();

		if (!io.Fonts->IsBuilt())
		{
			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
			auto pixmap = CreatePixmap(width, height, TextureFormat::TEXTURE_RGBA);
			pixmap->pixels->Poke(0, (const char*)pixels, pixmap->pixels->GetSize());
			auto fontTexture = CreateTexture(TEXTURE_2D, width, height, TextureFormat::TEXTURE_RGBA, {}, 1, TEXTURE_DEFAULT, TEXTUREFILTER_LINEAR);
			fontTexture->SetPixels(pixmap);
			if (io.Fonts->TexID == NULL)
			{
				io.Fonts->SetTexID((ImTextureID)PushTexture(fontTexture));
			}
			else
			{
				ReplaceTexture(fontTexture, (int)io.Fonts->TexID);
			}
		}
	}

	void ImGuiManager::UpdateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
			return;

		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			::SetCursor(nullptr);
		}
		else
		{
			// Show OS mouse cursor
			LPTSTR win32_cursor = IDC_ARROW;
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
			case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
			case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
			case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
			case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
			case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
			case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
			case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
			case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
			}
			::SetCursor(::LoadCursor(nullptr, win32_cursor));
		}
	}

	shared_ptr<Material> ImGuiManager::CreateImGuiMaterial()
	{
		auto material = CreateMaterial();
		material->SetShaderFamily(LoadShaderFamily("Shaders/ImGui.fam"));
		material->SetBackFaceCullMode(false);
		material->SetTransparent(true);
		material->SetDepthTest(false);
		return material;
	}

	std::tuple<vector<Vertex>, vector<uint32_t>> ImGuiSurface::PrepareBuffers(const ImDrawList* drawList)
	{
		vector<Vertex> vertices;
		vector<uint32_t> indices;

		for (auto vtx : drawList->VtxBuffer)
		{
			Vertex v = Vertex(Vec3(vtx.pos.x, ImGui::GetMainViewport()->Size.y - vtx.pos.y, 0), Vec3(0.0), Vec2(vtx.uv.x, vtx.uv.y));
			v.color = Vec4(Red(vtx.col), Green(vtx.col), Blue(vtx.col), Alpha(vtx.col)) / 255.0;
			vertices.push_back(v);
		}

		for (auto i : drawList->IdxBuffer)
		{
			indices.push_back(i);
		}

		return std::tuple<vector<Vertex>, vector<uint32_t>>(vertices, indices);
	}

	ImGuiSurface::ImGuiSurface(String name, shared_ptr<ImGuiManager> manager) : _name(name), _manager(manager), _index(0), _updated(false)
	{
	}

	void ImGuiSurface::Hide()
	{
		if (_model)
		{
			_model->SetHidden(true);
		}
	}

	void ImGuiSurface::Update(const ImDrawList* drawList, ImVec2 clip_off, int index, int max_index)
	{
		auto buffers = PrepareBuffers(drawList);
		vector<Vertex> vertices = std::get<vector<Vertex>>(buffers);
		vector<uint32_t> indices = std::get<vector<uint32_t>>(buffers);

		if (_model == NULL)
		{
			_model = CreateModel(_manager->_world);
			_model->SetRenderLayers(_manager->_renderLayer);
		}

		_model->SetHidden(false);
		_model->SetPosition(-clip_off.x, -clip_off.y, -((1.0 / max_index) * index));

		if (_model->lods[0]->meshes.size() != drawList->CmdBuffer.size())
		{
			_model->Clear();
		}

		for (int buff_idx = 0; buff_idx < drawList->CmdBuffer.size(); buff_idx++)
		{
			const ImDrawCmd* buf = &drawList->CmdBuffer[buff_idx];

			// Project scissor/clipping rectangles into framebuffer space
			ImVec2 clip_min(buf->ClipRect.x - clip_off.x, buf->ClipRect.y - clip_off.y);
			ImVec2 clip_max(buf->ClipRect.z - clip_off.x, buf->ClipRect.w - clip_off.y);
			if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
				continue;

			vector<uint32_t>::const_iterator first = indices.begin() + buf->IdxOffset;
			vector<uint32_t>::const_iterator last = indices.begin() + buf->IdxOffset + buf->ElemCount;
			vector<uint32_t> newIndices(first, last);

			//int start_i = *min_element(newIndices.begin(), newIndices.end());
			int start = *min_element(newIndices.begin(), newIndices.end());
			int end = (*max_element(newIndices.begin(), newIndices.end()))+1;
			vector<Vertex>::const_iterator first_v = vertices.begin() + start;
			vector<Vertex>::const_iterator last_v = vertices.begin() + end;
			vector<Vertex> newVertices(first_v, last_v);

			for (int i = 0; i < newIndices.size(); i++)
			{
				newIndices[i] = newIndices[i] - start;
			}

			shared_ptr<Mesh> mesh = NULL;

			if (_model->lods[0]->meshes.size() < (buff_idx + 1))
			{
				mesh = CreateMesh(MESH_TRIANGLES);//, newVertices, newIndices);
				mesh->SetMaterial(_manager->CreateImGuiMaterial());
				_model->AddMesh(mesh);
			}
			else
			{
				mesh = _model->lods[0]->meshes[buff_idx];
			}

			int v_size = ModMesh->VertexSize(mesh);
			int i_size = ModMesh->IndexSize(mesh);

			int v_diff = v_size - newVertices.size() - INDEX_FLOAT_RANGE;
			int i_diff = i_size - newIndices.size() - INDEX_FLOAT_RANGE;

			mesh->Modify(vector<Vertex>(v_size));
			mesh->Modify(vector<uint32_t>(i_size));


			if (abs(v_diff) > INDEX_FLOAT_RANGE)
			{
#ifdef DEBUG
				Print("Resizing (Vertex-Buffer): " + String(v_diff) + " | " +
					String(v_size) + "-" + String(newVertices.size()));
#endif
				ModMesh->ResizeVertexBuffer(mesh, newVertices.size() + INDEX_FLOAT_RANGE);
			}

			if (abs(i_diff) > INDEX_FLOAT_RANGE)//newIndices.size() > mesh->m_indices.size())
			{
#ifdef DEBUG
				Print("Resizing (Index-Buffer): " + String(i_diff) + " | " +
					String(i_size) + "-" + String(newIndices.size()));
#endif
				ModMesh->ResizeIndexBuffer(mesh, newIndices.size() + INDEX_FLOAT_RANGE);
			}

			mesh->Modify(newVertices);
			mesh->Modify(newIndices);

			mesh->GetMaterial()->SetTexture(_manager->_textures[(int)buf->TextureId], 0);
			mesh->GetMaterial()->SetColor((LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y);
		}
	}

	void ImGuiManager::BuildModels()
	{
		auto data = ImGui::GetDrawData();
		int model_index = 0;
		ImVec2 clip_off = data->DisplayPos;

		for (auto s : _surfaces)
		{
			s.second->Hide();
		}

		for (auto d : data->CmdLists)
		{
			auto surface = _surfaces[(String)d->_OwnerName];
			if (surface == NULL)
			{
				surface = CreateImGuiSurface((String)d->_OwnerName, Self()->As<ImGuiManager>());
				_surfaces[(String)d->_OwnerName] = surface;
			}

			surface->Update(d, clip_off, model_index++, data->CmdListsCount);

		}
	}

	ImGuiManager::ImGuiManager(shared_ptr<Window> window, shared_ptr<World> world, int renderLayer, shared_ptr<Framebuffer> renderTarget)
		: _window(window), _world(world), _renderLayer(renderLayer), _renderTarget(renderTarget)
	{
		if (!_initialized)
		{
			Setup();
			_initialized = true;
		}
	}

	ImTextureID ImGuiManager::RegisterTexture(shared_ptr<Texture> texture)
	{
		return (ImTextureID)PushTexture(texture);
	}

	void ImGuiManager::BeginFrame()
	{
		CreateFontTexture();
		//ImGui_ImplWin32_NewFrame();
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2( _renderTarget->GetSize().x, _renderTarget->GetSize().y);

		io.AddMousePosEvent(_window->GetMousePosition().x,_window->GetMousePosition().y);

		io.AddKeyEvent(ImGuiMod_Ctrl, _window->KeyDown(KEY_CONTROL));
		io.AddKeyEvent(ImGuiMod_Shift, _window->KeyDown(KEY_SHIFT));
		io.AddKeyEvent(ImGuiMod_Alt, _window->KeyDown(KEY_ALT));
		io.AddKeyEvent(ImGuiMod_Super, _window->KeyDown(KEY_COMMAND));

		UpdateMouseCursor();

		ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
		platform_io.Monitors.resize(0);

		auto displays = GetDisplays();
		for (auto d : displays)
		{
			ImGuiPlatformMonitor monitor;
			monitor.MainPos = ImVec2(d->GetPosition().x, d->GetPosition().y);
			monitor.MainSize = ImVec2(d->GetSize().x, d->GetSize().y);
			monitor.WorkPos = ImVec2(d->GetPosition().x, d->GetPosition().y);
			monitor.WorkSize = ImVec2(d->GetSize().x, d->GetSize().y);
			monitor.DpiScale = d->GetScale();
			monitor.PlatformHandle = d.get();
			platform_io.Monitors.push_back(monitor);
		}

		INT64 current_time = 0;
		::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
		io.DeltaTime = (float)(current_time - _time) / _ticksPerSecond;
		_time = current_time;

		ImGui::NewFrame();
	}

	void ImGuiManager::Sync()
	{
		ImGui::EndFrame();
		ImGui::Render();

		BuildModels();
	}

	
}
