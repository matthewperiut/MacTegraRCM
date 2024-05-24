#include <wx/wx.h>
#include "frame.h"

class MyApp : public wxApp
{
public:
    virtual bool OnInit()
    {
        TegraRCMFrame* frame = new TegraRCMFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
