#include "CLD.h"
#include "postProcessing.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filedlg.h>
#include <wx/wfstream.h>

class MyApp: public wxApp
{
public:
	virtual bool OnInit();
};

class BasicDrawPane : public wxPanel
{

public:
	BasicDrawPane(wxPanel* parent, Size, bool canUndo);

	CLD cld;
	PP processing;
	Mat dis;
	Mat temp;
	string processingS;
	void paintEvent(wxPaintEvent& evt);
	void paintNow(bool);
	void render( wxDC& dc,bool );
	DECLARE_EVENT_TABLE()
private:
	bool activateDraw;
};

class MyFrame: public wxFrame
{
public:
	MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	BasicDrawPane *drawPane;
	wxPanel* dp;// drawpane Container
	wxSlider *slider_sd;
	wxSlider *slider_s;
	wxSlider *slider_f;
	wxSlider *slider_k;
	wxSlider *slider_l;
	wxStaticText *slider_sd_t;
	wxStaticText *slider_s_t;
	wxStaticText *slider_f_t;
	wxStaticText *slider_k_t;
	wxStaticText *slider_l_t;
	void addlog(wxString info, wxColour& color);
	void activateRenderLoop(bool on);

protected:
	bool render_loop_on;
	wxTextCtrl *log; // Show the log
	wxButton *start;
	wxButton *clean;
	wxComboBox *processingBox;

	void OnStart(wxCommandEvent& event);
	void OnClean(wxCommandEvent& event);

	void OnProcessingBox(wxCommandEvent& event);

	//void OnSliderBrushSize(wxCommandEvent& event);
	//void OnSliderAddA(wxCommandEvent& event);
	//void OnSliderAddB(wxCommandEvent& event);

	void OnSliderS(wxCommandEvent& event);
	void OnSliderSd(wxCommandEvent& event);
	void OnSliderF(wxCommandEvent& event);
	void OnSliderK(wxCommandEvent& event);
	void OnSliderL(wxCommandEvent& event);

	void OnOpenSrc(wxCommandEvent& event);
	void OnOpenVfb(wxCommandEvent& event);
	void OnOpenETF(wxCommandEvent& event);
	void OnSaveResult(wxCommandEvent& event);

	void OnGenGVF(wxCommandEvent& event);
	
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnToggleLog(wxCommandEvent& event);
	void onIdle(wxIdleEvent& evt);
	wxDECLARE_EVENT_TABLE();
};

enum
{
	ID_ONOPENSRC = 1,
	ID_ONOPENVFB,
	ID_ONOPENETF,
	ID_ONOPENTEX,
	ID_ONOPENCONTOLIMG,
	ID_ONOPENSIZEIMG,
	ID_ONSAVE,
	ID_ONSAVERD,
	ID_ONREADRD,
	wxID_TOGGLE_LOG,

	ID_ONEdge2AddA,
	ID_ONEdge2AddB,
	ID_ONMask2AddA,
	ID_ONMask2AddB,
	ID_ONETF2GVF,
	ID_ONCLAHE,
	ID_ONHISTOGRAM,
	ID_ONSIZEMASK,
	ID_ONOPEN_MASK,
	ID_ONOPEN_MASK_S,
	ID_ONOPEN_PATTERN_PICKER,

	ID_WXEDIT1,
	BUTTON_Start,
	BUTTON_Fill,
	BUTTON_Clean,
	BUTTON_UNDO,
	BUTTON_REDO,
	COMBOBOX_Processing,
	COMBOBOX_Controlling,
	BUTTON_subDegree,
	BUTTON_addDegree,

	SLIDER_S_PICKER,
	SLIDER_S,
	SLIDER_S_T,
	SLIDER_sd,
	SLIDER_sd_T,
	COMBOBOX_GRADIENT_S_TYPE,
	COMBOBOX_GRADIENT_k_TYPE,
	SLIDER_F,
	SLIDER_F_T,
	SLIDER_K,
	SLIDER_K_T,
	SLIDER_L,
	SLIDER_L_T,
	SLIDER_Theta0,
	SLIDER_Theta0_T,
	SLIDER_BRUSH_SIZE,
	SLIDER_BRUSH_SIZE_T,
	SLIDER_AddA,
	SLIDER_AddA_T,
	SLIDER_AddB,
	SLIDER_AddB_T,
	CHECKBOX_MODIFY_FUNCTION,
	SLIDER_MINDEGREE,
	SLIDER_MINDEGREE_T,
	SLIDER_MAXDEGREE,
	SLIDER_MAXDEGREE_T,
	CHECKBOX_SEGMENTATION,
	COMBOBOX_Region,
	CHECKBOX_DISPLAY_REGION,

	CHECKBOX_Colormapping_isAda,
	COMBOBOX_ColormappingMode,
	SLIDER_Alpha,
	SLIDER_Alpha_T,
	SLIDER_Beta,
	SLIDER_Beta_T,

	BUTTON_Select //pattern picker
};