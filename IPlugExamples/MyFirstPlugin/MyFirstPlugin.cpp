#include "MyFirstPlugin.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kThreshold = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kThresholdX = 100,
  kThresholdY = 100,
  kKnobFrames = 60
};

MyFirstPlugin::MyFirstPlugin(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mThreshold(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kThreshold)->InitDouble("Threshold", 100.0, 0.01, 100.0, 0.01, "%");
  GetParam(kThreshold)->SetShape(1.0);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_BLUE);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

MyFirstPlugin::~MyFirstPlugin() {}

void MyFirstPlugin::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  int const channelCount = 2;

  for (int i = 0; i < channelCount; i++) {
	  double* input = inputs[i];
	  double* output = outputs[i];

	  for (int s = 0; s < nFrames; ++s, ++input, ++output) {
		  if (*input >= 0) {
			  // Make sure positive values can't go above the threshold:
			  *output = fmin(*input, mThreshold);
		  }
		  else {
			  // Make sure negative values can't go below the threshold:
			  *output = fmax(*input, -mThreshold);
		  }
	  }
  }
}

void MyFirstPlugin::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void MyFirstPlugin::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kThreshold:
      mThreshold = GetParam(kThreshold)->Value() / 100.;
      break;

    default:
      break;
  }
}
