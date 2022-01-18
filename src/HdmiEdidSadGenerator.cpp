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

#include "HdmiEdidSadGenerator.hpp"
#include "HdmiEdidPrimitive.hpp"

struct SadEncodingConversionTable
{
public:
  int sadEncoding;
  AudioFormat::ENCODING afwEncoding;

  SadEncodingConversionTable(int sadEncoding, AudioFormat::ENCODING afwEncoding) : sadEncoding(sadEncoding), afwEncoding(afwEncoding){};
};


int HdmiEdidSadGenerator::getSadEncodingFromAudioFormat(AudioFormat::ENCODING afwEncoding)
{
  int result = 0;

  if( afwEncoding < AudioFormat::PCM_UNKNOWN ){
    result = 1;
    if( afwEncoding == AudioFormat::ENCODING::PCM_16BIT ){
      result = result | (HDMI_EDID_SAD_ENCODING_LPCM16 << 16);
    } else if( afwEncoding == AudioFormat::ENCODING::PCM_24BIT_PACKED ){
      result = result | (HDMI_EDID_SAD_ENCODING_LPCM24 << 16);
    }
  } else {
    const static SadEncodingConversionTable conversionTable[]=
    {
      SadEncodingConversionTable( 1, AudioFormat::ENCODING::PCM_UNKNOWN),
      SadEncodingConversionTable( 2, AudioFormat::ENCODING::COMPRESSED_AC3),
      SadEncodingConversionTable( 3, AudioFormat::ENCODING::COMPRESSED_MP2),
      SadEncodingConversionTable( 4, AudioFormat::ENCODING::COMPRESSED_MP3),
      SadEncodingConversionTable( 5, AudioFormat::ENCODING::COMPRESSED_MP2),
      SadEncodingConversionTable( 6, AudioFormat::ENCODING::COMPRESSED_AAC),
      SadEncodingConversionTable( 7, AudioFormat::ENCODING::COMPRESSED_DTS),
      SadEncodingConversionTable( 8, AudioFormat::ENCODING::COMPRESSED_ATRAC),
      SadEncodingConversionTable( 9, AudioFormat::ENCODING::PDM_SACD),
      SadEncodingConversionTable(10, AudioFormat::ENCODING::COMPRESSED_E_AC3),
      SadEncodingConversionTable(11, AudioFormat::ENCODING::COMPRESSED_DTS_HD),
      SadEncodingConversionTable(12, AudioFormat::ENCODING::COMPRESSED_DOLBY_TRUEHD),
      SadEncodingConversionTable(13, AudioFormat::ENCODING::COMPRESSED_UNKNOWN),
      SadEncodingConversionTable(14, AudioFormat::ENCODING::COMPRESSED_WMA_PRO),
      SadEncodingConversionTable(15 | ( 4 << HDMI_EDID_SAD_ENCODING_EXTENDED_SHIFT << 8), AudioFormat::ENCODING::COMPRESSED_HE_AAC_V1),
      SadEncodingConversionTable(15 | ( 5 << HDMI_EDID_SAD_ENCODING_EXTENDED_SHIFT << 8), AudioFormat::ENCODING::COMPRESSED_HE_AAC_V2),
      SadEncodingConversionTable(15 | ( 6 << HDMI_EDID_SAD_ENCODING_EXTENDED_SHIFT << 8), AudioFormat::ENCODING::COMPRESSED_AAC_LC),
      SadEncodingConversionTable(15 | ( 7 << HDMI_EDID_SAD_ENCODING_EXTENDED_SHIFT << 8), AudioFormat::ENCODING::COMPRESSED_DRA),
      SadEncodingConversionTable(15 | (12 << HDMI_EDID_SAD_ENCODING_EXTENDED_SHIFT << 8), AudioFormat::ENCODING::COMPRESSED_AC4),
      SadEncodingConversionTable( 0, AudioFormat::ENCODING::COMPRESSED_UNKNOWN)
    };

    for(int i=0; (conversionTable[i].sadEncoding!=0); i++){
      if( conversionTable[i].afwEncoding == afwEncoding ){
        result = conversionTable[i].afwEncoding;
        break;
      }
    }
  }

  return result;
}

int HdmiEdidSadGenerator::getSadChannelFromAudioFormat(AudioFormat::CHANNEL channel)
{
  return AudioFormat::getNumberOfChannels( channel ) - 1;
}

int HdmiEdidSadGenerator::getSadSamplingRateFromAudioFormat(int samplingRate)
{
  int result = 0;

  switch( samplingRate){
    case 32000:
      result = HDMI_EDID_SAD_SAMPLING_RATE_32K << 8;
      break;
    case 44100:
      result = HDMI_EDID_SAD_SAMPLING_RATE_44K << 8;
      break;
    case 88200:
      result = HDMI_EDID_SAD_SAMPLING_RATE_88K << 8;
      break;
    case 96000:
      result = HDMI_EDID_SAD_SAMPLING_RATE_96K << 8;
      break;
    case 176000:
      result = HDMI_EDID_SAD_SAMPLING_RATE_176K << 8;
      break;
    case 192000:
      result = HDMI_EDID_SAD_SAMPLING_RATE_192K << 8;
      break;
    case 48000:
    default:
      result = HDMI_EDID_SAD_SAMPLING_RATE_48K << 8;
      break;
  }

  return result;
}

ByteBuffer HdmiEdidSadGenerator::getSadFromAudioFormat(AudioFormat audioFormat, std::map<std::string, std::string> additionalCapabilities)
{
  ByteBuffer result;

  int sadBytes = getSadEncodingFromAudioFormat( audioFormat.getEncoding() ) |
              getSadChannelFromAudioFormat( audioFormat.getChannels() ) |
              ( audioFormat.isEncodingPcm() ? getSadSamplingRateFromAudioFormat( audioFormat.getSamplingRate() ) : 0 ) | 
              getSadAdditionalFeature( audioFormat.getEncoding(), additionalCapabilities );

  result.push_back( (uint8_t) sadBytes & 0xFF );  sadBytes = sadBytes >> 8;
  result.push_back( (uint8_t) sadBytes & 0xFF );  sadBytes = sadBytes >> 8;
  result.push_back( (uint8_t) sadBytes & 0xFF );

  return result;
}

int HdmiEdidSadGenerator::getSadAdditionalFeature(AudioFormat::ENCODING afwEncoding, std::map<std::string, std::string> additionalCapabilities)
{
  int result = 0;

  if( !AudioFormat::isEncodingPcm( afwEncoding ) ){
    // not PCM, codec specific
    switch( afwEncoding ){
      case AudioFormat::ENCODING::COMPRESSED_E_AC3:
        // Thanks the info. https://www.mail-archive.com/linux-media@vger.kernel.org/msg148721.html
        if( additionalCapabilities.contains("JOC") ){
          if( additionalCapabilities["JOC"] == "true" ){
            result |= ( HDMI_EDID_SAD_EXT_E_AC3_JOC << 16 );
          }
        }
        if( additionalCapabilities.contains("JOC_DolbyAtmos") ){
          if( additionalCapabilities["JOC_DolbyAtmos"] == "true" ){
            result |= ( HDMI_EDID_SAD_EXT_E_AC3_ACMOD28 << 16 );
          }
        }
        break;
      case AudioFormat::ENCODING::COMPRESSED_AAC_LC:
        if( additionalCapabilities.contains("channel_22.2ch") ){
          if( additionalCapabilities["channel_22.2ch"] == "true" ){
            result |= ( HDMI_EDID_SAD_EXT_AAC_LC_22CH << 16 );
          }
        }
        break;
      default:
        break;
    }
  }

  return result;
}
