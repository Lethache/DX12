#pragma once

namespace ToolsGUI
{
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;

    public ref class ToolsForm : public Form
    {
    public:
        RadioButton^ moveLight;
        RadioButton^ colorByPosition;
        RadioButton^ moveToCube;

        TrackBar^ strength;
        TrackBar^ red;
        TrackBar^ green;
        TrackBar^ blue;

        bool resetLight;
        bool resetCube;

        ToolsForm()
        {
            Text = L"ToolsForm";

            ClientSize =
                System::Drawing::Size(540, 390);

            FormBorderStyle =
                System::Windows::Forms::FormBorderStyle::FixedToolWindow;

            ControlBox = false;
            MaximizeBox = false;
            MinimizeBox = false;

            StartPosition = FormStartPosition::Manual;
            Location = Point(20, 80);

            resetLight = false;
            resetCube = false;

            moveLight = AddMode(
                L"Move Light", 15);

            moveLight->Checked = true;

            Button^ resetLightButton = AddButton(
                L"Reset Light Position", 42);

            resetLightButton->Click +=
                gcnew EventHandler(
                    this,
                    &ToolsForm::ResetLight);

            strength = AddSlider(
                L"Specular Strength",
                80,
                200,
                80);

            red = AddSlider(
                L"Specular Color   R",
                130,
                100,
                100);

            green = AddSlider(
                L"G",
                175,
                100,
                100);

            blue = AddSlider(
                L"B",
                220,
                100,
                100);

            colorByPosition = AddMode(
                L"Color By Position", 270);

            Button^ resetCubeButton = AddButton(
                L"Reset Cube Position", 297);

            resetCubeButton->Click +=
                gcnew EventHandler(
                    this,
                    &ToolsForm::ResetCube);

            moveToCube = AddMode(
                L"Move Light To Cube", 345);
        }

    private:
        RadioButton^ AddMode(String^ title, int y)
        {
            RadioButton^ item = gcnew RadioButton();

            item->Text = title;
            item->Location = Point(18, y);
            item->AutoSize = true;

            Controls->Add(item);

            return item;
        }

        Button^ AddButton(String^ title, int y)
        {
            Button^ item = gcnew Button();

            item->Text = title;
            item->Location = Point(35, y);

            item->Size =
                System::Drawing::Size(150, 25);

            Controls->Add(item);

            return item;
        }

        TrackBar^ AddSlider(
            String^ title,
            int y,
            int maximum,
            int value)
        {
            Label^ label = gcnew Label();

            label->Text = title;
            label->Location = Point(20, y + 8);
            label->AutoSize = true;

            Controls->Add(label);

            TrackBar^ bar = gcnew TrackBar();

            bar->Location = Point(140, y);

            bar->Size =
                System::Drawing::Size(380, 40);

            bar->Minimum = 0;
            bar->Maximum = maximum;
            bar->Value = value;
            bar->TickFrequency = maximum / 10;

            Controls->Add(bar);

            return bar;
        }

        void ResetLight(Object^ sender, EventArgs^ e)
        {
            resetLight = true;
        }

        void ResetCube(Object^ sender, EventArgs^ e)
        {
            resetCube = true;
        }
    };
}