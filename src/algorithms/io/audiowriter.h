/*
 * Copyright (C) 2006-2021  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#ifndef ESSENTIA_STREAMING_AUDIOWRITER_H
#define ESSENTIA_STREAMING_AUDIOWRITER_H

#include "streamingalgorithm.h"
#include "audiocontext.h"

namespace essentia {
namespace streaming {

class AudioWriter : public Algorithm {
 protected:
  Sink<StereoSample> _audio;
  bool _configured;
  AudioContext _audioCtx;

 public:
  AudioWriter() : Algorithm(), _configured(false) {
    declareInput(_audio, 4096, "audio", "the input audio");
  }

  void declareParameters() {
    declareParameter("filename", "the name of the encoded file", "", Parameter::STRING);
    declareParameter("format", "the audio output format","{wav,aiff,mp3,ogg,flac}", "wav");
    declareParameter("sampleRate", "the audio sampling rate [Hz]","(0,inf)", 44100.);
    declareParameter("bitrate", "the audio bit rate for compressed formats [kbps]",
                     "{32,40,48,56,64,80,96,112,128,144,160,192,224,256,320}", 192);
  }

  void configure();
  AlgorithmStatus process();
  void reset();

  static const char* name;
  static const char* category;
  static const char* description;

};

} // namespace streaming
} // namespace essentia

#include "network.h"
#include "algorithm.h"
#include "vectorinput.h"

namespace essentia {
namespace standard {

// Standard non-streaming algorithm comes after the streaming one as it
// depends on it
class AudioWriter : public Algorithm {
 protected:
  Input<::essentia::VectorEx<StereoSample> > _audio;

  Real _duration;
  int _nChannels;
  Real _sampleRate;

  bool _configured;

  streaming::Algorithm* _writer;
  streaming::VectorInput<StereoSample, 1024>* _audiogen;
  scheduler::Network* _network;

  void createInnerNetwork();

 public:
  AudioWriter() : _configured(false) {
    declareInput(_audio, "audio", "the audio signal");

    createInnerNetwork();
  }

  ~AudioWriter() {
    delete _network;
  }

  void declareParameters() {
    declareParameter("filename", "the name of the encoded file", "", Parameter::STRING);
    declareParameter("format", "the audio output format","{wav,aiff,mp3,ogg,flac}", "wav");
    declareParameter("sampleRate", "the audio sampling rate [Hz]","(0,inf)", 44100.);
    declareParameter("bitrate", "the audio bit rate for compressed formats [kbps]",
                     "{32,40,48,56,64,80,96,112,128,144,160,192,224,256,320}", 192);
  }

  void configure();

  void compute();

  static const char* name;
  static const char* category;
  static const char* description;
};

} // namespace standard
} // namespace essentia


#endif // ESSENTIA_STREAMING_AUDIOWRITER_H
