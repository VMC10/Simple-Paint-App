#include <wx/wx.h>
#include "frame.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);


bool MyApp::OnInit()
{
    wxInitAllImageHandlers();
    MainFrame *frame = new MainFrame("New", wxDefaultPosition, wxDefaultSize);
    frame->Show(true);
    return true;
}


