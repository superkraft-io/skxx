#pragma once

#include "./skxx_daw_plugin_lib/skxx/core/sk_common.hpp"
#include "./skxx_daw_plugin_lib/iPlug2_SK/Examples/IPlugWebUI_SK/IPlugWebUI_SK.h"

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


class SK_DAW_Plugin : public SK_DAW_Plugin_Project {
public:
    SK_DAW_Plugin(const InstanceInfo& info) : SK_DAW_Plugin_Project(info , kNumParams, presetCount) {
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
