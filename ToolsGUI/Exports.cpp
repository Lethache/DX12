#include "ToolsForm.h"

using namespace ToolsGUI;
using namespace System::Windows::Forms;

static ToolsForm^ FindForm()
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

extern "C" __declspec(dllexport)
void __stdcall ShowToolsForm()
{
    if (FindForm() == nullptr)
    {
        Application::EnableVisualStyles();

        ToolsForm^ form = gcnew ToolsForm();
        form->Show();
    }
}

extern "C" __declspec(dllexport)
void __stdcall CloseToolsForm()
{
    ToolsForm^ form = FindForm();

    if (form != nullptr)
    {
        form->Close();
        delete form;
    }
}

extern "C" __declspec(dllexport)
int __stdcall GetToolsMode()
{
    ToolsForm^ form = FindForm();

    if (form == nullptr)
    {
        return 0;
    }

    if (form->colorByPosition->Checked)
    {
        return 1;
    }

    if (form->moveToCube->Checked)
    {
        return 2;
    }

    return 0;
}

extern "C" __declspec(dllexport)
bool __stdcall ConsumeResetLight()
{
    ToolsForm^ form = FindForm();

    if (form == nullptr)
    {
        return false;
    }

    bool result = form->resetLight;

    form->resetLight = false;

    return result;
}

extern "C" __declspec(dllexport)
bool __stdcall ConsumeResetCube()
{
    ToolsForm^ form = FindForm();

    if (form == nullptr)
    {
        return false;
    }

    bool result = form->resetCube;

    form->resetCube = false;

    return result;
}

extern "C" __declspec(dllexport)
float __stdcall GetToolsSpecularStrength()
{
    ToolsForm^ form = FindForm();

    if (form == nullptr)
    {
        return 0.8f;
    }

    return form->strength->Value / 100.0f;
}

extern "C" __declspec(dllexport)
float __stdcall GetToolsSpecularRed()
{
    ToolsForm^ form = FindForm();

    if (form == nullptr)
    {
        return 1.0f;
    }

    return form->red->Value / 100.0f;
}

extern "C" __declspec(dllexport)
float __stdcall GetToolsSpecularGreen()
{
    ToolsForm^ form = FindForm();

    if (form == nullptr)
    {
        return 1.0f;
    }

    return form->green->Value / 100.0f;
}

extern "C" __declspec(dllexport)
float __stdcall GetToolsSpecularBlue()
{
    ToolsForm^ form = FindForm();

    if (form == nullptr)
    {
        return 1.0f;
    }

    return form->blue->Value / 100.0f;
}