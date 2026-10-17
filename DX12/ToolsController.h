#pragma once

#include "StandardIncludes.h"

class ToolsController : public Singleton<ToolsController>
{
public:
    ToolsController();
    virtual ~ToolsController();

    void LoadToolsLibrary();

    void ShowForm() const
    {
        m_showForm();
    }

    void CloseForm() const
    {
        if (m_closeForm != nullptr)
        {
            m_closeForm();
        }
    }

    int GetMode() const
    {
        return m_getMode();
    }

    bool ConsumeResetLight() const
    {
        return m_resetLight();
    }

    bool ConsumeResetCube() const
    {
        return m_resetCube();
    }

    float GetSpecularStrength() const
    {
        return m_getSpecularStrength();
    }

    XMFLOAT4 GetSpecularColor() const
    {
        return XMFLOAT4(
            m_getSpecularRed(),
            m_getSpecularGreen(),
            m_getSpecularBlue(),
            1.0f);
    }

private:
    typedef void(__stdcall* ToolsVoidFunction)();
    typedef bool(__stdcall* ToolsBoolFunction)();
    typedef int(__stdcall* ToolsIntFunction)();
    typedef float(__stdcall* ToolsFloatFunction)();

    HMODULE m_module;

    ToolsVoidFunction m_showForm;
    ToolsVoidFunction m_closeForm;

    ToolsIntFunction m_getMode;

    ToolsBoolFunction m_resetLight;
    ToolsBoolFunction m_resetCube;

    ToolsFloatFunction m_getSpecularStrength;
    ToolsFloatFunction m_getSpecularRed;
    ToolsFloatFunction m_getSpecularGreen;
    ToolsFloatFunction m_getSpecularBlue;
};