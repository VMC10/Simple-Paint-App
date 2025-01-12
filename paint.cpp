#include <wx/gdicmn.h>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <wx/gtk/brush.h>
#include <wx/gtk/pen.h>
#include "paint.h"
#include "enum.h"

PaintWindow::PaintWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size)
    : wxWindow(parent, id, pos, size)
{
    this->SetBackgroundStyle(wxBG_STYLE_PAINT);

    this->Bind(wxEVT_PAINT, &PaintWindow::OnPaint, this);
    this->Bind(wxEVT_LEFT_DOWN, &PaintWindow::OnMouseDown, this);
    this->Bind(wxEVT_MOTION, &PaintWindow::OnMouseMove, this);
    this->Bind(wxEVT_LEFT_UP, &PaintWindow::OnMouseUp, this);
    this->Bind(wxEVT_LEAVE_WINDOW, &PaintWindow::OnMouseLeave, this);

    BuildContextMenu();

    this->Bind(wxEVT_CONTEXT_MENU, &PaintWindow::OnContextMenuEvent, this);
}

void PaintWindow::setTool(Tools tool){
    currentTool = tool;
}

Tools PaintWindow::getTool() const {
    return currentTool;
}

void PaintWindow::OnPaint(wxPaintEvent &)
{
    wxAutoBufferedPaintDC dc(this);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();

    wxGraphicsContext *gc = wxGraphicsContext::Create(dc);

    if (gc)
    {
        DrawOnContext(gc);
        if (showEraser && isErasing)
        {
            gc->SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_SOLID)); // Đường viền đứt đoạn
            gc->SetBrush(*wxTRANSPARENT_BRUSH);             // Không tô màu bên trong
            gc->DrawEllipse(eraserPos.x - static_cast<float> (currentEraserSize) / 2,
                            eraserPos.y - static_cast<float> (currentEraserSize) / 2,
                            currentEraserSize,
                            currentEraserSize);
        }
        delete gc;
    }
}

void PaintWindow::DrawOnContext(wxGraphicsContext *gc)
{
    for (const auto &s : squiggles)
    {
        auto pointsVector = s.points;
        if (pointsVector.size() > 1)
        {
            gc->SetPen(wxPen(s.color, s.width));
            gc->StrokeLines(pointsVector.size(), pointsVector.data());
        }
    }
}

void PaintWindow::ShowSaveDialog()
{
    wxFileDialog saveFileDialog(this, _("Save"), "", "",
                                "PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxBitmap bitmap(this->GetSize() * this->GetContentScaleFactor());

    wxMemoryDC memDC;

    memDC.SetUserScale(this->GetContentScaleFactor(), this->GetContentScaleFactor());

    memDC.SelectObject(bitmap);
    memDC.SetBackground(*wxWHITE_BRUSH);
    memDC.Clear();

    wxGraphicsContext *gc = wxGraphicsContext::Create(memDC);

    if (gc)
    {
        DrawOnContext(gc);
        delete gc;
    }

    bitmap.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_PNG);
}

void PaintWindow::OnMouseDown(wxMouseEvent &event)
{   
    auto pt = event.GetPosition();
    if (isDrawing){
        squiggles.push_back({{pt}, penColor, currentPenSize});
        squiggles.back().points.push_back(pt);
    }
    else {
        squiggles.push_back({{pt}, eraserColor, currentEraserSize});
        squiggles.back().points.push_back(pt);
        wxSetCursor(wxCURSOR_BLANK);
    }
    isMouseMoving = true;
}

void PaintWindow::OnMouseMove(wxMouseEvent &event)
{
    if (isMouseMoving)
    {
        auto pt = event.GetPosition();
        if (isErasing){
            eraserPos = pt;
            showEraser = true;
            wxSetCursor(wxCURSOR_BLANK);
        }
        else {
            showEraser = false;
            wxSetCursor(*wxSTANDARD_CURSOR);
        }
        auto &currentSquiggle = squiggles.back();
        currentSquiggle.points.push_back(pt);
        Refresh();
    }

    else if (isErasing){
        auto pt = event.GetPosition();
        eraserPos = pt;
        showEraser = true;
        wxSetCursor(wxCURSOR_BLANK);
        Refresh();
    }
    else {
        showEraser = false;
        wxSetCursor(*wxSTANDARD_CURSOR);
        Refresh();
    }
}

void PaintWindow::OnMouseUp(wxMouseEvent &)
{
    isMouseMoving = false;
}

void PaintWindow::OnMouseLeave(wxMouseEvent &)
{
    isMouseMoving = false;
    showEraser = false;
    wxSetCursor(*wxSTANDARD_CURSOR);
    Refresh();
}

void PaintWindow::BuildContextMenu()
{
    auto clear = contextMenu.Append(wxID_ANY, "&Clear");
    auto save = contextMenu.Append(wxID_ANY, "Save &As...");

    this->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent &)
        {
            this->squiggles.clear();
            this->Refresh();
        },
        clear->GetId());

    this->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent &)
        {
            this->ShowSaveDialog();
        },
        save->GetId());
}

void PaintWindow::OnContextMenuEvent(wxContextMenuEvent &e)
{
    auto clientPos = e.GetPosition() == wxDefaultPosition
                         ? wxPoint(this->GetSize().GetWidth() / 2, this->GetSize().GetHeight() / 2)
                         : this->ScreenToClient(e.GetPosition());
    PopupMenu(&this->contextMenu, clientPos);
}

void PaintWindow::setDrawingMode(bool status){
    isDrawing = status;
    isErasing = !isDrawing;
}
