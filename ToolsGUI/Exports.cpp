#include "ToolsForm.h"

using namespace ToolsGUI;
using namespace System;
using namespace System::Windows::Forms;

static ToolsForm^ FindBackgroundForm()
{
    for (int i = 0; i < Application::OpenForms->Count; ++i)
    {
        ToolsForm^ form =
            dynamic_cast<ToolsForm^>(
                Application::OpenForms[i]);

        if (form != nullptr)
        {
            return form;
        }
    }

    return nullptr;
}

static LightingForm^ FindLightingForm()
{
    for (int i = 0; i < Application::OpenForms->Count; ++i)
    {
        LightingForm^ form =
            dynamic_cast<LightingForm^>(
                Application::OpenForms[i]);

        if (form != nullptr)
        {
            return form;
        }
    }

    return nullptr;
}

extern "C" __declspec(dllexport)
void __stdcall ShowToolsForm()
{
    ToolsForm^ background = FindBackgroundForm();
    LightingForm^ lighting = FindLightingForm();

    if (background == nullptr && lighting == nullptr)
    {
        Application::EnableVisualStyles();
    }

    if (background == nullptr)
    {
        background = gcnew ToolsForm();
        background->Show();
    }

    if (lighting == nullptr)
    {
        lighting = gcnew LightingForm();
        lighting->Show();
    }
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsRedData()
{
    ToolsForm^ form = FindBackgroundForm();

    return form != nullptr
        ? form->checkBoxRed->Checked
        : false;
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsGreenData()
{
    ToolsForm^ form = FindBackgroundForm();

    return form != nullptr
        ? form->checkBoxGreen->Checked
        : false;
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsBlueData()
{
    ToolsForm^ form = FindBackgroundForm();

    return form != nullptr
        ? form->checkBoxBlue->Checked
        : false;
}

extern "C" __declspec(dllexport)
float __stdcall GetToolsSpecularPower()
{
    LightingForm^ form = FindLightingForm();

    return form != nullptr
        ? static_cast<float>(form->trackBarPower->Value)
        : 32.0f;
}

extern "C" __declspec(dllexport)
float __stdcall GetToolsSpecularStrength()
{
    LightingForm^ form = FindLightingForm();

    return form != nullptr
        ? static_cast<float>(
            form->trackBarStrength->Value) / 100.0f
        : 0.8f;
}