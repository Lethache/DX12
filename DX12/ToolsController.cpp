#include "ToolsController.h"

ToolsController::ToolsController()
{
    m_module = nullptr;

    m_showForm = nullptr;
    m_closeForm = nullptr;

    m_getMode = nullptr;

    m_resetLight = nullptr;
    m_resetCube = nullptr;

    m_getSpecularStrength = nullptr;
    m_getSpecularRed = nullptr;
    m_getSpecularGreen = nullptr;
    m_getSpecularBlue = nullptr;
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
        "Failed to load ToolsGUI.dll. Build ToolsGUI first.");

    m_showForm =
        reinterpret_cast<ToolsVoidFunction>(
            GetProcAddress(
                m_module,
                "ShowToolsForm"));

    m_closeForm =
        reinterpret_cast<ToolsVoidFunction>(
            GetProcAddress(
                m_module,
                "CloseToolsForm"));

    m_getMode =
        reinterpret_cast<ToolsIntFunction>(
            GetProcAddress(
                m_module,
                "GetToolsMode"));

    m_resetLight =
        reinterpret_cast<ToolsBoolFunction>(
            GetProcAddress(
                m_module,
                "ConsumeResetLight"));

    m_resetCube =
        reinterpret_cast<ToolsBoolFunction>(
            GetProcAddress(
                m_module,
                "ConsumeResetCube"));

    m_getSpecularStrength =
        reinterpret_cast<ToolsFloatFunction>(
            GetProcAddress(
                m_module,
                "GetToolsSpecularStrength"));

    m_getSpecularRed =
        reinterpret_cast<ToolsFloatFunction>(
            GetProcAddress(
                m_module,
                "GetToolsSpecularRed"));

    m_getSpecularGreen =
        reinterpret_cast<ToolsFloatFunction>(
            GetProcAddress(
                m_module,
                "GetToolsSpecularGreen"));

    m_getSpecularBlue =
        reinterpret_cast<ToolsFloatFunction>(
            GetProcAddress(
                m_module,
                "GetToolsSpecularBlue"));

    M_ASSERT(
        m_showForm != nullptr,
        "Missing ShowToolsForm.");

    M_ASSERT(
        m_closeForm != nullptr,
        "Missing CloseToolsForm.");

    M_ASSERT(
        m_getMode != nullptr,
        "Missing GetToolsMode.");

    M_ASSERT(
        m_resetLight != nullptr,
        "Missing ConsumeResetLight.");

    M_ASSERT(
        m_resetCube != nullptr,
        "Missing ConsumeResetCube.");

    M_ASSERT(
        m_getSpecularStrength != nullptr,
        "Missing GetToolsSpecularStrength.");

    M_ASSERT(
        m_getSpecularRed != nullptr,
        "Missing GetToolsSpecularRed.");

    M_ASSERT(
        m_getSpecularGreen != nullptr,
        "Missing GetToolsSpecularGreen.");

    M_ASSERT(
        m_getSpecularBlue != nullptr,
        "Missing GetToolsSpecularBlue.");
}