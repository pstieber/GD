#include "ErrorCompilation.h"

#ifdef DEBUG
#include "nommgr.h"
#endif

//(*InternalHeaders(ErrorCompilation)








#ifdef DEBUG

#endif

//(*IdInit(ErrorCompilation)














BEGIN_EVENT_TABLE(ErrorCompilation,wxDialog)
	//(*EventTable(ErrorCompilation)
	//*)
END_EVENT_TABLE()

ErrorCompilation::ErrorCompilation(wxWindow* parent, string report)
{
	//(*Initialize(ErrorCompilation)




























































	if (report != "")
	{
		ReportEdit->Clear();
		ReportEdit->AppendText(report);
	}
}

ErrorCompilation::~ErrorCompilation()
{
	//(*Destroy(ErrorCompilation)
	//*)
}


void ErrorCompilation::OnAideBtClick(wxCommandEvent& event)
{
}

void ErrorCompilation::OnFermerBtClick(wxCommandEvent& event)
{
    EndModal(0);
}