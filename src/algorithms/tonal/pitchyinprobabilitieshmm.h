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

#ifndef ESSENTIA_PITCHYINPROBABILITIESHMM_H
#define ESSENTIA_PITCHYINPROBABILITIESHMM_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class PitchYinProbabilitiesHMM : public Algorithm {

 private:
  Input<::essentia::VectorEx<::essentia::VectorEx<Real> > > _pitchCandidates;
  Input<::essentia::VectorEx<::essentia::VectorEx<Real> > > _probabilities;
  Output<::essentia::VectorEx<Real> > _pitch;

  Algorithm* _viterbi;

  Real _minFrequency;
  int _numberBinsPerSemitone;
  Real _selfTransition;
  Real _yinTrust;
  int _nPitch;
  int _transitionWidth;
  ::essentia::VectorEx<Real> _freqs;

  ::essentia::VectorEx<Real> _init;
  ::essentia::VectorEx<int> _from;
  ::essentia::VectorEx<int> _to;
  ::essentia::VectorEx<Real> _transProb;

  ::essentia::VectorEx<Real> _tempPitch;

 public:
  PitchYinProbabilitiesHMM() {
    declareInput(_pitchCandidates, "pitchCandidates", "the pitch candidates");
    declareInput(_probabilities, "probabilities", "the pitch probabilities");
    declareOutput(_pitch, "pitch", "pitch frequencies in Hz");

    _viterbi = AlgorithmFactory::create("Viterbi");
  }

  void declareParameters() {
    declareParameter("minFrequency", "minimum detected frequency", "(0,inf)", 61.735);
    declareParameter("numberBinsPerSemitone", "number of bins per semitone", "(1,inf)", 5);
    declareParameter("selfTransition", "the self transition probabilities", "(0,1)", 0.99);  
    declareParameter("yinTrust", "the yin trust parameter", "(0,1)", 0.5);
  }

  void configure();
  void compute();

  static const char* name;
  static const char* category;
  static const char* description;

 protected:
  const ::essentia::VectorEx<Real> calculateObsProb(const ::essentia::VectorEx<Real> pitchCandidates, const ::essentia::VectorEx<Real> probabilities);
}; // class PitchYin

} // namespace standard
} // namespace essentia


#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class PitchYinProbabilitiesHMM : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<::essentia::VectorEx<Real> > > _pitchCandidates;
  Sink<::essentia::VectorEx<::essentia::VectorEx<Real> > > _probabilities;
  Source<::essentia::VectorEx<Real> > _pitch;

 public:
  PitchYinProbabilitiesHMM() {
    declareAlgorithm("PitchYinProbabilitiesHMM");
    declareInput(_pitchCandidates, TOKEN, "pitchCandidates");
    declareInput(_probabilities, TOKEN, "probabilities");
    declareOutput(_pitch, TOKEN, "pitch");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_PITCHYIN_H
