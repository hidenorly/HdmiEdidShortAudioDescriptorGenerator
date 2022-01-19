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

#include "HdmiEdidSadContainer.hpp"
#include "HdmiEdidSadGenerator.hpp"

void HdmiAudioCapability::ensureSadBytes(AudioFormat::ENCODING encoding, int numOfChannels, int maxSamplingRate)
{
  std::map<std::string, std::string> nullCaps;
  mSadBytes = HdmiEdidSadGenerator::getSadFromAudioFormat( AudioFormat( mEncoding, maxSamplingRate, AudioFormat::getAudioChannel( mNumOfChannels ) ), nullCaps );
  ensureCodecSpecific();
}

void HdmiAudioCapability::ensureCodecSpecific(void)
{

}

HdmiAudioCapability::HdmiAudioCapability() : mEncoding(AudioFormat::ENCODING::PCM_16BIT), mNumOfChannels(2), mMaxSamplingRate(48000)
{
  ensureSadBytes( mEncoding, mNumOfChannels, mMaxSamplingRate );
}

HdmiAudioCapability::HdmiAudioCapability(AudioFormat::ENCODING encoding, int numOfChannels, int maxSamplingRate) : mEncoding(encoding), mNumOfChannels(numOfChannels), mMaxSamplingRate(maxSamplingRate)
{
  ensureSadBytes( mEncoding, mNumOfChannels, mMaxSamplingRate );
}

HdmiAudioCapability::~HdmiAudioCapability()
{

}

std::vector<AudioFormat> HdmiAudioCapability::getSupportedAudioFormats()
{
  std::vector<AudioFormat> result;

  std::vector<AudioFormat::CHANNEL> channels = getChannels();  
  std::vector<int> samplingRates = getSamplingRates();

  for( auto& aChannel : channels ){
    for( auto& aSamplingRate : samplingRates ){
      result.push_back( AudioFormat( mEncoding, aSamplingRate, aChannel ) );
    }
  }

  return result;
}

AudioFormat::ENCODING HdmiAudioCapability::getEncoding()
{
  return mEncoding;
}

std::vector<AudioFormat::CHANNEL> HdmiAudioCapability::getChannels(void)
{
  std::vector<AudioFormat::CHANNEL> result;

  for(int i=0; i<mNumOfChannels; i++){
    result.push_back( AudioFormat::getAudioChannel( i ) );
  }

  return result;
}

std::vector<int> HdmiAudioCapability::getSamplingRates(void)
{
  std::vector<int> result;

  const static int samplingRates[]={
    AudioFormat::SAMPLING_RATE::SAMPLING_RATE_8_KHZ,
    AudioFormat::SAMPLING_RATE::SAMPLING_RATE_16_KHZ,
    AudioFormat::SAMPLING_RATE::SAMPLING_RATE_44_1_KHZ,
    AudioFormat::SAMPLING_RATE::SAMPLING_RATE_48_KHZ,
    AudioFormat::SAMPLING_RATE::SAMPLING_RATE_88_2_KHZ,
    AudioFormat::SAMPLING_RATE::SAMPLING_RATE_96_KHZ,
    AudioFormat::SAMPLING_RATE::SAMPLING_RATE_192_KHZ,
    0
  };
  for( int i=0; samplingRates[i]!=0 || samplingRates[i]<=mMaxSamplingRate; i++ ){
    result.push_back( (int)samplingRates[i] );
  }

  return result;
}

std::map<std::string, std::string> HdmiAudioCapability::getAdditionalCapabilities(void)
{
  return mAdditionalCapabilities;
}

ByteBuffer HdmiAudioCapability::getRawSadBytes(void)
{
  return mSadBytes;
}



HdmiEdidSADContainer::HdmiEdidSADContainer()
{

}

HdmiEdidSADContainer::~HdmiEdidSADContainer()
{

}

bool HdmiEdidSADContainer::removeHdmiAudioCapability(std::shared_ptr<HdmiAudioCapability> pHdmiAudioCapability)
{
  bool result = false;
  if( pHdmiAudioCapability ){
    AudioFormat::ENCODING encoding = pHdmiAudioCapability->getEncoding();
    if( mHdmiAudioCapability.contains( encoding ) ){
      mHdmiAudioCapability.erase( encoding );
      result = true;
    }
  }
  return result;
}

bool HdmiEdidSADContainer::removeHdmiAudioCapability(AudioFormat::ENCODING afwEncoding)
{
  bool result = false;
  if( mHdmiAudioCapability.contains( afwEncoding ) ){
    mHdmiAudioCapability.erase( afwEncoding );
    result = true;
  }
  return result;
}

void HdmiEdidSADContainer::addHdmiAudioCapability(std::shared_ptr<HdmiAudioCapability> pHdmiAudioCapability)
{
  if( pHdmiAudioCapability ){
    mHdmiAudioCapability.insert_or_assign( pHdmiAudioCapability->getEncoding(), pHdmiAudioCapability );
  }
}

ByteBuffer HdmiEdidSADContainer::getRawSadBytes(void)
{
  ByteBuffer result;
  for( auto& [afwEncoding, pHdmiAudioCapability] : mHdmiAudioCapability ){
    ByteBuffer aSadBytes = pHdmiAudioCapability->getRawSadBytes();
    result.insert( result.end(), aSadBytes.begin(), aSadBytes.end() );
  }
  return result;
}

