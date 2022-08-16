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

#ifndef ESSENTIA_SPRMODELSYNTH_H
#define ESSENTIA_SPRMODELSYNTH_H


#include "algorithm.h"
#include "algorithmfactory.h"

#include <fstream>

namespace essentia {
namespace standard {

class SprModelSynth : public Algorithm {

 protected:
  Input<::essentia::VectorEx<Real> > _magnitudes;
  Input<::essentia::VectorEx<Real> > _frequencies;
  Input<::essentia::VectorEx<Real> > _phases;
  Input<::essentia::VectorEx<Real> > _res;

  Output<::essentia::VectorEx<Real> > _outframe;
  Output<::essentia::VectorEx<Real> > _outsineframe;
  Output<::essentia::VectorEx<Real> > _outresframe;

  Real _sampleRate;
  int _fftSize;
  int _hopSize;

  Algorithm* _sineModelSynth;
  Algorithm* _ifftSine;
  Algorithm* _overlapAdd;



 public:
  SprModelSynth() {
    declareInput(_magnitudes, "magnitudes", "the magnitudes of the sinusoidal peaks");
    declareInput(_frequencies, "frequencies", "the frequencies of the sinusoidal peaks [Hz]");
    declareInput(_phases, "phases", "the phases of the sinusoidal peaks");
    declareInput(_res, "res", "the residual frame");

    declareOutput(_outframe, "frame", "the output audio frame of the Sinusoidal Plus Stochastic model");
    declareOutput(_outsineframe, "sineframe", "the output audio frame for sinusoidal component ");
    declareOutput(_outresframe, "resframe", "the output audio frame for stochastic component ");

    _sineModelSynth = AlgorithmFactory::create("SineModelSynth");

    _ifftSine = AlgorithmFactory::create("IFFT");
    _overlapAdd = AlgorithmFactory::create("OverlapAdd");

  }

  ~SprModelSynth() {

    delete _sineModelSynth;
    delete _ifftSine;
    delete _overlapAdd;

  }

  void declareParameters() {
    declareParameter("fftSize", "the size of the output FFT frame (full spectrum size)", "[1,inf)", 2048);
    declareParameter("hopSize", "the hop size between frames", "[1,inf)", 512);
    declareParameter("sampleRate", "the audio sampling rate [Hz]", "(0,inf)", 44100.);
  }

  void configure();
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

class SprModelSynth : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<Real> > _magnitudes;
  Sink<::essentia::VectorEx<Real> > _frequencies;
  Sink<::essentia::VectorEx<Real> > _phases;
  Sink<::essentia::VectorEx<Real> > _res;
  
  Source<::essentia::VectorEx<Real> > _outframe;
  Source<::essentia::VectorEx<Real> > _outsineframe;
  Source<::essentia::VectorEx<Real> > _outresframe;

 public:
  SprModelSynth() {
    declareAlgorithm("SprModelSynth");
    declareInput(_magnitudes, TOKEN, "magnitudes");
    declareInput(_frequencies, TOKEN, "frequencies");
    declareInput(_phases, TOKEN, "phases");
    declareInput(_res, TOKEN, "res");

    declareOutput(_outframe, TOKEN, "frame");
    declareOutput(_outsineframe, TOKEN, "sineframe");
    declareOutput(_outresframe, TOKEN, "resframe");
  }
};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_SPRMODELSYNTH_H
