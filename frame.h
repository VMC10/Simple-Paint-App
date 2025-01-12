#pragma once
#include <string>
#include <vector>
#include "paint.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

private:
    wxPanel *BuildSidePanel(wxWindow *parent);
    void UpdateToolsUI(wxButton* activeButton);
    void UpdateSliderUI(wxSlider* slider);
    void UpdateSize(wxStaticText *string);
    std::vector<wxButton *> toolButtons;
    PaintWindow *paint;
    const std::string panelBackground = "#2c2828";
};