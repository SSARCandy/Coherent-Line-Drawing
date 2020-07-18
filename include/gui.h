#ifndef GUI_H_
#define GUI_H_

#include "CLD.h"
#include "postProcessing.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/filedlg.h>
#include <wx/wfstream.h>

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class DrawPane : public wxPanel
{

public:
    DrawPane(wxPanel *, cv::Size);

    void render();
    void set_mode(std::string);
    cv::Mat &image();
    CLD &cld();
    DECLARE_EVENT_TABLE()

private:
    std::string mode_;
    CLD cld_;
    cv::Mat dis_;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    DrawPane *drawPane;
    wxPanel *dp; // drawpane Container
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
    void addlog(wxString info, const wxColour &color);

private:
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

    void SetRenderingState(bool on);

    void OnStart(wxCommandEvent &event);
    void OnClean(wxCommandEvent &event);
    void OnSolveIt(wxCommandEvent &event);
    void OnRefineETF(wxCommandEvent &event);
    void OnIterativeFDoG(wxCommandEvent &event);
    void OnProcessingBox(wxCommandEvent &event);
    void OnSliderRho(wxCommandEvent &event);
    void OnSliderETFkernel(wxCommandEvent &event);
    void OnSliderSigmaM(wxCommandEvent &event);
    void OnSliderSigmaC(wxCommandEvent &event);
    void OnSliderTau(wxCommandEvent &event);
    void OnOpenSrc(wxCommandEvent &event);
    void OnSaveResult(wxCommandEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnToggleLog(wxCommandEvent &event);
    void OnIdle(wxIdleEvent &evt);
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

#endif // GUI_H_
