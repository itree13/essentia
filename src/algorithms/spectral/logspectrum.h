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

#ifndef ESSENTIA_LOGSPECTRUM_H
#define ESSENTIA_LOGSPECTRUM_H

#include "algorithm.h"

namespace essentia {
namespace standard {

class LogSpectrum : public Algorithm {
 public:

 protected:
  Input<::essentia::VectorEx<Real> > _spectrum;
  Output<::essentia::VectorEx<Real> > _logFreqSpectrum;
  Output<::essentia::VectorEx<Real> > _meanTuning;
  Output<Real> _localTuning;

 public:
  LogSpectrum() {
    declareInput(_spectrum, "spectrum", "spectrum frame");
    declareOutput(_logFreqSpectrum, "logFreqSpectrum", "log frequency spectrum frame");
    declareOutput(_meanTuning, "meanTuning", "normalized mean tuning frequency");
    declareOutput(_localTuning, "localTuning", "normalized local tuning frequency");
  }

  void declareParameters() {
    declareParameter("frameSize", "the input frame size of the spectrum vector", "(1,inf)", 1025);
    declareParameter("sampleRate", "the input sample rate", "(0,inf)", 44100.);
    declareParameter("rollOn", "this removes low-frequency noise - useful in quiet recordings", "[0,5]", 0.f);
    declareParameter("binsPerSemitone", " bins per semitone", "(0,inf)", 3.0);
    declareParameter("nOctave", "the number of octave of the output vector", "(0,10)", 7);
  }

  void configure();
  void compute();
  void reset();

  static const char* name;
  static const char* category;
  static const char* description;
  static const Real precision;

 protected:
  int _frameCount;
  int _nBPS;
  int _nOctave;
  int _nNote;
  size_t _frameSize;
  Real _sampleRate;
  Real _rollon;
  ::essentia::VectorEx<int> _kernelFftIndex;
  ::essentia::VectorEx<int> _kernelNoteIndex;
  ::essentia::VectorEx<Real> _meanTunings;
  ::essentia::VectorEx<Real> _localTunings;
  ::essentia::VectorEx<Real> _kernelValue;
  ::essentia::VectorEx<Real> _sinvalues;
  ::essentia::VectorEx<Real> _cosvalues;

  bool logFreqMatrix(Real fs, int frameSize, int maxOctave, ::essentia::VectorEx<Real> &outmatrix);
  Real cospuls(Real x, Real centre, Real width);
  Real pitchCospuls(Real x, Real centre, int binsperoctave);
  void initialize();
};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class LogSpectrum : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<Real> > _spectrum;
  Source<::essentia::VectorEx<Real> > _logFreqSpectrum;
  Source<::essentia::VectorEx<Real> > _meanTuning;
  Source<Real> _localTuning;

 public:
  LogSpectrum() {
    declareAlgorithm("LogSpectrum");
    declareInput(_spectrum, TOKEN, "spectrum");
    declareOutput(_logFreqSpectrum, TOKEN, "logFreqSpectrum");
    declareOutput(_meanTuning, TOKEN, "meanTuning");
    declareOutput(_localTuning, TOKEN, "localTuning");
  }
};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_LOGSPECTRUM_H
