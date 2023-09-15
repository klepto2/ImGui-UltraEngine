#include "UltraEngine.h"
#include "Components/Motion/Mover.hpp"
#include "imgui-integration/ImGuiManager.h"

using namespace UltraEngine;
using namespace UltraEngine::ImGuiIntegration;
using namespace std;

#define MAX_FRAMES 50

class StatCounter : public Object
{
    int _dataCount = 0;
    int _currentFrame = 0;
public:
    vector<float> data;

    float getMax() { return *max_element(data.begin(), data.end()); }
    float getMin() { return *min_element(data.begin(), data.end()); }
    float getAvarge()
    {
        float average = 0.0f;
        int removeCounts = 0;
        for (int n = 0; n < _dataCount; n++)
        {
            auto v = data[n];
            if (v <= 0.0) {
                v = 0.0;
                removeCounts++;
            }
            average += v;
        }
        average /= max((float)(_dataCount - removeCounts), 1.0f);

        return average;
    }

    void SetData(float value)
    {
        data[_currentFrame] = value;
        _currentFrame = (_currentFrame + 1) % _dataCount;
    }

    int GetFrame() { return _currentFrame; }

    StatCounter(int count) : _dataCount(count)
    {
        data = vector<float>(_dataCount,0);
    }
};


class ImDebugRenderer : public Object
{
    shared_ptr<World> _world;
    shared_ptr<Camera> _camera;

    shared_ptr<StatCounter> _fpsCounter;
    shared_ptr<StatCounter> _cullCounter;
    shared_ptr<StatCounter> _renderCounter;

    shared_ptr<StatCounter> _particletimeCounter;
    shared_ptr<StatCounter> _physictimeCounter;

    shared_ptr<StatCounter> _animationtimeCounter;

    bool _wireframe;
    bool _debugCollision;
    float _tesselation;
    Vec2 _tesselationParams;
    Vec2 _cameraRange;
    Vec4 _clearColor;
    bool _isFirstTime = true;

public:
    bool _isOpen = true;

    ImDebugRenderer(shared_ptr<World> world, shared_ptr<Camera> camera = NULL) : _world(world), _camera(camera)
    {
        _world->RecordStats(true);
        _fpsCounter = make_shared<StatCounter>(50);
        _cullCounter = make_shared<StatCounter>(100);
        _renderCounter = make_shared<StatCounter>(100);
        _particletimeCounter = make_shared<StatCounter>(100);
        _physictimeCounter = make_shared<StatCounter>(100);
        _animationtimeCounter = make_shared<StatCounter>(100);
    }

    void CollectStatistics()
    {
        _fpsCounter->SetData(_world->renderstats.framerate);
        _cullCounter->SetData(_world->renderstats.cullingtime);
        _renderCounter->SetData(_world->renderstats.rendertime);

        _particletimeCounter->SetData(_world->physicsstats.particletime);
        _physictimeCounter->SetData(_world->physicsstats.physicstime);

        _animationtimeCounter->SetData(_world->animationstats.time);
    }

    void UpdateCameraValues(bool read)
    {
        if (_camera != NULL)
        {
            if (read)
            {
                _wireframe = _camera->GetWireframe();
                _debugCollision = _camera->GetDebugPhysicsMode();
                _tesselation = _camera->GetTessellation();
                _cameraRange = _camera->GetRange();
                _clearColor = _camera->GetClearColor();
            }
            else
            {
                _camera->SetWireframe(_wireframe);
                _camera->SetDebugPhysicsMode(_debugCollision);
                _camera->SetTessellation(_tesselation);
                _camera->SetRange(_cameraRange);
                _camera->SetClearColor(_clearColor);
            }
        }
    }

    void Render()
    {
        CollectStatistics();

        if (_isOpen)
        {
           // ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Appearing);
            ImGui::SetNextWindowBgAlpha(0.75);
           // ImGui::SetNextWindowSize(ImVec2(-1, -1), ImGuiCond_Appearing);

            ImGui::Begin("Statisitics - Maincamera-Settings", &_isOpen , _isFirstTime ? ImGuiWindowFlags_AlwaysAutoResize : NULL);
            _isFirstTime = false;

            ImGui::BeginTabBar("##SettingsTabbar");
            
            if (ImGui::BeginTabItem("Render"))
            {
                ImGui::PlotLines(("Framerate (" + String(_world->renderstats.framerate) + " fps )").c_str(), &_fpsCounter->data[0], _fpsCounter->data.size(), _fpsCounter->GetFrame(), ("average : " + String(_fpsCounter->getAvarge())).c_str(), max(_fpsCounter->getMin() - 100.0, 0.0), _fpsCounter->getMax() + 100.0, ImVec2(0, 80.0f));
                ImGui::PlotLines(("Culltime (" + String(_world->renderstats.cullingtime) + " ms)").c_str(), &_cullCounter->data[0], _cullCounter->data.size(), _cullCounter->GetFrame(), ("average : " + String(_cullCounter->getAvarge())).c_str(), max(_cullCounter->getMin() - 5.0, 0.0), _cullCounter->getMax() + 5.0, ImVec2(0, 80.0f));
                ImGui::PlotLines(("Rendertime (" + String(_world->renderstats.rendertime) + " ms)").c_str(), &_renderCounter->data[0], _renderCounter->data.size(), _renderCounter->GetFrame(), ("average : " + String(_renderCounter->getAvarge())).c_str(), max(_renderCounter->getMin() - 1.0, 0.0), _renderCounter->getMax() + 2.0, ImVec2(0, 80.0f));

                ImGui::Text("Cameras        : %d", _world->renderstats.cameras);
                ImGui::Text("Meshbatches    : %d", _world->renderstats.meshbatches);
                ImGui::Text("Pipelines      : %d", _world->renderstats.pipelines);
                ImGui::Text("Polygons       : %d", _world->renderstats.polygons);
                ImGui::Text("Vertices       : %d", _world->renderstats.vertices);
                ImGui::Text("Instances      : %d", _world->renderstats.instances);
                ImGui::Text("Shadows        : %d", _world->renderstats.shadows);
                ImGui::Text("Shadowpolygons : %d", _world->renderstats.shadowpolygons);
                ImGui::Text("VRam           : %d", _world->renderstats.vram);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Physics"))
            {
                ImGui::PlotLines(("Particle-Time (" + String(_world->physicsstats.particletime) + " ms)").c_str(), &_particletimeCounter->data[0], _particletimeCounter->data.size(), _particletimeCounter->GetFrame(), ("average : " + String(_particletimeCounter->getAvarge())).c_str(), max(_particletimeCounter->getMin() - 5.0, 0.0), _particletimeCounter->getMax() + 5.0, ImVec2(0, 80.0f));
                ImGui::PlotLines(("Physics-Time (" + String(_world->physicsstats.physicstime) + " ms)").c_str(), &_physictimeCounter->data[0], _physictimeCounter->data.size(), _physictimeCounter->GetFrame(), ("average : " + String(_physictimeCounter->getAvarge())).c_str(), max(_physictimeCounter->getMin() - 1.0, 0.0), _physictimeCounter->getMax() + 2.0, ImVec2(0, 80.0f));

                ImGui::Text("Active-Bodies  : %d", _world->physicsstats.activebodies);
                ImGui::Text("Active-Joints  : %d", _world->physicsstats.activejoints);
                ImGui::Text("Collisions     : %d", _world->physicsstats.collisions);
                ImGui::Text("Iterations     : %d", _world->physicsstats.iterations);
                ImGui::Text("Particles      : %d", _world->physicsstats.particles);
                ImGui::Text("Substeps       : %d", _world->physicsstats.substeps);
                ImGui::Text("Threads        : %d", _world->physicsstats.threads);

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Animation"))
            {
                ImGui::PlotLines(("Animation-Time (" + String(_world->animationstats.time) + " ms)").c_str(), &_animationtimeCounter->data[0], _animationtimeCounter->data.size(), _animationtimeCounter->GetFrame(), ("average : " + String(_animationtimeCounter->getAvarge())).c_str(), max(_animationtimeCounter->getMin() - 5.0, 0.0), _animationtimeCounter->getMax() + 5.0, ImVec2(0, 80.0f));
               
                ImGui::Text("Bones          : %d", _world->animationstats.bones);
                ImGui::Text("Skeletons      : %d", _world->animationstats.skeletons);
                ImGui::Text("Threads        : %d", _world->animationstats.threads);


                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Camera-Settings"))
            {
                if (_camera != NULL)
                {
                    UpdateCameraValues(true);
                }

                ImGui::BeginDisabled(_camera == NULL);
                ImGui::Checkbox("Wireframe", &_wireframe);
                if (ImGui::BeginItemTooltip())
                {
                    ImGui::TextUnformatted("Toggles the 'Wireframe'-mode for the main-camera.");
                    ImGui::EndTooltip();
                }
                ImGui::Checkbox("Debug-Physics", &_debugCollision);
                ImGui::SliderFloat("Tesselation", &_tesselation, 0.0,50.0);

                ImGui::SliderFloat("Range-Near", &_cameraRange[0], 0.01, 100.0);
                ImGui::SliderFloat("Range-Far", &_cameraRange[1], 1.0, 100000.0);
                ImGui::ColorEdit4("Clear-Color", &_clearColor[0]);

                ImGui::EndDisabled();
                ImGui::EndTabItem();

                if (_camera != NULL)
                {
                    UpdateCameraValues(false);
                }
            }

            ImGui::EndTabBar();

            ImGui::End();
        }
    }

    void Show() { _isOpen = true; };

    void Toggle() { _isOpen = !_isOpen; };
};


int main(int argc, const char* argv[])
{
    auto plg = LoadPlugin("Plugins/FITextureLoader");
    auto plg2 = LoadPlugin("Plugins/KTX2TextureLoader");
    //Get the displays
    auto displays = GetDisplays();

    //Create a window
    auto window = CreateWindow("Ultra Engine", 0, 0, 1280, 720, displays[0], WINDOW_CENTER | WINDOW_TITLEBAR);

    //Create a world
    auto world = CreateWorld();

    //Create a framebuffer
    auto framebuffer = CreateFramebuffer(window);

    //Create a camera
    auto camera = CreateCamera(world);
    camera->SetPosition(0, 0, -1);
    camera->SetFov(70);
    camera->SetClearColor(0.125);
    camera->SetTessellation(4);

    //Create a light
    auto  light = CreateBoxLight(world);
    light->SetRange(-10, 10);
    light->SetRotation(35, 35, 0);
    light->SetColor(4);


    //Display material
    auto model = CreateCubeSphere(world, 0.5, 8, MESH_QUADS);
    model->SetCollisionType(1);
    auto mtl = LoadMaterial("Materials/rocks_ground_02.json");
    mtl->SetTessellation(true);
    mtl->SetDisplacement(0.075f);
    model->SetMaterial(mtl);

    //Entity component system
    auto component = model->AddComponent<Mover>();
    component->rotationspeed.y = 45;

    // Init the Manager
    auto imGuiManager = CreateImGuiManager(window, world, 2, framebuffer);
    auto debugRenderer = make_shared<ImDebugRenderer>(world, camera); // A custom class to render debug information for the world

    auto sampleTexture = mtl->GetTexture(0); // Get the diffuse Texture
    auto imGuiTexture = imGuiManager->RegisterTexture(sampleTexture); // Register the texture for usage with ImGui

    //Define some settings to control the camera and material settings
    bool camera_wireFrame = false;
    float tesselation_level = 4.0;
    Vec2 mtl_displacement = mtl->GetDisplacement();
    bool show_demo_window = false;
    bool show_metrics_window = false;
    bool show_about_window = false;
    bool show_texture_sample = false;
    bool show_settings_window = false;

    auto defaultFont = ImGui::GetIO().Fonts->AddFontDefault();
    

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    //Main loop
    while (window->Closed() == false and window->KeyDown(KEY_ESCAPE) == false)
    {
        //Begin the ui-definition
        imGuiManager->BeginFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
  
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                exit(0);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::SeparatorText("Dear ImGui");
            ImGui::MenuItem("Demonstration", 0, &show_demo_window);
            ImGui::MenuItem("Metrics", 0, &show_metrics_window);
            ImGui::SeparatorText("UltraEngine");
            ImGui::MenuItem("Debug-View", 0, &debugRenderer->_isOpen);
            ImGui::MenuItem("Settings", 0, &show_settings_window);
            ImGui::MenuItem("Texture", 0, &show_texture_sample);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        if (show_metrics_window)
        {
            ImGui::ShowMetricsWindow(&show_metrics_window);
        }

        debugRenderer->Render(); //render the debug panel (only when open)

        //Start rendering of the Settings
        if (show_settings_window)
        {
            ImGui::Begin("Material-Settings", &show_settings_window, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::SliderFloat2("Displacement", &mtl_displacement[0], -.2, .2, "%.3f");
            ImGui::End();
        }

        //Render a window containing the UltraEngine texture
        if (show_texture_sample)
        {
            ImGui::SetNextWindowSize(ImVec2(128, 128), ImGuiCond_Appearing);
            ImGui::Begin("Texture", &show_texture_sample);
            ImGuiID windowId = ImGui::GetItemID();

            ImGui::SetNextItemAllowOverlap();
            auto c_p = ImGui::GetContentRegionAvail();
            ImGui::Image(imGuiTexture, c_p);
            ImGui::End();
        }

        //Stop UI-Recording and update/create the models for UltraEngine
        imGuiManager->Sync();

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
        
        //Update the Settings modified by the UI
        mtl->SetDisplacement(mtl_displacement.x, mtl_displacement.y);
       
        world->Update();
        world->Render(framebuffer);
    }
    return 0;
}