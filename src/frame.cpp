#include "frame.h"

#include <iostream>
#include <fstream>
#include <libusb.h>

#include "fusee.h"
#include "resources.h"

TegraRCMFrame::TegraRCMFrame() : wxFrame(nullptr, wxID_ANY, "MacOS Tegra RCM", wxDefaultPosition, wxSize(300, 166))
{
    panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    // File picker
    filePicker = new wxFilePickerCtrl(panel, wxID_ANY, wxEmptyString, "Select a file", "*.*",
                                      wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE);
    filePicker->GetTextCtrl()->SetHint("Select .bin Payload");

    std::ifstream prevBinPath(GetResourcePath() + "previousBINPath.txt");
    if (prevBinPath.good()) {
        std::string line;
        getline(prevBinPath, line);
        filePicker->SetFileName(wxFileName(line));
    }

    vbox->Add(filePicker, 0, wxEXPAND | wxALL, 10);

    // USB device selector and refresh button
    wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);

    // USB device status text
    usbStatusText = new wxStaticText(panel, wxID_ANY, "Checking for Switch...");
    hbox->Add(usbStatusText, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);

    wxButton* refreshButton = new wxButton(panel, wxID_ANY, "↻", wxDefaultPosition, wxSize(30, 30));
    hbox->Add(refreshButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    refreshButton->Bind(wxEVT_BUTTON, &TegraRCMFrame::OnRefresh, this);

    vbox->Add(hbox, 0, wxALIGN_CENTER | wxALL, 10);

    // Inject Payload button
    wxButton* injectButton = new wxButton(panel, wxID_ANY, "Inject Payload");
    vbox->Add(injectButton, 0, wxALIGN_CENTER | wxALL, 10);

    injectButton->Bind(wxEVT_BUTTON, &TegraRCMFrame::OnInjectPayload, this);

    panel->SetSizer(vbox);

    CheckForSwitch();  // Initial population of USB devices

}

void TegraRCMFrame::CheckForSwitch()
{
    libusb_context *ctx = nullptr;
    libusb_device **devs = nullptr;
    ssize_t cnt;

    int r = libusb_init(&ctx);
    if (r < 0) {
        usbStatusText->SetLabel("Failed to initialize libusb");
        usbStatusText->SetForegroundColour(*wxRED);
        return;
    }

    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        usbStatusText->SetLabel("Failed to get USB device list");
        usbStatusText->SetForegroundColour(*wxRED);
        libusb_exit(ctx);
        return;
    }

    bool switchFound = false;
    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device *dev = devs[i];
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(dev, &desc);

        if (desc.idVendor == DEFAULT_VID && desc.idProduct == DEFAULT_PID) {
            switchFound = true;
            break;
        }
    }

    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);

    if (switchFound) {
        usbStatusText->SetLabel("Switch Found");
        usbStatusText->SetForegroundColour(*wxGREEN);
    } else {
        usbStatusText->SetLabel("Switch Not Found");
        usbStatusText->SetForegroundColour(*wxRED);
    }
    panel->Layout();
}


void TegraRCMFrame::OnRefresh(wxCommandEvent& event)
{
    CheckForSwitch();
}

void TegraRCMFrame::OnInjectPayload(wxCommandEvent& event) {
    wxString filePath = filePicker->GetPath();

    if (filePath.IsEmpty()) {
        wxMessageBox("Please select a payload", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string filepath = filePath.ToStdString();
    std::string message;
    if (fuseec(filePath.char_str(), message) == 0) {
        wxLogMessage("Successfully Smashed the Stack!");
        std::ofstream binPath;
        binPath.open (GetResourcePath() + "previousBINPath.txt");
        binPath << filePath.ToStdString();
        binPath.close();
    }
    else {
        wxLogMessage(message.c_str());
    }

}