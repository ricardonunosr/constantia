#include "editor.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

float test[3]{1.0f, 1.0f, 1.0f};

void ImGuiInit(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiCleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorImGuiRender(bool editor)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool scene = false;
    static bool showDemo = false;
    if (showDemo)
        ImGui::ShowDemoWindow(&showDemo);

    if (editor)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Editor"))
            {
                ImGui::MenuItem("Scene", NULL, &scene);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (scene)
        {
            ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiCond_FirstUseEver);
            if (!ImGui::Begin("Scene", &scene))
            {
                ImGui::End();
                return;
            }

            ImGuiTreeNodeFlags flags = (true ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
            flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)3, flags, "Test");
            if (ImGui::IsItemClicked())
            {
                // m_SelectionContext = entity;
            }

            bool entityDeleted = false;
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete Entity"))
                    entityDeleted = true;

                ImGui::EndPopup();
            }

            if (opened)
            {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
                bool opened = ImGui::TreeNodeEx((void*)9817239, flags, "Test");
                if (opened)
                    ImGui::TreePop();
                ImGui::TreePop();
            }

            if (entityDeleted)
            {
                /*m_Context->DestroyEntity(entity);
                if (m_SelectionContext == entity)
                    m_SelectionContext = {};*/
            }

            ImGui::Separator();

            ImGui::DragFloat3("Position", &test[0], 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::DragFloat3("Rotation", &test[0], 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::DragFloat3("Scale", &test[0], 0.1f, 0.0f, 0.0f, "%.2f");

            ImGui::End();
        }
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
