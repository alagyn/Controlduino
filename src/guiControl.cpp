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
        : mode(Mode::None)
        , window(nullptr)
        , port()
        , fresh(true)
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
        window = glfwCreateWindow(720, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
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

    constexpr int windowFlags =
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    void ControlduinoGUI::loop()
    {
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        auto io = ImGui::GetIO();

        bool run = true;
        fresh = true;
        // Main loop
        while(!glfwWindowShouldClose(window) && run)
        {
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(io.DisplaySize);
            ImGui::Begin("Controlduino", nullptr, windowFlags);

            switch(mode)
            {
            case Mode::Port:
                run = drawComSelect();
                break;
            default:
                break;
            }

            ImGui::End();

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
        }
    }

    std::string ControlduinoGUI::getComPort()
    {
        mode = Mode::Port;
        loop();
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

} //namespace bdd