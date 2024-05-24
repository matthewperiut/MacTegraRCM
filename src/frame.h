#pragma once

#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/choice.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <CoreFoundation/CoreFoundation.h>
#include <map>

class TegraRCMFrame : public wxFrame {
public:
    TegraRCMFrame();
private:
    const uint16_t DEFAULT_VID = 0x0955;
    const uint16_t DEFAULT_PID = 0x7321;

    wxPanel* panel;
    wxFilePickerCtrl* filePicker;
    wxStaticText* usbStatusText;
    wxChoice* usbChoice;
    std::map<std::string, std::pair<uint16_t, uint16_t>> usbDeviceMap;

    void CheckForSwitch();
    void OnRefresh(wxCommandEvent& event);
    void OnInjectPayload(wxCommandEvent& event);
};