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

#ifndef ESSENTIA_TEMPOTAPDEGARA_H
#define ESSENTIA_TEMPOTAPDEGARA_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class TempoTapDegara : public Algorithm {

 protected:
  Input<::essentia::VectorEx<Real> > _onsetDetections;
  Output<::essentia::VectorEx<Real> > _ticks;

 public:
  TempoTapDegara() {
    declareInput(_onsetDetections, "onsetDetections", "the input frame-wise vector of onset detection values");
    declareOutput(_ticks, "ticks", "the list of resulting ticks [s]");

    _movingAverage = AlgorithmFactory::create("MovingAverage");
    _frameCutter = AlgorithmFactory::create("FrameCutter");
    _autocorrelation = AlgorithmFactory::create("AutoCorrelation");
  }

  ~TempoTapDegara() {
    if (_movingAverage) delete _movingAverage;
    if (_frameCutter) delete _frameCutter;
    if (_autocorrelation) delete _autocorrelation;
  };

  void declareParameters() {
    declareParameter("sampleRateODF", "the sampling rate of the onset detection function [Hz]", "(0,inf)", 44100./512);
    declareParameter("resample", "use upsampling of the onset detection function (may increase accuracy)", "{none,x2,x3,x4}", "none");
    declareParameter("maxTempo", "fastest tempo allowed to be detected [bpm]", "[60,250]", 208);
    declareParameter("minTempo", "slowest tempo allowed to be detected [bpm]", "[40,180]", 40);
  }

  void reset();
  void configure();
  void compute();

  static const char* name;
  static const char* category;
  static const char* description;

 private:
  // Davies' beat periods estimation:
  int _smoothingWindowHalfSize;
  static const int _numberCombs = 4;
  Real _frameDurationODF;
  Real _sampleRateODF;
  int _hopSizeODF;
  Real _hopDurationODF;
  int _resample;
  size_t _numberFramesODF;
  int _periodMinIndex;
  int _periodMaxIndex;
  int _periodMaxUserIndex;
  int _periodMinUserIndex;
  ::essentia::VectorEx<Real> _tempoWeights;
  ::essentia::VectorEx<::essentia::VectorEx<Real> > _transitionsViterbi;  // transition matrix for Viterbi
  Algorithm* _autocorrelation;
  Algorithm* _movingAverage;
  Algorithm* _frameCutter;
  void createTempoPreferenceCurve();
  void createViterbiTransitionMatrix();
  void findViterbiPath(const ::essentia::VectorEx<Real>& prior,
                     const ::essentia::VectorEx<::essentia::VectorEx<Real> > transitionMatrix,
                     const ::essentia::VectorEx<::essentia::VectorEx<Real> >& observations,
                     ::essentia::VectorEx<Real>& path);
  void computeBeatPeriodsDavies(::essentia::VectorEx<Real> detections,
                                ::essentia::VectorEx<Real>& beatPeriods,
                                ::essentia::VectorEx<Real>& beatEndPositions);
  void adaptiveThreshold(::essentia::VectorEx<Real>& array, int smoothingHalfSize);

  // Degara's beat tracking from periods:
  Real _alpha;
  Real _sigma_ibi;
  int _numberStates;    // number HMM states
  Real _resolutionODF;  // time resolution of ODF
  size_t _numberFrames; // number of ODF values
  void computeBeatsDegara(::essentia::VectorEx <Real>& detections,
                          const ::essentia::VectorEx<Real>& beatPeriods,
                          const ::essentia::VectorEx<Real>& beatEndPositions,
                          ::essentia::VectorEx<Real>& ticks);
  void computeHMMTransitionMatrix(const ::essentia::VectorEx<Real>& ibiPDF,
                                  ::essentia::VectorEx<::essentia::VectorEx<Real> >& transitions);
  void decodeBeats(std::map<Real, ::essentia::VectorEx<::essentia::VectorEx<Real> > >& transitionMatrix,
                   const ::essentia::VectorEx<Real>& beatPeriods,
                   const ::essentia::VectorEx<Real>& beatEndPositions,
                   const ::essentia::VectorEx<::essentia::VectorEx<Real> >& biy,
                   ::essentia::VectorEx<int>& sequenceStates);

  void gaussianPDF(::essentia::VectorEx<Real>& gaussian, Real gaussianStd, Real step, Real scale=1.);
}; // class TempoTapDegara

} // namespace standard
} // namespace essentia


#include "streamingalgorithmcomposite.h"
#include "pool.h"

namespace essentia {
namespace streaming {

class TempoTapDegara : public AlgorithmComposite {

 protected:
  SinkProxy<Real> _onsetDetections;
  Source<Real> _ticks;

  Pool _pool;
  Algorithm* _poolStorage;
  standard::Algorithm * _tempoTapDegara;

 public:
  TempoTapDegara();
  ~TempoTapDegara();

  void declareParameters() {
    declareParameter("sampleRateODF", "the sampling rate of the onset detection function [Hz]", "(0,inf)", 44100./512);
    declareParameter("resample", "use upsampling of the onset detection function (may increase accuracy)", "{none,x2,x3,x4}", "none");
    declareParameter("maxTempo", "fastest tempo allowed to be detected [bpm]", "[60,250]", 208);
    declareParameter("minTempo", "slowest tempo allowed to be detected [bpm]", "[40,180]", 40);
  }

  void configure() {
    _tempoTapDegara->configure(INHERIT("sampleRateODF"),
                               INHERIT("resample"),
                               INHERIT("maxTempo"),
                               INHERIT("minTempo"));
  }

  void declareProcessOrder() {
    declareProcessStep(SingleShot(_poolStorage));
    declareProcessStep(SingleShot(this));
  }

  AlgorithmStatus process();
  void reset();

  static const char* name;
  static const char* category;
  static const char* description;
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_TEMPOTAPDEGARA_H
