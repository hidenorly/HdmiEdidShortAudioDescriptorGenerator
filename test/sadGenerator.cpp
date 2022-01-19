/*
  Copyright (C) 2021 hidenorly

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
#include "OptParse.hpp"
#include "StringTokenizer.hpp"
#include <iostream>
#include <sstream>
#include <vector>

void dump(std::map<std::string, std::string> keyVals)
{
  for( auto& [aKey, aValue] : keyVals ){
    std::cout << aKey << " = " << aValue << std::endl;
  }
}

int main(int argc, char** argv)
{
  std::vector<OptParse::OptParseItem> options;
  options.push_back( OptParse::OptParseItem("-e", "--encode", true, "PCM_16BIT", "Specify Encode"));
  options.push_back( OptParse::OptParseItem("-c", "--channel", true, "2", "Specify number of channels"));
  options.push_back( OptParse::OptParseItem("-s", "--samplingrate", true, "48000", "Specify samplingRate"));
  options.push_back( OptParse::OptParseItem("-p", "--params", true, "", "Specify codec specific parameters e.g.JOC=true,JOC_DolbyAtmos=true"));

  OptParse optParser( argc, argv, options, std::string("Hdmi Edid Short Audio Descriptor (SAD) generator e.g. -e PCM_16BIT -c 2 -s 48000") );

  AudioFormat::ENCODING encoding = AudioFormat::getEncodingFromString( optParser.values["-e"] );
  int numOfChannels = std::stoi( optParser.values["-c"] );
  int samplingRate = std::stoi( optParser.values["-s"] );

  std::map<std::string, std::string> params;

  if( !optParser.values["-p"].empty() ){
    StringTokenizer token(optParser.values["-p"], ",");
    while( token.hasNext() ){
      StringTokenizer token2( token.getNext(), "=");
      if( token2.hasNext() ){
        params.insert_or_assign( token2.getNext(), token2.getNext() );
      }
    }
  }
  std::cout << "encoding:" << AudioFormat::getEncodingString( encoding ) << std::endl;
  std::cout << "channels:" << std::to_string( numOfChannels ) << std::endl;
  std::cout << "samplingRate:" << std::to_string( samplingRate ) << std::endl;
  dump( params );

  ByteBuffer sadBytes = HdmiEdidSadGenerator::getSadFromAudioFormat( AudioFormat(encoding, samplingRate, AudioFormat::getAudioChannel(numOfChannels) ), params );

  std::cout << std::hex;
  for( auto& aData : sadBytes ){
      std::cout << (int)aData << ",";
  }
  std::cout << std::dec << std::endl;

  return 0;
}