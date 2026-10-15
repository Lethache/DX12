#pragma once

namespace ToolsGUI
{
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;

    public ref class ToolsForm : public Form
    {
    public:
        CheckBox^ checkBoxRed;
        CheckBox^ checkBoxGreen;
        CheckBox^ checkBoxBlue;

        ToolsForm()
        {
            checkBoxRed = CreateCheckBox(L"Red", 20);
            checkBoxGreen = CreateCheckBox(L"Green", 45);
            checkBoxBlue = CreateCheckBox(L"Blue", 70);

            Controls->Add(checkBoxRed);
            Controls->Add(checkBoxGreen);
            Controls->Add(checkBoxBlue);

            Text = L"Background";
            ClientSize = System::Drawing::Size(135, 120);

            FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedToolWindow;

            ControlBox = false;
            TopMost = true;
            MaximizeBox = false;
            MinimizeBox = false;
        }

    private:
        CheckBox^ CreateCheckBox(String^ text, int y)
        {
            CheckBox^ box = gcnew CheckBox();

            box->Text = text;
            box->Location = Point(15, y);
            box->AutoSize = true;
            box->Checked = false;

            return box;
        }
    };

    public ref class LightingForm : public Form
    {
    public:
        TrackBar^ trackBarPower;
        TrackBar^ trackBarStrength;

        LightingForm()
        {
            Text = L"Lighting Controls";

            ClientSize =
                System::Drawing::Size(360, 205);

            FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedToolWindow;

            ControlBox = false;
            TopMost = true;
            MaximizeBox = false;
            MinimizeBox = false;

            StartPosition =
                FormStartPosition::Manual;

            Location = Point(180, 80);

            Label^ powerTitle = gcnew Label();
            powerTitle->Text = L"Specular Power";
            powerTitle->Location = Point(15, 15);
            powerTitle->AutoSize = true;

            trackBarPower = gcnew TrackBar();
            trackBarPower->Location = Point(10, 40);
            trackBarPower->Size =
                System::Drawing::Size(280, 45);

            trackBarPower->Minimum = 1;
            trackBarPower->Maximum = 128;
            trackBarPower->Value = 32;
            trackBarPower->TickFrequency = 16;

            labelPower = gcnew Label();
            labelPower->Location = Point(300, 45);
            labelPower->AutoSize = true;

            Label^ strengthTitle = gcnew Label();
            strengthTitle->Text = L"Specular Strength";
            strengthTitle->Location = Point(15, 100);
            strengthTitle->AutoSize = true;

            trackBarStrength = gcnew TrackBar();
            trackBarStrength->Location = Point(10, 125);
            trackBarStrength->Size =
                System::Drawing::Size(280, 45);

            trackBarStrength->Minimum = 0;
            trackBarStrength->Maximum = 200;
            trackBarStrength->Value = 80;
            trackBarStrength->TickFrequency = 20;

            labelStrength = gcnew Label();
            labelStrength->Location = Point(300, 130);
            labelStrength->AutoSize = true;

            trackBarPower->ValueChanged +=
                gcnew EventHandler(
                    this,
                    &LightingForm::SliderChanged);

            trackBarStrength->ValueChanged +=
                gcnew EventHandler(
                    this,
                    &LightingForm::SliderChanged);

            Controls->Add(powerTitle);
            Controls->Add(trackBarPower);
            Controls->Add(labelPower);

            Controls->Add(strengthTitle);
            Controls->Add(trackBarStrength);
            Controls->Add(labelStrength);

            UpdateLabels();
        }

    private:
        Label^ labelPower;
        Label^ labelStrength;

        void UpdateLabels()
        {
            labelPower->Text =
                trackBarPower->Value.ToString();

            float strength =
                static_cast<float>(
                    trackBarStrength->Value) / 100.0f;

            labelStrength->Text =
                strength.ToString(L"F2");
        }

        void SliderChanged(Object^ sender, EventArgs^ e)
        {
            UpdateLabels();
        }
    };
}