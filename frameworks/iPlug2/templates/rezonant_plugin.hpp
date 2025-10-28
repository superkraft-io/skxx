#pragma once

#include "./rezonant/skxx/core/sk_common.hpp"
#include "./rezonant/iPlug2_SK/Examples/IPlugWebUI_SK/IPlugWebUI_SK.h"

using namespace iplug;


const int presetCount = 3;

enum EParams {
    kGain = 0,
    kBoolean,
    kInteger,
    kDouble,
    kList,
    kFrequency,
    kPercent,
    kMilliseconds,
    kNumParams
};


class Rezonant_Plugin : public <plugin_class_name> {
public:
    //VU is in percent
    float vuLevels[2]{ 0, 0 };

    Rezonant_Plugin(const InstanceInfo& info) : <plugin_class_name>(info , kNumParams, presetCount) {
        //Configure parameters
        GetParam(kGain)->InitGain("Gain", -70., -70, 0.);
        GetParam(kBoolean)->InitBool("Boolean", false);
        GetParam(kInteger)->InitInt("Integer", 5, 1, 9);
        GetParam(kDouble)->InitDouble("Double", 50.0, 0.0, 100.0, 0.1);

        GetParam(kList)->InitEnum("List", 0, 3, "", IParam::kFlagsNone, "", "Option 1", "Option 2", "Option 3");

        GetParam(kFrequency)->InitFrequency("Frequency", 0.0, 20.0, 22000.0);

        GetParam(kPercent)->InitPercentage("Percent", 50.0);

        GetParam(kMilliseconds)->InitMilliseconds("Milliseconds Time", 500.0, 1.0, 2000.0);


        //Create presets
        MakePreset("One", -70.);
        MakePreset("Two", -30.);
        MakePreset("Three", 0.);


        //--------------------------//
        onPluginInitialized = [&]() {
            //This callback is called when SK++ is fully initialized.
            //You can now access all core SK++ features freely.
            
            
            //Lets hook into the SK++ synced timer so that we can update our visuals in sync with the monitor
            skg->displaySyncedTimer->on([&]() {
                //We send any relevant UI data from here, such as VU meter values, spectrogram data, etc...

                //First we handle the data in a float array
                vuLevels[0] += .5;
                vuLevels[1] += .25;

                if (vuLevels[0] > 100) vuLevels[0] = 0;
                if (vuLevels[1] > 100) vuLevels[1] = 0;

                //Then we send the data to the appropriate view, in this case "first_view"
                SK_Window_Root* wnd = skg->findWindowByTag("first_view");
                if (!wnd) return;

                if (!wnd->webview.isReady) return;

                //Finally we send the data to the view
                wnd->webview.sendSharedBuffer(sizeof(float) * 2, &vuLevels, { {"id", "vuData"} });
            });
        };
    }

    void ProcessBlock(sample** inputs, sample** outputs, int nFrames) {
        //process audio buffer here
        //reading buffer: inputs[channel number][sample position]
        //writing buffer: inputs[outputs number][sample position]
    }

    void OnReset() {
        auto sr = GetSampleRate();
    }

    void OnParamChange(int paramIdx) {
        //Do something when parameter changes
    }

    void ProcessMidiMsg(const IMidiMsg& msg) {
        TRACE;

        msg.PrintMsg();
        SendMidiMsg(msg);
    }
};
