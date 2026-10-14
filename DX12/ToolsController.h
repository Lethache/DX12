#pragma once

#include "StandardIncludes.h"

class ToolsController : public Singleton<ToolsController>
{
public:
    ToolsController();
    virtual ~ToolsController();

    typedef void(__stdcall* ShowToolsForm)();
    typedef bool(__stdcall* GetToolsBoolData)();

    void ShowForm() const
    {
        m_showForm();
    }

    bool GetRedData() const
    {
        return m_getRedData();
    }

    bool GetGreenData() const
    {
        return m_getGreenData();
    }

    bool GetBlueData() const
    {
        return m_getBlueData();
    }

    void LoadToolsLibrary();

private:
    HMODULE m_module;

    ShowToolsForm m_showForm;
    GetToolsBoolData m_getRedData;
    GetToolsBoolData m_getGreenData;
    GetToolsBoolData m_getBlueData;
};