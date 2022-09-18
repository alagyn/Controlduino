#include <arduino_xinput.h>
#include <errorUtils.h>
#include <guiControl.h>
#include <serializer.h>

#include <iostream>
#include <sstream>

#include <GLFW/glfw3.h>
#include <backends\imgui_impl_glfw.h>
#include <backends\imgui_impl_opengl3.h>
#include <imgui.h>

namespace bdd {

    static void glfw_error_callback(int error, const char* description)
    {
        std::stringstream ss;
        ss << "GLFW Error " << error << ": " << description;
        displayError("GLFW", ss.str().c_str());
    }

    ControlduinoGUI::ControlduinoGUI()
        : window(nullptr)
        , port()
        , fresh(true)
        , infoPtr(nullptr)
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if(!glfwInit())
        {
            displayError("GUI", "Cannot Init GLFW");
            throw GUIError();
        }

        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        // Create window with graphics context
        window = glfwCreateWindow(480, 480, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
        if(window == NULL)
        {
            displayError("GUI", "Cannot create window");
            throw GUIError();
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    ControlduinoGUI::~ControlduinoGUI()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void ControlduinoGUI::setInfoPtr(XUSB_REPORT* infoPtr)
    {
        this->infoPtr = infoPtr;
    }

    constexpr int windowFlags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool ControlduinoGUI::poll(GUIMode mode)
    {
        bool run = true;
        // Main loop

        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        auto io = ImGui::GetIO();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Controlduino", nullptr, windowFlags);

        switch(mode)
        {
        case GUIMode::Port:
            run = drawComSelect();
            break;
        case GUIMode::Info:
            run = drawInfo();
            break;
        default:
            break;
        }

        ImGui::End();

        //ImGui::ShowDemoWindow();

        fresh = false;

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w,
                     clear_color.y * clear_color.w,
                     clear_color.z * clear_color.w,
                     clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);

        run = run && !glfwWindowShouldClose(window);

        return run;
    }

    void ControlduinoGUI::loop(GUIMode mode)
    {
        while(poll(mode))
        {
        }
    }

    std::string ControlduinoGUI::getComPort()
    {
        fresh = true;
        loop(GUIMode::Port);
        return port;
    }

    bool ControlduinoGUI::drawComSelect()
    {
        static std::vector<std::pair<std::string, std::string>> ports;

        if(ImGui::Button("Refresh") || fresh)
        {
            Serializer::checkPorts(ports);
            std::cout << "Found " << ports.size() << " Ports\n";
        }

        for(auto iter = ports.begin(); iter != ports.end(); ++iter)
        {
            std::stringstream ss;
            auto val = *iter;
            ss << val.first << ": " << val.second;
            ImGui::Text(ss.str().c_str());
            ImGui::SameLine();
            if(ImGui::Button("Select"))
            {
                port = val.first;
                return false;
            }
        }

        return true;
    }

    void ControlduinoGUI::drawInfo_Button(const char* title, uint16_t mask)
    {
        std::stringstream ss;
        ss << title << ": " << (bool)(infoPtr->wButtons & mask);
        ImGui::Text(ss.str().c_str());
    }

    float map(int16_t in, float min, float max)
    {
        constexpr float range = (float)UINT16_MAX;
        float x = in;

        return min + (max - min) * ((x - (float)INT16_MIN) / (range));
    }

    bool ControlduinoGUI::drawInfo()
    {
        drawInfo_Button("A", button::A);
        drawInfo_Button("B", button::B);
        drawInfo_Button("X", button::X);
        drawInfo_Button("Y", button::Y);

        ImGui::Separator();

        drawInfo_Button("Up", button::Up);
        drawInfo_Button("Down", button::Down);
        drawInfo_Button("Left", button::Left);
        drawInfo_Button("Right", button::Right);

        ImGui::Separator();

        drawInfo_Button("LB", button::LB);
        drawInfo_Button("RB", button::RB);

        drawInfo_Button("L3", button::L3);
        drawInfo_Button("R3", button::R3);

        {
            std::stringstream ss;
            ss << "LT: " << (int)infoPtr->bLeftTrigger;
            ImGui::Text(ss.str().c_str());
        }

        {
            std::stringstream ss;
            ss << "RT: " << (int)infoPtr->bRightTrigger;
            ImGui::Text(ss.str().c_str());
        }

        ImGui::Separator();

        drawInfo_Button("Start", button::Start);
        drawInfo_Button("Back", button::Back);

        ImDrawList* drawlist = ImGui::GetWindowDrawList();

        ImGui::Columns(2);
        ImGui::Text("Left Stick");
        {
            std::stringstream ss;
            ss << "X:" << infoPtr->sThumbLX << " Y:" << infoPtr->sThumbLY;
            ImGui::Text(ss.str().c_str());
        }

        ImVec2 c = ImGui::GetCursorPos();

        ImVec4 colorvec(1.0f, 1.0f, 1.0f, 1.0f);
        ImColor color = ImColor(colorvec);
        ImVec2 c2 = c;
        c2.x += 50;
        c2.y += 50;
        drawlist->AddRect(c, c2, color);

        ImVec2 c3 = c;

        c3.x = map(infoPtr->sThumbLX, c.x, c2.x);
        c3.y = map(infoPtr->sThumbLY, c.y, c2.y);

        drawlist->AddNgon(c3, 2, color, 4, 1);

        ImGui::NextColumn();
        ImGui::Text("Right Stick");

        {
            std::stringstream ss;
            ss << "X:" << infoPtr->sThumbRX << " Y:" << infoPtr->sThumbRY;
            ImGui::Text(ss.str().c_str());
        }
        c = ImGui::GetCursorPos();

        c.x += 70;
        c2 = c;

        c2.x += 50;
        c2.y += 50;
        drawlist->AddRect(c, c2, color);

        c3.x = map(infoPtr->sThumbRX, c.x, c2.x);
        c3.y = map(infoPtr->sThumbRY, c.y, c2.y);

        drawlist->AddNgon(c3, 2, color, 4, 1);

        ImGui::Columns(1);

        return true;
    }

} //namespace bdd