# Dear ImGui - UltraEngine Backend

A backend for using [Dear ImGui](https://github.com/ocornut/imgui) in [UltraEngine
](https://www.ultraengine.com/)

(Update 03/20/2024)

	- Changed Shaderfamily and shaders to support UltraEngine 0.9.5

Basic integration is done using the Docking branch of "Dear ImGui".
Supports nearly everything, only the new Viewport-System will be implemented at a later time.

Usage:

 - Clone the Repository
 - Download the Sub-Module or manually download the latest ImGui version you want to use
 - Add the ImGui files to your solution
 - Add ImGuiManager.h and ImGuiManager.cpp to the solution
 - Select all ImGui cpp files in VisualStudio and open the properties via rightclick
 - Disable the precompiled header usage for these files

Basic Setup:

    #include "UltraEngine.h"
    #include "Components/Motion/Mover.hpp"
    #include "imgui-integration/ImGuiManager.h"
    
    using namespace UltraEngine;
    using namespace UltraEngine::ImGuiIntegration;
    using namespace std;
 
	void main()
	{
		// Setup your app (window/world etc.)
		auto imGuiManager = CreateImGuiManager(window, world, 2, framebuffer);
		
		//mainloop
		while (window->Closed() == false and window->KeyDown(KEY_ESCAPE) == false)
	  {
	        //Begin the ui-definition
	        imGuiManager->BeginFrame();
	        // Define your Gui
	        ...
	        //Stop UI-Recording and update/create the models for UltraEngine
	        imGuiManager->Sync();
			
			//Update the world as normal
			world->Update();
			world->Render(framewbuffer);
		}
		return 0;
	}
