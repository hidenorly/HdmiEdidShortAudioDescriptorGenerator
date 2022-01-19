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

#ifndef __HDMI_EDID_SAD_CONTAINER_HPP__
#define __HDMI_EDID_SAD_CONTAINER_HPP__

#include "AudioFormat.hpp"
#include "Buffer.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>

class HdmiAudioCapability
{
protected:
  ByteBuffer mSadBytes;
  AudioFormat::ENCODING mEncoding;
  int mNumOfChannels;
  int mMaxSamplingRate;
  std::map<std::string, std::string> mAdditionalCapabilities;

protected:
  void ensureSadBytes(AudioFormat::ENCODING encoding, int numOfChannels, int maxSamplingRate);
  virtual void ensureCodecSpecific(void);

public:
  HdmiAudioCapability();
  HdmiAudioCapability(AudioFormat::ENCODING encoding, int numOfChannels = 8, int maxSamplingRate = 0);
  virtual ~HdmiAudioCapability();

  std::vector<AudioFormat> getSupportedAudioFormats();

  AudioFormat::ENCODING getEncoding();
  std::vector<AudioFormat::CHANNEL> getChannels(void);
  std::vector<int> getSamplingRates(void);

  std::map<std::string, std::string> getAdditionalCapabilities(void);

  ByteBuffer getRawSadBytes(void);
};

class HdmiEdidSADContainer
{
protected:
  std::map<AudioFormat::ENCODING, std::shared_ptr<HdmiAudioCapability>> mHdmiAudioCapability;

public:
  HdmiEdidSADContainer();
  virtual ~HdmiEdidSADContainer();

  bool removeHdmiAudioCapability(std::shared_ptr<HdmiAudioCapability> pHdmiAudioCapability);
  bool removeHdmiAudioCapability(AudioFormat::ENCODING afwEncoding);
  void addHdmiAudioCapability(std::shared_ptr<HdmiAudioCapability> pHdmiAudioCapability);
  ByteBuffer getRawSadBytes(void);
};

#endif /* __HDMI_EDID_SAD_CONTAINER_HPP__ */
