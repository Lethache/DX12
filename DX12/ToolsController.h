#pragma once

#include "StandardIncludes.h"

class ToolsController :
    public Singleton<ToolsController>
{
public:
    // Constructors / Destructors
    ToolsController();
    virtual ~ToolsController() {}

    // Function pointer types
    typedef void(__stdcall* ShowToolsForm)();
    typedef bool(__stdcall* GetToolsRedData)();
    typedef bool(__stdcall* GetToolsGreenData)();
    typedef bool(__stdcall* GetToolsBlueData)();

    // Accessors
    void ShowForm() const
    {
        m_showForm();
    }

    int GetRedData() const
    {
        return m_getRedData();
    }

    int GetGreenData() const
    {
        return m_getGreenData();
    }

    int GetBlueData() const
    {
        return m_getBlueData();
    }

    // Methods
    void LoadToolsLibrary();

private:
    // Members
    HMODULE m_module;

    ShowToolsForm m_showForm;
    GetToolsRedData m_getRedData;
    GetToolsGreenData m_getGreenData;
    GetToolsBlueData m_getBlueData;
};