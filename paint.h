#pragma once
#include <wx/gdicmn.h>
#include <wx/wx.h>
#include <wx/overlay.h>
#include <vector>
#include "path.h"
#include "enum.h"

class PaintWindow : public wxWindow
{
public:
    PaintWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size);
    virtual ~PaintWindow() noexcept {}
    void setTool(Tools tool);
    Tools getTool() const;
    void ShowSaveDialog();

    int currentPenSize{1};
    int currentEraserSize{10};
    int maxPenSize{30};
    int minPenSize{1};
    int maxEraserSize{100};
    int minEraserSize{1};
    bool isDrawing{true};
    bool isErasing{false};
    wxColour penColor{*wxBLACK};
    wxColour eraserColor{*wxWHITE};
    void setDrawingMode(bool status);

private:
    void OnPaint(wxPaintEvent &);
    void DrawOnContext(wxGraphicsContext *gc);
    void OnMouseDown(wxMouseEvent &);
    void OnMouseMove(wxMouseEvent &);
    void OnMouseUp(wxMouseEvent &);
    void OnMouseLeave(wxMouseEvent &);
    bool isMouseMoving{};
    std::vector<Path> squiggles;
    Tools currentTool = Tools::Pen;
    wxMenu contextMenu;
    void BuildContextMenu();
    void OnContextMenuEvent(wxContextMenuEvent &);
    bool showEraser{false};
    wxPoint eraserPos{-1, -1};

};
