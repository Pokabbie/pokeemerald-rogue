#include "Window.h"

#include "Defines.h"
#include "Log.h"

#ifdef IMGUI_SUPPORT
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"
#endif

#include <GLFW/glfw3.h>

Window::Window(WindowConfig const& config)
    : m_Config(config)
    , m_GlfwWindow(nullptr)
{
}

bool Window::Create()
{
    LOG_INFO("Creating Window");

    if (m_GlfwWindow != nullptr)
    {
        ASSERT_FAIL("Window already Created");
        return false;
    }

    if (!glfwInit())
    {
        ASSERT_FAIL("Failed to initialise GLFW");
        return false;
    }

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    m_GlfwWindow = glfwCreateWindow(640, 480, m_Config.title.c_str(), NULL, NULL);
    if (!m_GlfwWindow)
    {
        glfwTerminate();
        ASSERT_FAIL("Failed to create Window");
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_GlfwWindow);
    glfwSwapInterval(1);

#ifdef IMGUI_SUPPORT
    if (m_Config.imGuiEnabled)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_GlfwWindow, true);
        //ImGui_ImplOpenGL3_Init("#version 130");
        ImGui_ImplOpenGL2_Init();
    }
#endif

    return true;
}

bool Window::Destroy()
{
    if (m_GlfwWindow == nullptr)
    {
        ASSERT_FAIL("Window not created yet");
        return false;
    }


#ifdef IMGUI_SUPPORT
    if (m_Config.imGuiEnabled)
    {
        // Cleanup
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }
#endif

    glfwDestroyWindow(m_GlfwWindow);
    glfwTerminate();

    m_GlfwWindow = nullptr;
    return true;
}

void Window::EnterMainLoop(WindowCallback callback, void* userData)
{
    if (m_GlfwWindow == nullptr)
    {
        ASSERT_FAIL("Window not created yet");
        return;
    }

    bool continueLoop = true;

#ifdef IMGUI_SUPPORT
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
#endif

    while (!glfwWindowShouldClose(m_GlfwWindow) && continueLoop)
    {
        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        //glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

#ifdef IMGUI_SUPPORT
        if (m_Config.imGuiEnabled)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
#endif

        continueLoop = callback(this, userData);

#ifdef IMGUI_SUPPORT
        if (m_Config.imGuiEnabled)
        {
            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
            {
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

                ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
                ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
                ImGui::Checkbox("Another Window", &show_another_window);

                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

                if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                    counter++;
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

                if (ImGui::Button("Dark"))
                    ImGui::StyleColorsDark();

                if (ImGui::Button("Light"))
                    ImGui::StyleColorsLight();

                ImGui::End();
            }

            // 3. Show another simple window.
            if (true)
            {
                ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    show_another_window = false;
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
        }
#endif

        int display_w, display_h;
        glfwGetFramebufferSize(m_GlfwWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

#ifdef IMGUI_SUPPORT
        if (m_Config.imGuiEnabled)
        {
            ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
            ImGuiIO& io = ImGui::GetIO();

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }
#endif

        glfwSwapBuffers(m_GlfwWindow);
    }

}
