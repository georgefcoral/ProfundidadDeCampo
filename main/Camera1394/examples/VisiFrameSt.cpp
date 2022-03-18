/*  
 	 VisiFrameSt.cpp - an example that captures images from two cameras,
	 and display each of them on a window.
	 
    Copyright (C) 2010  Arturo Espinosa-Romero (arturoemx@gmail.com)
    Facultad de Matemáticas, Universidad Autónoma de Yucatán, México.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "wx/sizer.h"
#include "wx/wx.h"
#include <Camera1394.h>
 
class BasicDrawPane;
 
class RenderTimer : public wxTimer
{
    BasicDrawPane* pane;
	 uint32_t RenderInterval;
public:
    RenderTimer(BasicDrawPane* pane);
	 void SetRenderInterval(uint32_t ri)
	 {
		RenderInterval = ri;
	 }
    void Notify();
    void start();
};
 
class MyFrame;
 
class BasicDrawPane : public wxPanel
{

	 uint32_t Rows, Cols, Size;
public:
    BasicDrawPane(MyFrame* parent);
	 MyFrame *Frame;
	 uint8_t *image_data;
	 wxImage I;
	
	 void Init();
	 void Set_Rows(uint32_t r) {Rows = r;}
	 void Set_Cols(uint32_t c) {Cols = c;}
	 uint32_t Get_Rows() {return Rows;}
	 uint32_t Get_Cols() {return Cols;}
    void paintEvent(wxPaintEvent& evt);
    void paintNow();
    void render( wxDC& dc );
    
    DECLARE_EVENT_TABLE()
};
 
class MyFrame;
 
class MyApp: public wxApp
{
    bool OnInit();
    
    MyFrame *frame1, *frame2;
public:
        
};
 
 
RenderTimer::RenderTimer(BasicDrawPane* pane) : wxTimer()
{
    RenderTimer::pane = pane;
}
 
void RenderTimer::Notify()
{
    pane->Refresh();
}
 
void RenderTimer::start()
{
    wxTimer::Start(RenderInterval);
}
 
IMPLEMENT_APP(MyApp)
 
class MyFrame : public wxFrame, public Camera1394
{
    RenderTimer* timer;
    BasicDrawPane* drawPane;
    
public:
    MyFrame(const uint32_t CamNum=0) : wxFrame((wxFrame *)NULL, -1,  wxT("Hello wxDC"), wxPoint(50,50), wxSize(640, 480)), Camera1394 (CamNum)
    {
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        drawPane = new BasicDrawPane( this );
        sizer->Add(drawPane, 1, wxEXPAND);
        SetSizer(sizer);
        
        timer = new RenderTimer(drawPane);
        Show();
    }
    ~MyFrame()
    {
        delete timer;
    }
    void onClose(wxCloseEvent& evt)
    {
        timer->Stop();
		  Finish_Capture();
		  cleanup_all();	
        evt.Skip();
    }
	 void StartTimer()
	 {
        timer->start();
	 }
	 void Init_drawPane()
	 {
		drawPane->Init();
	 }
	 void SetRenderInterval(uint32_t ri)
	 {
			timer->SetRenderInterval(ri);
	 }
    DECLARE_EVENT_TABLE()
};
 
 
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_CLOSE(MyFrame::onClose)
END_EVENT_TABLE()
 
bool MyApp::OnInit()
{
    frame1 = new MyFrame(0);
    frame2 = new MyFrame(1);
	 frame1->Print_Cameras_List();
    frame1->Print_Camera_Info();
    frame2->Print_Camera_Info();
    frame1->Setup (10, DC1394_VIDEO_MODE_640x480_MONO8, DC1394_ISO_SPEED_800, DC1394_FRAMERATE_30, 10, RGB8);
    frame2->Setup (10, DC1394_VIDEO_MODE_640x480_MONO8, DC1394_ISO_SPEED_800, DC1394_FRAMERATE_30, 10, RGB8);
    frame1->Set_Filename_Base("Camera_Zero");
    frame2->Set_Filename_Base("Camera_One");
    frame1->Set_MaxFileCounter(10);
    frame2->Set_MaxFileCounter(10);
 	 frame1->Set_FileSaveType(FST_NONE);
 	 frame2->Set_FileSaveType(FST_NONE);
    frame1->Set_CaptureInterval (0.5);
    frame2->Set_CaptureInterval (0.5);
	 frame1->Init_drawPane();
	 frame2->Init_drawPane();
    frame1->Start_Capture ();
    frame2->Start_Capture ();
	 frame1->StartTimer ();
	 frame2->StartTimer ();
    frame1->Show ();
    frame2->Show ();
 
    return true;
} 
 
 
BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
EVT_PAINT(BasicDrawPane::paintEvent)
END_EVENT_TABLE()
 
 
 
BasicDrawPane::BasicDrawPane(MyFrame* parent) :
wxPanel(parent)
{
	Frame = parent;
	image_data = 0;
}

void BasicDrawPane::Init ()
{
	float fr;


	fr = Frame->Get_Video_FrameRate();
	Frame->SetRenderInterval(1000 / (uint32_t)fr);
		
	Frame->Get_Frame_Size (Rows, Cols);
	Size = 3 * Rows * Cols;
	image_data = (uint8_t *)malloc( Size * sizeof (uint8_t));
	I.Create(Cols, Rows, true);
	I.SetData (image_data);

}
 
 
void BasicDrawPane::paintEvent(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    render(dc);
}
 
void BasicDrawPane::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}
 
void BasicDrawPane::render( wxDC& dc )
{
	uint32_t idx;
	uFrame *uF;

   idx = Frame->Get_Last_Captured_Frame_Index();

	uF = Frame->Get_uFrame_Pointer (idx);

	if (uF->Size)
	{
		memcpy (image_data, uF->image, uF->Size);
		
		wxBitmap bm (I, -1);
	 	dc.Clear();
	 	dc.DrawBitmap (bm, 0, 0, false);
	}

	Frame->Release_uFrame_Pointer(idx);
}
