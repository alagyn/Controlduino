#include <arduino_xinput.h>
#include <errorUtils.h>
#include <guiControl.h>
#include <serializer.h>

#include <exception>
#include <iostream>
#include <sstream>

#include <GLFW/glfw3.h>
#include <backends\imgui_impl_glfw.h>
#include <backends\imgui_impl_opengl3.h>
#include <imgui.h>

namespace bdd {

    float map(int16_t in, float min, float max);

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
        , state()
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

        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
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

    void ControlduinoGUI::setState(XUSB_REPORT state)
    {
        this->state = state;
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
        case GUIMode::Calib:
            run = drawCalib();
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

        if(glfwWindowShouldClose(window))
        {
            throw GUIExit();
        }

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

    enum class CalibState
    {
        LX,
        LY,
        RX,
        RY
    };

    CalibState calibState = CalibState::LX;

    void ControlduinoGUI::setCalib(Calibration* calib)
    {
        if(!calib)
        {
            throw GUIError();
        }
        this->calib = calib;
    }

    ReadManager* readMan;

    void ControlduinoGUI::runCalibration(ReadManager* rm)
    {
        readMan = rm;
        calibState = CalibState::LX;
        fresh = true;
        loop(GUIMode::Calib);
        calib->writeCalibFile();
    }

    float map(int16_t in, float min, float max)
    {
        constexpr float range = (float)1024.0;
        float x = in;

        return min + ((max - min) * (x) / range);
    }

    uint16_t newMin = 1024 / 2, newMax = 1024 / 2;

    void resetCalibVals()
    {
        newMin = 1024 / 2;
        newMax = 1024 / 2;
    }

    bool ControlduinoGUI::drawCalib()
    {
        ImDrawList* drawlist = ImGui::GetWindowDrawList();

        std::string text;
        uint16_t curMin, curWidth, curVal;

        XUSB_REPORT state = readMan->updateState();

        switch(calibState)
        {
        case CalibState::LX:
            text = "Left X";
            curMin = calib->lxMin;
            curWidth = calib->lxWidth;
            curVal = state.sThumbLX;
            break;
        case CalibState::LY:
            text = "Left Y";
            curMin = calib->lyMin;
            curWidth = calib->lyWidth;
            curVal = state.sThumbLY;
            break;
        case CalibState::RX:
            text = "Right X";
            curMin = calib->rxMin;
            curWidth = calib->rxWidth;
            curVal = state.sThumbRX;
            break;
        case CalibState::RY:
            text = "Right Y";
            curMin = calib->ryMin;
            curWidth = calib->ryWidth;
            curVal = state.sThumbRY;
            break;
        default:
            break;
        }

        {
            ImGui::Text("Calibrating");
            ImGui::Text(text.c_str());
            std::stringstream ss;
            ss << "Val: " << curVal;
            ImGui::Text(ss.str().c_str());
        }

        constexpr float WIDTH = 200;
        constexpr float HEIGHT = 35;
        constexpr float OFFSET = 10;
        constexpr float OFFSET_HEIGHT = HEIGHT - (OFFSET);
        constexpr float MIDDLE = HEIGHT / 2;

        ImVec2 origin = ImGui::GetCursorPos();

        constexpr ImU32 EDGE_COL = IM_COL32(100, 100, 255, 255);

        drawlist->AddLine(origin, ImVec2(origin.x, origin.y + HEIGHT), EDGE_COL);
        drawlist->AddLine(
            ImVec2(origin.x + WIDTH, origin.y), ImVec2(origin.x + WIDTH, origin.y + HEIGHT), EDGE_COL);

        constexpr ImU32 BOUND_COL = IM_COL32(255, 50, 50, 255);

        float dist = map(newMin, origin.x, origin.x + WIDTH);

        drawlist->AddLine(ImVec2(dist, origin.y + OFFSET), ImVec2(dist, origin.y + OFFSET_HEIGHT), BOUND_COL);

        dist = map(newMax, origin.x, origin.x + WIDTH);

        drawlist->AddLine(ImVec2(dist, origin.y + OFFSET), ImVec2(dist, origin.y + OFFSET_HEIGHT), BOUND_COL);

        dist = map(curVal, origin.x, origin.x + WIDTH);

        constexpr float DOT_SIZE = 4;
        constexpr ImU32 DOT_COL = IM_COL32(50, 255, 50, 255);

        drawlist->AddCircleFilled(ImVec2(dist, origin.y + MIDDLE), DOT_SIZE, DOT_COL);

        ImGui::SetCursorPosY(origin.y + HEIGHT + 5);

        newMin = newMin < curVal ? newMin : curVal;
        newMax = newMax > curVal ? newMax : curVal;

        {
            std::stringstream xxx;
            xxx << "Min: " << newMin << " Max: " << newMax;
            ImGui::Text(xxx.str().c_str());
        }

        if(ImGui::Button("Prev"))
        {
            switch(calibState)
            {
            case CalibState::LX:
                // Pass
                break;
            case CalibState::LY:
                calibState = CalibState::LX;
                resetCalibVals();
                break;
            case CalibState::RX:
                calibState = CalibState::LY;
                resetCalibVals();
                break;
            case CalibState::RY:
                calibState = CalibState::RX;
                resetCalibVals();
                break;
            }
        }

        if(ImGui::Button("Reset"))
        {
            resetCalibVals();
        }

        if(ImGui::Button("Next"))
        {
            switch(calibState)
            {
            case CalibState::LX:
                calibState = CalibState::LY;
                calib->lxMin = newMin;
                calib->lxWidth = newMax - newMin;
                resetCalibVals();
                break;
            case CalibState::LY:
                calibState = CalibState::RX;
                calib->lyMin = newMin;
                calib->lyWidth = newMax - newMin;
                resetCalibVals();
                break;
            case CalibState::RX:
                calibState = CalibState::RY;
                calib->rxMin = newMin;
                calib->rxWidth = newMax - newMin;
                resetCalibVals();
                break;
            case CalibState::RY:
                calib->ryMin = newMin;
                calib->ryWidth = newMax - newMin;
                return false;
            }
        }

        return true;
    }

    void ControlduinoGUI::drawInfo_Button(const char* title, uint16_t mask)
    {
        std::stringstream ss;
        ss << title << ": " << (bool)(state.wButtons & mask);
        ImGui::Text(ss.str().c_str());
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
            ss << "LT: " << (int)state.bLeftTrigger;
            ImGui::Text(ss.str().c_str());
        }

        {
            std::stringstream ss;
            ss << "RT: " << (int)state.bRightTrigger;
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
            ss << "X:" << state.sThumbLX << " Y:" << state.sThumbLY;
            ImGui::Text(ss.str().c_str());
        }

        ImVec2 c = ImGui::GetCursorPos();
        c.y += 20;

        ImVec4 colorvec(1.0f, 1.0f, 1.0f, 1.0f);
        ImColor color = ImColor(colorvec);
        ImVec2 c2 = c;
        c2.x += 50;
        c2.y += 50;
        drawlist->AddRect(c, c2, color);

        ImVec2 c3 = c;

        c3.x = map(state.sThumbLX, c.x, c2.x);
        c3.y = map(state.sThumbLY, c.y, c2.y);

        {
            std::stringstream ss;
            ss << "X: " << c3.x - c.x;
            ImGui::Text(ss.str().c_str());
        }

        drawlist->AddNgon(c3, 2, color, 4, 1);

        ImGui::NextColumn();
        ImGui::Text("Right Stick");

        {
            std::stringstream ss;
            ss << "X:" << state.sThumbRX << " Y:" << state.sThumbRY;
            ImGui::Text(ss.str().c_str());
        }
        c = ImGui::GetCursorPos();

        c.x += 70;
        c2 = c;

        c2.x += 50;
        c2.y += 50;
        drawlist->AddRect(c, c2, color);

        c3.x = map(state.sThumbRX, c.x, c2.x);
        c3.y = map(state.sThumbRY, c.y, c2.y);

        drawlist->AddNgon(c3, 2, color, 4, 1);

        ImGui::Columns(1);

        return true;
    }

} //namespace bdd