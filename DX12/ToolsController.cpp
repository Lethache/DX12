#include "ToolsController.h"

ToolsController::ToolsController()
{
    m_module = nullptr;

    m_showForm = nullptr;
    m_getRedData = nullptr;
    m_getGreenData = nullptr;
    m_getBlueData = nullptr;
}

ToolsController::~ToolsController()
{
    if (m_module != nullptr)
    {
        FreeLibrary(m_module);
        m_module = nullptr;
    }
}

void ToolsController::LoadToolsLibrary()
{
    if (m_module != nullptr)
    {
        return;
    }

    m_module = LoadLibraryW(L"ToolsGUI.dll");

    M_ASSERT(
        m_module != nullptr,
        "Failed to load ToolsGUI.dll");

    m_showForm = reinterpret_cast<ShowToolsForm>(
        GetProcAddress(m_module, "ShowToolsForm"));

    m_getRedData = reinterpret_cast<GetToolsBoolData>(
        GetProcAddress(m_module, "GetToolsRedData"));

    m_getGreenData = reinterpret_cast<GetToolsBoolData>(
        GetProcAddress(m_module, "GetToolsGreenData"));

    m_getBlueData = reinterpret_cast<GetToolsBoolData>(
        GetProcAddress(m_module, "GetToolsBlueData"));

    M_ASSERT(
        m_showForm != nullptr,
        "Missing ShowToolsForm");

    M_ASSERT(
        m_getRedData != nullptr,
        "Missing GetToolsRedData");

    M_ASSERT(
        m_getGreenData != nullptr,
        "Missing GetToolsGreenData");

    M_ASSERT(
        m_getBlueData != nullptr,
        "Missing GetToolsBlueData");
}