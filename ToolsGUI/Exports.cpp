#include "ToolsForm.h"

using namespace ToolsGUI;
using namespace System;
using namespace System::Windows::Forms;

static ToolsForm^ GetOpenedToolsForm()
{
    if (Application::OpenForms->Count == 0)
    {
        return nullptr;
    }

    return dynamic_cast<ToolsForm^>(
        Application::OpenForms[0]);
}

extern "C" __declspec(dllexport)
void __stdcall ShowToolsForm()
{
    Application::EnableVisualStyles();

    Application::SetCompatibleTextRenderingDefault(
        false);

    ToolsForm^ form = gcnew ToolsForm();

    form->Show();
    form->BringToFront();
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsRedData()
{
    ToolsForm^ form = GetOpenedToolsForm();

    if (form == nullptr)
    {
        return false;
    }

    return form->checkBoxRed->Checked;
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsGreenData()
{
    ToolsForm^ form = GetOpenedToolsForm();

    if (form == nullptr)
    {
        return false;
    }

    return form->checkBoxGreen->Checked;
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsBlueData()
{
    ToolsForm^ form = GetOpenedToolsForm();

    if (form == nullptr)
    {
        return false;
    }

    return form->checkBoxBlue->Checked;
}