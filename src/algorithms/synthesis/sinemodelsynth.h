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

#ifndef ESSENTIA_SINEMODELSYNTH_H
#define ESSENTIA_SINEMODELSYNTH_H

// defines for generateSine function
#define BH_SIZE 1001
#define BH_SIZE_BY2 501
#define MFACTOR 100


#include "algorithm.h"

namespace essentia {
namespace standard {

class SineModelSynth : public Algorithm {

 private:
  Input<::essentia::VectorEx<Real> > _magnitudes;
  Input<::essentia::VectorEx<Real> > _frequencies;
  Input<::essentia::VectorEx<Real> > _phases;
  Output<::essentia::VectorEx<std::complex<Real> > > _outfft;

  Real _sampleRate;
  int _fftSize;
  int _hopSize;

  ::essentia::VectorEx<Real> _lastytfreq;
  ::essentia::VectorEx<Real> _lastytphase;


 public:
  SineModelSynth() {
    declareInput(_magnitudes, "magnitudes", "the magnitudes of the sinusoidal peaks");
    declareInput(_frequencies, "frequencies", "the frequencies of the sinusoidal peaks [Hz]");
    declareInput(_phases, "phases", "the phases of the sinusoidal peaks");
    declareOutput(_outfft, "fft", "the output FFT frame");
  }

  void declareParameters() {
    declareParameter("fftSize", "the size of the output FFT frame (full spectrum size)", "[1,inf)", 2048);
    declareParameter("hopSize", "the hop size between frames", "[1,inf)", 512);
    declareParameter("sampleRate", "the audio sampling rate [Hz]", "(0,inf)", 44100.);
  }

  void configure() {
    _sampleRate = parameter("sampleRate").toReal();
    _fftSize = parameter("fftSize").toInt();
    _hopSize = parameter("hopSize").toInt();
    
  }

  void compute();


  static const char* name;
  static const char* category;
  static const char* description;

};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class SineModelSynth : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<Real> > _magnitudes;
  Sink<::essentia::VectorEx<Real> > _frequencies;
  Sink<::essentia::VectorEx<Real> > _phases;
  Source<::essentia::VectorEx<std::complex<Real> > > _outfft;

 public:
  SineModelSynth() {
    declareAlgorithm("SineModelSynth");
    declareInput(_magnitudes, TOKEN, "magnitudes");
    declareInput(_frequencies, TOKEN, "frequencies");
    declareInput(_phases, TOKEN, "phases");
    declareOutput(_outfft, TOKEN, "fft");
  }
};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_SINEMODELSYNTH_H
