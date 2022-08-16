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

#ifndef ESSENTIA_NNLSCHROMA_H
#define ESSENTIA_NNLSCHROMA_H

#include "algorithm.h"
#include "nnls.h"

namespace essentia {
namespace standard {

class NNLSChroma : public Algorithm {
 public:

 protected:
  Input<::essentia::VectorEx<::essentia::VectorEx<Real> > > _logSpectrum;
  Input<::essentia::VectorEx<Real> > _meanTuning;
  Input<::essentia::VectorEx<Real> > _localTuning;
  Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _tunedLogfreqSpectrum;
  Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _semitoneSpectrum;
  Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _bassChromagram;
  Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _chromagram;

 public:
  NNLSChroma() {
    declareInput(_logSpectrum, "logSpectrogram", "log spectrum frames");
    declareInput(_meanTuning, "meanTuning", "mean tuning frames");
    declareInput(_localTuning, "localTuning", "local tuning frames");
    declareOutput(_tunedLogfreqSpectrum, "tunedLogfreqSpectrum", "Log frequency spectrum after tuning");
    declareOutput(_semitoneSpectrum, "semitoneSpectrum", "a spectral representation with one bin per semitone");
    declareOutput(_bassChromagram, "bassChromagram", " a 12-dimensional chromagram, restricted to the bass range");
    declareOutput(_chromagram, "chromagram", "a 12-dimensional chromagram, restricted with mid-range emphasis");
  }

  void declareParameters() {
    declareParameter("frameSize", "the input frame size of the spectrum vector", "(1,inf)", 1025);
    declareParameter("sampleRate", "the input sample rate", "(0,inf)", 44100.);
    declareParameter("useNNLS", "toggle between NNLS approximate transcription and linear spectral mapping", "{true,false}", true);
    declareParameter("tuningMode", "local uses a local average for tuning, global uses all audio frames. Local tuning is only advisable when the tuning is likely to change over the audio", "{global,local}", "global");
    declareParameter("spectralWhitening", "determines how much the log-frequency spectrum is whitened", "[0,1.0]", 1.0);
    declareParameter("spectralShape", " the shape of the notes in the NNLS dictionary", "(0.5,0.9)", 0.7);
    declareParameter("chromaNormalization", "determines whether or how the chromagrams are normalised", "{none,maximum,L1,L2}", "none");
  }

  void configure();
  void compute();
  void reset();

  static const char* name;
  static const char* category;
  static const char* description;
  static const Real precision;

 protected:
  bool _useNNLS;
  bool _tuningMode;
  int _doNormalizeChroma;
  size_t _frameSize;
  Real _sampleRate;
  Real _whitening;
  Real _spectralShape;
  ::essentia::VectorEx<int> _kernelFftIndex;
  ::essentia::VectorEx<int> _kernelNoteIndex;
  ::essentia::VectorEx<Real> _kernelValue;
  ::essentia::VectorEx<Real> _hw;
  ::essentia::VectorEx<Real> _sinvalues;
  ::essentia::VectorEx<Real> _cosvalues;
  ::essentia::VectorEx<Real> _dict;

  bool logFreqMatrix(Real fs, int frameSize, ::essentia::VectorEx<Real> outmatrix);
  Real cospuls(Real x, Real centre, Real width);
  Real pitchCospuls(Real x, Real centre, int binsperoctave);
  ::essentia::VectorEx<Real> SpecialConvolution(::essentia::VectorEx<Real> convolvee, ::essentia::VectorEx<Real> kernel);
  void dictionaryMatrix(::essentia::VectorEx<Real> dm, Real s_param);

};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class NNLSChroma : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<::essentia::VectorEx<Real> > > _logSpectrum;
  Sink<::essentia::VectorEx<Real> > _meanTuning;
  Sink<::essentia::VectorEx<Real> > _localTuning;
  Source<::essentia::VectorEx<::essentia::VectorEx<Real> > > _tunedLogfreqSpectrum;
  Source<::essentia::VectorEx<::essentia::VectorEx<Real> > > _semitoneSpectrum;
  Source<::essentia::VectorEx<::essentia::VectorEx<Real> > > _bassChromagram;
  Source<::essentia::VectorEx<::essentia::VectorEx<Real> > > _chromagram;

 public:
  NNLSChroma() {
    declareAlgorithm("NNLSChroma");
    declareInput(_logSpectrum, TOKEN, "logSpectrogram");
    declareInput(_meanTuning, TOKEN, "meanTuning");
    declareInput(_localTuning, TOKEN, "localTuning");
    declareOutput(_tunedLogfreqSpectrum, TOKEN, "tunedLogfreqSpectrum");
    declareOutput(_semitoneSpectrum, TOKEN, "semitoneSpectrum");
    declareOutput(_bassChromagram, TOKEN, "bassChromagram");
    declareOutput(_chromagram, TOKEN, "chromagram");
  }

};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_NNLSCHROMA_H
