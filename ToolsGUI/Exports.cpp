#include "ToolsForm.h"

using namespace ToolsGUI;
using namespace System;
using namespace System::Windows::Forms;

static ToolsForm^ GetOpenedToolsForm()
{
    for (int i = 0; i < Application::OpenForms->Count; ++i)
    {
        ToolsForm^ form =
            dynamic_cast<ToolsForm^>(Application::OpenForms[i]);

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
    ToolsForm^ form = GetOpenedToolsForm();

    if (form == nullptr)
    {
        Application::EnableVisualStyles();

        form = gcnew ToolsForm();
        form->Show();
    }

    form->BringToFront();
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsRedData()
{
    ToolsForm^ form = GetOpenedToolsForm();

    return form != nullptr
        ? form->checkBoxRed->Checked
        : false;
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsGreenData()
{
    ToolsForm^ form = GetOpenedToolsForm();

    return form != nullptr
        ? form->checkBoxGreen->Checked
        : true;
}

extern "C" __declspec(dllexport)
bool __stdcall GetToolsBlueData()
{
    ToolsForm^ form = GetOpenedToolsForm();

    return form != nullptr
        ? form->checkBoxBlue->Checked
        : false;
}