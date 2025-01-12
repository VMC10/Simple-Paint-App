#include <wx/dcbuffer.h>
#include <wx/colourdata.h>
#include <wx/colordlg.h>
#include <wx/splitter.h>
#include <wx/dc.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wrapsizer.h>
#include <wx/bitmap.h>
#include <wx/slider.h>
#include "frame.h"
#include "enum.h"


MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    wxSplitterWindow *splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER | wxSP_LIVE_UPDATE);

    splitter->SetMinimumPaneSize(FromDIP(150));
    paint = new PaintWindow(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    auto controlsPanel = BuildSidePanel(splitter);

    splitter->SplitVertically(controlsPanel, paint);
    splitter->SetSashPosition(FromDIP(160));

    this->SetSize(FromDIP(1000), FromDIP(500));
    this->SetMinSize({FromDIP(400), FromDIP(200)});
}

void MainFrame::UpdateToolsUI(wxButton *activeButton){
    for (auto button : toolButtons)
    {
        if (button == activeButton)
        {
            button->SetBackgroundColour(*wxLIGHT_GREY);
        }
        else {
            button->SetBackgroundColour(wxNullColour);
        }
        button->Refresh();
    }
}

void MainFrame::UpdateSliderUI(wxSlider* slider){
    if (paint->getTool() == Tools::Pen){
        slider->SetMax(paint->maxPenSize);
        slider->SetValue(paint->currentPenSize);
        slider->SetMin(paint->minPenSize);
    }
    else {
        slider->SetMax(paint->maxEraserSize);
        slider->SetValue(paint->currentEraserSize);
        slider->SetMin(paint->minEraserSize);
    }
    slider->Refresh();
}

void MainFrame::UpdateSize(wxStaticText *text){
    if (paint->getTool() == Tools::Pen){
        text->SetLabel(wxString::Format("Pen size: %d px", paint->currentPenSize));
    }
    else {
        text->SetLabel(wxString::Format("Eraser size: %d px", paint->currentEraserSize));
    }
}


wxPanel *MainFrame::BuildSidePanel(wxWindow *parent)
{
    auto sidePanel = new wxScrolled<wxPanel>(parent, wxID_ANY);
    sidePanel->SetScrollRate(0, FromDIP(10));
    sidePanel->SetBackgroundColour(wxColour(panelBackground));

    auto mainSizer = new wxBoxSizer(wxVERTICAL);
    
    auto text = new wxStaticText(sidePanel, wxID_ANY, "Colors");
    mainSizer->Add(text, 0, wxALL, FromDIP(5));

    auto colorPreview = new wxPanel(sidePanel, wxID_ANY, wxDefaultPosition, wxSize(50, 50));
    colorPreview->SetBackgroundStyle(wxBG_STYLE_PAINT);
    colorPreview->SetBackgroundColour(paint->penColor);
    colorPreview->Bind(wxEVT_PAINT, [colorPreview](wxPaintEvent &event){
        wxAutoBufferedPaintDC dc(colorPreview);
        wxSize size = colorPreview->GetClientSize();
        int border = 2;
        int corner = 10;
        wxBrush brush(colorPreview->GetBackgroundColour());
        dc.SetBrush(brush);
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRoundedRectangle(border, border, size.GetWidth() - 2*border, size.GetHeight() - 2*border, corner);
            });
    
    colorPreview->Bind(wxEVT_LEFT_DOWN, [this, sidePanel, colorPreview](wxMouseEvent &event){
                
                wxColourData colorData;
                colorData.SetChooseFull(true);
                colorData.SetColour(paint->penColor);
                wxColourDialog colorDialog(sidePanel, &colorData);
                if (colorDialog.ShowModal() == wxID_OK){
                    wxColourData retData = colorDialog.GetColourData();
                    wxColour selectedColor = retData.GetColour();
                    paint->penColor = selectedColor;
                    colorPreview->SetBackgroundColour(selectedColor);
                    colorPreview->Refresh();
                    }
            });

    mainSizer->Add(colorPreview, 0, wxALL, FromDIP(5));
        
    text = new wxStaticText(sidePanel, wxID_ANY, "Tools");
    mainSizer->Add(text, 0, wxALL, FromDIP(5));
    
    auto toolSizer = new wxBoxSizer(wxHORIZONTAL);
    
    auto penBitmap = wxBitmap("../icons/pen.png", wxBITMAP_TYPE_PNG);
    auto penButton = new wxBitmapButton(sidePanel, wxID_ANY, penBitmap);
    penButton->SetMinSize(wxSize(50, 50));
    toolButtons.push_back(penButton);
    toolSizer->Add(penButton, 0, wxALL, FromDIP(5));
    
    auto eraserBitmap = wxBitmap("../icons/eraser.png", wxBITMAP_TYPE_PNG);
    auto eraserButton = new wxBitmapButton(sidePanel, wxID_ANY, eraserBitmap);
    eraserButton->SetMinSize(wxSize(50, 50));
    toolButtons.push_back(eraserButton);
    toolSizer->Add(eraserButton, 0, wxALL, FromDIP(5));

    UpdateToolsUI(penButton);
    mainSizer->Add(toolSizer, 0, wxALL, FromDIP(5));

    auto sizeText = new wxStaticText(sidePanel, wxID_ANY, "");
    UpdateSize(sizeText);
    mainSizer->Add(sizeText, 0, wxALL, FromDIP(5));

    auto sizeSlider = new wxSlider(sidePanel, wxID_ANY, 0, 0, 0);
    UpdateSliderUI(sizeSlider);
    mainSizer->Add(sizeSlider, 0, wxALL, FromDIP(5));
    
    penButton->Bind(wxEVT_BUTTON, [this, penButton, eraserButton, sizeSlider, sizeText](wxCommandEvent $event){
            paint->setTool(Tools::Pen);
            paint->setDrawingMode(true);
            UpdateToolsUI(penButton);
            UpdateSliderUI(sizeSlider);
            UpdateSize(sizeText);
            });

    eraserButton->Bind(wxEVT_BUTTON, [this, penButton, eraserButton, sizeSlider, sizeText](wxCommandEvent $event){
            paint->setTool(Tools::Eraser);
            paint->setDrawingMode(false);
            UpdateToolsUI(eraserButton);
            UpdateSliderUI(sizeSlider);
            UpdateSize(sizeText);
            });

    sizeSlider->Bind(wxEVT_SLIDER, [this, sizeSlider, sizeText](wxCommandEvent &event) {
            
            if (paint->getTool() == Tools::Pen){
                paint->currentPenSize = sizeSlider->GetValue(); 
            }
            else {
                paint->currentEraserSize = sizeSlider->GetValue();
            }
            UpdateSize(sizeText);
            paint->Refresh(); 
        
        });


    auto saveButton = new wxButton(sidePanel, wxID_ANY, "Save As...");

    mainSizer->AddStretchSpacer();
    mainSizer->Add(saveButton, 0, wxALL, FromDIP(5));
    mainSizer->AddSpacer(FromDIP(5));

    sidePanel->SetSizer(mainSizer);

    saveButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent &event)
                 { paint->ShowSaveDialog(); });
    
    
    return sidePanel;
}

