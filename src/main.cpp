#include "gui.h"


BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)

END_EVENT_TABLE()


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
	// Menu - File
    EVT_MENU(ID_ONOPENSRC, MyFrame::OnOpenSrc)
	EVT_MENU(ID_ONSAVE, MyFrame::OnSaveResult)
	EVT_MENU(wxID_EXIT, MyFrame::OnExit)

	// Menu - Help
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
	EVT_MENU(wxID_TOGGLE_LOG, MyFrame::OnToggleLog)

	// Toolbar
	EVT_BUTTON(BUTTON_Start, MyFrame::OnStart)
	EVT_BUTTON(BUTTON_Clean, MyFrame::OnClean)
	EVT_COMBOBOX(COMBOBOX_Processing, MyFrame::OnProcessingBox)

	// Control Panel
	EVT_BUTTON(BUTTON_RefineETF, MyFrame::OnRefineETF)
	EVT_BUTTON(BUTTON_SolveIt, MyFrame::OnSolveIt)
	EVT_BUTTON(BUTTON_IterativeFDoG, MyFrame::OnIterativeFDoG)
	EVT_SLIDER(SLIDER_RHO, MyFrame::OnSliderRho)
	EVT_SLIDER(SLIDER_ETF_KERNEL, MyFrame::OnSliderETFkernel)
	EVT_SLIDER(SLIDER_SIGMA_M, MyFrame::OnSliderSigmaM)
	EVT_SLIDER(SLIDER_SIGMA_C, MyFrame::OnSliderSigmaC)
	EVT_SLIDER(SLIDER_TAU, MyFrame::OnSliderTau)


wxEND_EVENT_TABLE()
wxIMPLEMENT_APP(MyApp);
