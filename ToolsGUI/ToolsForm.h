#pragma once

namespace ToolsGUI
{
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;

    public ref class ToolsForm : public Form
    {
    public:
        ToolsForm()
        {
            InitializeComponent();
        }

        CheckBox^ checkBoxRed;
        CheckBox^ checkBoxGreen;
        CheckBox^ checkBoxBlue;

    protected:
        ~ToolsForm()
        {
            if (components)
            {
                delete components;
            }
        }

    private:
        System::ComponentModel::Container^ components;

        CheckBox^ CreateCheckBox(
            String^ text,
            int y,
            bool isChecked)
        {
            CheckBox^ checkBox = gcnew CheckBox();

            checkBox->Text = text;
            checkBox->Location = Point(20, y);
            checkBox->AutoSize = true;
            checkBox->Checked = isChecked;

            return checkBox;
        }

        void InitializeComponent()
        {
            components =
                gcnew System::ComponentModel::Container();

            SuspendLayout();

            checkBoxRed = CreateCheckBox(L"Red", 25, false);
            checkBoxGreen = CreateCheckBox(L"Green", 45, true);
            checkBoxBlue = CreateCheckBox(L"Blue", 65, false);

            Controls->Add(checkBoxRed);
            Controls->Add(checkBoxGreen);
            Controls->Add(checkBoxBlue);

            AutoScaleDimensions = SizeF(6.0f, 13.0f);
            AutoScaleMode =
                System::Windows::Forms::AutoScaleMode::Font;

            ClientSize = System::Drawing::Size(135, 250);

            FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedToolWindow;

            ControlBox = false;
            MaximizeBox = false;
            MinimizeBox = false;

            Name = L"ToolsForm";
            Text = L"ToolsForm";
            TopMost = true;

            ResumeLayout(false);
            PerformLayout();
        }
    };
}