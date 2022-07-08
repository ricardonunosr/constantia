#pragma once

struct GLFWwindow;

void ImGuiInit(GLFWwindow* window);
void ImGuiCleanup();
void EditorImGuiRender(bool editor);