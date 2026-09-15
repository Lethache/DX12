#include "ToolsController.h"

ToolsController::ToolsController()
{
    m_module = nullptr;
    m_showForm = nullptr;
    m_getRedData = nullptr;
    m_getGreenData = nullptr;
    m_getBlueData = nullptr;
}

void ToolsController::LoadToolsLibrary()
{
    m_module = LoadLibraryW(L"ToolsGUI.dll");

    M_ASSERT(
        m_module != nullptr,
        "Failed to load ToolsGUI.dll");

    FARPROC address = GetProcAddress(
        m_module,
        "ShowToolsForm");

    M_ASSERT(
        address != nullptr,
        "Failed to find ShowToolsForm");

    m_showForm =
        reinterpret_cast<ShowToolsForm>(address);

    address = GetProcAddress(
        m_module,
        "GetToolsRedData");

    M_ASSERT(
        address != nullptr,
        "Failed to find GetToolsRedData");

    m_getRedData =
        reinterpret_cast<GetToolsRedData>(address);

    address = GetProcAddress(
        m_module,
        "GetToolsGreenData");

    M_ASSERT(
        address != nullptr,
        "Failed to find GetToolsGreenData");

    m_getGreenData =
        reinterpret_cast<GetToolsGreenData>(address);

    address = GetProcAddress(
        m_module,
        "GetToolsBlueData");

    M_ASSERT(
        address != nullptr,
        "Failed to find GetToolsBlueData");

    m_getBlueData =
        reinterpret_cast<GetToolsBlueData>(address);
}