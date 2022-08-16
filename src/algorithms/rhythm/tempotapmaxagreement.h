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

#ifndef ESSENTIA_TEMPOTAPMAXAGREEMENT_H
#define ESSENTIA_TEMPOTAPMAXAGREEMENT_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class TempoTapMaxAgreement : public Algorithm {

 protected:
  Input<::essentia::VectorEx<::essentia::VectorEx<Real> > > _tickCandidates;
  Output<::essentia::VectorEx<Real> > _ticks;
  Output<Real> _confidence;

 public:
  TempoTapMaxAgreement() {
    declareInput(_tickCandidates, "tickCandidates", "the tick candidates estimated using different beat trackers (or features) [s]");
    declareOutput(_ticks, "ticks", "the list of resulting ticks [s]");
    declareOutput(_confidence, "confidence", "confidence with which the ticks were detected [0, 5.32]");
  }

  ~TempoTapMaxAgreement() {
  };

  void declareParameters() {
  }

  void reset();
  void configure();
  void compute();

  static const char* name;
  static const char* category;
  static const char* description;

 private:
  Real _minTickTime;  // ignore peaks before this time [s]
  static const int _numberBins = 40; // number of histogram bins for information gain method
                                     // corresponds to Log2(40) = 5.32 maximum
                                     // confidence value

  ::essentia::VectorEx<Real> _histogramBins;
  ::essentia::VectorEx<Real> _binValues;

  // parameters for the continuity-based method
  Real _phaseThreshold;  // size of tolerance window for beat phase
  Real _periodThreshold; // size of tolerance window for beat period

  Real computeBeatInfogain(::essentia::VectorEx<Real>& ticks1, ::essentia::VectorEx<Real>& ticks2);

  void removeFirstSeconds(::essentia::VectorEx<Real>& ticks);
  void FindBeatError(const ::essentia::VectorEx<Real>& ticks1,
                     const ::essentia::VectorEx<Real>& ticks2,
                     ::essentia::VectorEx<Real>& beatError);
  Real FindEntropy(::essentia::VectorEx<Real>& beatError);
  size_t closestTick(const ::essentia::VectorEx<Real>& ticks, Real x);
  void histogram(const ::essentia::VectorEx<Real>& array, ::essentia::VectorEx<Real>& counter);

}; // class TempoTapMaxAgreement

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class TempoTapMaxAgreement : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<::essentia::VectorEx<Real> > > _tickCandidates;
  Source<::essentia::VectorEx<Real> > _ticks;
  Source<Real> _confidence;

 public:
  TempoTapMaxAgreement() {
    declareAlgorithm("TempoTapMaxAgreement");
    declareInput(_tickCandidates, TOKEN, "tickCandidates");
    declareOutput(_ticks, TOKEN, "ticks");
    declareOutput(_confidence, TOKEN, "confidence");
  }
};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_TEMPOTAPMAXAGREEMENT_H
