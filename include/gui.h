#include "CLD.h"
#include "postProcessing.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filedlg.h>
#include <wx/wfstream.h>

class MyApp : public wxApp {
public:
	virtual bool OnInit();
};

class BasicDrawPane : public wxPanel {

public:
	BasicDrawPane(wxPanel* parent, cv::Size, bool canUndo);

	CLD cld;
	PP processing;
	cv::Mat dis;
	cv::Mat temp;
	string processingS;
	void paintEvent(wxPaintEvent& evt);
	void paintNow(bool);
	void render(wxDC& dc, bool);
	DECLARE_EVENT_TABLE()
private:
	bool activateDraw;
};

class MyFrame : public wxFrame {
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	BasicDrawPane *drawPane;
	wxPanel* dp;// drawpane Container
	wxSlider *slider_rho;
	wxSlider *slider_ETFkernel;
	wxSlider *slider_sigma1;
	wxSlider *slider_sigma2;
	wxSlider *slider_t;
	wxStaticText *slider_rho_t;
	wxStaticText *slider_ETFkernel_t;
	wxStaticText *slider_sigma1_t;
	wxStaticText *slider_sigma2_t;
	wxStaticText *slider_t_t;
	void addlog(wxString info, const wxColour& color);
	void activateRenderLoop(bool on);

protected:
	bool render_loop_on;
	int ETF_kernel;
	int ETF_iteration;
	int FDoG_iteration;
	wxTextCtrl *log; // Show the log
	wxButton *start;
	wxButton *clean;
	wxButton *solve;
	wxButton *refineETF;
	wxButton *iterativeFDoG;
	wxComboBox *processingBox;

	void OnStart(wxCommandEvent& event);
	void OnClean(wxCommandEvent& event);
	void OnSolveIt(wxCommandEvent& event);
	void OnRefineETF(wxCommandEvent& event);
	void OnIterativeFDoG(wxCommandEvent& event);

	void OnProcessingBox(wxCommandEvent& event);


	void OnSliderRho(wxCommandEvent& event);
	void OnSliderETFkernel(wxCommandEvent& event);
	void OnSliderSigmaM(wxCommandEvent& event);
	void OnSliderSigmaC(wxCommandEvent& event);
	void OnSliderTau(wxCommandEvent& event);

	void OnOpenSrc(wxCommandEvent& event);
	void OnSaveResult(wxCommandEvent& event);

	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnToggleLog(wxCommandEvent& event);
	void onIdle(wxIdleEvent& evt);
	wxDECLARE_EVENT_TABLE();
};

enum {
	// Menu > File
	ID_ONOPENSRC = 1,
	ID_ONSAVE,

	// Menu > Help
	wxID_TOGGLE_LOG,

	// Toolbar
	BUTTON_Start,
	BUTTON_Clean,
	COMBOBOX_Processing,

	// Log
	ID_WXEDIT1,

	// Control Panel > Button
	BUTTON_SolveIt,
	BUTTON_RefineETF,
	BUTTON_IterativeFDoG,

	// Control Panel > Slider
	SLIDER_TAU,
	SLIDER_RHO,
	SLIDER_SIGMA_M,
	SLIDER_SIGMA_C,
	SLIDER_ETF_KERNEL,

	// Control Panel > Slider Text
	SLIDER_TAU_T,
	SLIDER_RHO_T,
	SLIDER_SIGMA_M_T,
	SLIDER_SIGMA_C_T,
	SLIDER_ETF_KERNEL_T,
};
