/*
  Copyright (C) 2022 hidenorly

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __HDMI_EDID_DOLBY_AUDIO_CAPABILITY_HPP__
#define __HDMI_EDID_DOLBY_AUDIO_CAPABILITY_HPP__

#include "HdmiEdidSadContainer.hpp"

class HdmiDolbyAudioCapability : public HdmiAudioCapability
{
protected:
  bool mbAtmosEnabled;
  virtual void ensureCodecSpecific(void);

public:
  HdmiDolbyAudioCapability(AudioFormat::ENCODING encoding = AudioFormat::ENCODING::COMPRESSED_E_AC3, int numOfChannels = 8, bool bAtmosEnabled = false);
  virtual ~HdmiDolbyAudioCapability();
};

#endif /* __HDMI_EDID_DOLBY_AUDIO_CAPABILITY_HPP__ */
