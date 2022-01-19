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

#include "HdmiEdidDolbyAudioCapability.hpp"

void HdmiDolbyAudioCapability::ensureCodecSpecific(void)
{
  // ensure atmos capability
  switch( mEncoding ){
    case AudioFormat::ENCODING::COMPRESSED_E_AC3:
      // Thanks the info. https://www.mail-archive.com/linux-media@vger.kernel.org/msg148721.html
      mAdditionalCapabilities.insert_or_assign("JOC", mbAtmosEnabled ? "true" : "false" );
      mAdditionalCapabilities.insert_or_assign("JOC_DolbyAtmos", mbAtmosEnabled ? "true" : "false" );
      break;
    default:
      break;
  }
}

HdmiDolbyAudioCapability::HdmiDolbyAudioCapability(AudioFormat::ENCODING encoding, int numOfChannels, bool bAtmosEnabled) : HdmiAudioCapability(encoding, numOfChannels, 48000), mbAtmosEnabled(bAtmosEnabled)
{

}

HdmiDolbyAudioCapability::~HdmiDolbyAudioCapability()
{

}
