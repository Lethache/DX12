#pragma once

namespace ToolsGUI
{
    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class ToolsForm :
        public System::Windows::Forms::Form
    {
    public:
        ToolsForm(void)
        {
            InitializeComponent();
        }

        // Checkbox components must be public
        System::Windows::Forms::CheckBox^ checkBoxRed;
        System::Windows::Forms::CheckBox^ checkBoxGreen;
        System::Windows::Forms::CheckBox^ checkBoxBlue;

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

#pragma region Windows Form Designer generated code

        void InitializeComponent(void)
        {
            this->checkBoxRed =
                (gcnew System::Windows::Forms::CheckBox());

            this->checkBoxGreen =
                (gcnew System::Windows::Forms::CheckBox());

            this->checkBoxBlue =
                (gcnew System::Windows::Forms::CheckBox());

            this->SuspendLayout();

            // checkBoxRed
            this->checkBoxRed->AutoSize = true;
            this->checkBoxRed->Location =
                System::Drawing::Point(57, 49);

            this->checkBoxRed->Name =
                L"checkBoxRed";

            this->checkBoxRed->Size =
                System::Drawing::Size(46, 17);

            this->checkBoxRed->TabIndex = 0;
            this->checkBoxRed->Text = L"Red";

            this->checkBoxRed->UseVisualStyleBackColor =
                true;

            this->checkBoxRed->CheckedChanged +=
                gcnew System::EventHandler(
                    this,
                    &ToolsForm::checkBox1_CheckedChanged);

            // checkBoxGreen
            this->checkBoxGreen->AutoSize = true;

            this->checkBoxGreen->Location =
                System::Drawing::Point(57, 73);

            this->checkBoxGreen->Name =
                L"checkBoxGreen";

            this->checkBoxGreen->Size =
                System::Drawing::Size(55, 17);

            this->checkBoxGreen->TabIndex = 1;
            this->checkBoxGreen->Text = L"Green";

            this->checkBoxGreen->UseVisualStyleBackColor =
                true;

            // checkBoxBlue
            this->checkBoxBlue->AutoSize = true;

            this->checkBoxBlue->Location =
                System::Drawing::Point(57, 97);

            this->checkBoxBlue->Name =
                L"checkBoxBlue";

            this->checkBoxBlue->Size =
                System::Drawing::Size(47, 17);

            this->checkBoxBlue->TabIndex = 2;
            this->checkBoxBlue->Text = L"Blue";

            this->checkBoxBlue->UseVisualStyleBackColor =
                true;

            // ToolsForm
            this->AutoScaleDimensions =
                System::Drawing::SizeF(6, 13);

            this->AutoScaleMode =
                System::Windows::Forms::AutoScaleMode::Font;

            this->ClientSize =
                System::Drawing::Size(283, 342);

            this->ControlBox = false;

            this->Controls->Add(
                this->checkBoxBlue);

            this->Controls->Add(
                this->checkBoxGreen);

            this->Controls->Add(
                this->checkBoxRed);

            this->FormBorderStyle =
                System::Windows::Forms::
                FormBorderStyle::FixedToolWindow;

            this->Name = L"ToolsForm";
            this->Text = L"ToolsForm";
            this->TopMost = true;

            this->ResumeLayout(false);
            this->PerformLayout();
        }

#pragma endregion

    private:
        System::Void checkBox1_CheckedChanged(
            System::Object^ sender,
            System::EventArgs^ e)
        {
        }
    };
}