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

#ifndef ESSENTIA_PITCHYINPROBABILITIES_H
#define ESSENTIA_PITCHYINPROBABILITIES_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class PitchYinProbabilities : public Algorithm {

 private:
  Input<::essentia::VectorEx<Real> > _signal;
  Output<::essentia::VectorEx<Real> > _pitch;
  Output<::essentia::VectorEx<Real> > _probabilities;
  Output<Real> _RMS;

  Algorithm* _FFT;
  Algorithm* _IFFT;
  Algorithm* _RMSALGO;

  ::essentia::VectorEx<Real> _yin;
  ::essentia::VectorEx<Real> _peakProb;
  ::essentia::VectorEx<Real> _freq;
  ::essentia::VectorEx<Real> _peakProb_freq;

  int _frameSize;
  Real _sampleRate;
  Real _lowAmp;
  bool _preciseTime;

 public:
  PitchYinProbabilities() {
    declareInput(_signal, "signal", "the input signal frame");
    declareOutput(_pitch, "pitch", "the output pitch candidate frequencies in cents");
    declareOutput(_probabilities, "probabilities", "the output pitch candidate probabilities");
    declareOutput(_RMS, "RMS", "the output RMS value");

    _FFT = AlgorithmFactory::create("FFTC");
    _IFFT = AlgorithmFactory::create("FFTC");
    _RMSALGO = AlgorithmFactory::create("RMS");
  }


  void declareParameters() {
    declareParameter("frameSize", "number of samples in the input frame", "[2,inf)", 2048);
    declareParameter("sampleRate", "sampling rate of the input audio [Hz]", "(0,inf)", 44100.);
    declareParameter("lowAmp", "the low RMS amplitude threshold", "(0,1]", 0.1);  
    declareParameter("preciseTime", "use non-standard precise YIN timing (slow).", "{true,false}", false);
  }

  void configure();
  void compute();

  static const char* name;
  static const char* category;
  static const char* description;

 protected:
  Real parabolicInterpolation(const ::essentia::VectorEx<Real> yinBuffer, const size_t tau, const size_t yinBufferSize);
  void slowDifference(const ::essentia::VectorEx<Real> sig, ::essentia::VectorEx<Real> &yinBuffer);
  void fastDifference(const ::essentia::VectorEx<Real> in, ::essentia::VectorEx<Real> &yinBuffer, const size_t yinBufferSize);
};

} // namespace standard
} // namespace essentia


#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class PitchYinProbabilities : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<Real> > _signal;
  Source<::essentia::VectorEx<Real> > _pitch;
  Source<::essentia::VectorEx<Real> > _probabilities;
  Source<Real> _RMS;

  Algorithm* _FFT;
  Algorithm* _IFFT;
  Algorithm* _RMSALGO;

 public:
  PitchYinProbabilities() {
    declareAlgorithm("PitchYinProbabilities");
    declareInput(_signal, TOKEN, "signal");
    declareOutput(_pitch, TOKEN, "pitch");
    declareOutput(_probabilities, TOKEN, "probabilities");
    declareOutput(_RMS, TOKEN, "RMS");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_PITCHYIN_H
