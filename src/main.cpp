#include "gui.h"


BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)

END_EVENT_TABLE()



wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
	// Menu - File
    EVT_MENU(ID_ONOPENSRC, MyFrame::OnOpenSrc)
    EVT_MENU(ID_ONOPENVFB, MyFrame::OnOpenVfb)
	EVT_MENU(ID_ONOPENETF, MyFrame::OnOpenETF)
	EVT_MENU(ID_ONSAVE, MyFrame::OnSaveResult)
	EVT_MENU(wxID_EXIT, MyFrame::OnExit)

	// Menu - Tool
	EVT_MENU(ID_ONETF2GVF, MyFrame::OnGenGVF)

	// Menu - Help
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
	EVT_MENU(wxID_TOGGLE_LOG, MyFrame::OnToggleLog)

	// Toolbar
	EVT_BUTTON(BUTTON_Start, MyFrame::OnStart)
	EVT_BUTTON(BUTTON_SolveIt, MyFrame::OnSolveIt)
	EVT_BUTTON(BUTTON_Clean, MyFrame::OnClean)
	EVT_COMBOBOX(COMBOBOX_Processing, MyFrame::OnProcessingBox)

	// Control Panel
	//EVT_SLIDER(SLIDER_BRUSH_SIZE, MyFrame::OnSliderBrushSize)
	//EVT_SLIDER(SLIDER_AddA, MyFrame::OnSliderAddA)
	//EVT_SLIDER(SLIDER_AddB, MyFrame::OnSliderAddB)

	EVT_SLIDER(SLIDER_S, MyFrame::OnSliderS)
	EVT_SLIDER(SLIDER_sd, MyFrame::OnSliderSd)
	EVT_SLIDER(SLIDER_F, MyFrame::OnSliderF)
	EVT_SLIDER(SLIDER_K, MyFrame::OnSliderK)
	EVT_SLIDER(SLIDER_L, MyFrame::OnSliderL)
wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
