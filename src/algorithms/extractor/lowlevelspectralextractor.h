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

#ifndef LOW_LEVEL_SPECTRAL_EXTRACTOR_H
#define LOW_LEVEL_SPECTRAL_EXTRACTOR_H

#include "streamingalgorithmcomposite.h"
#include "pool.h"
#include "algorithm.h"
#include "vectorinput.h"
#include "network.h"

namespace essentia {
namespace streaming {

class LowLevelSpectralExtractor : public AlgorithmComposite {
 protected:
  SinkProxy<Real> _signal;

  SourceProxy<::essentia::VectorEx<Real> > _bbands;
  SourceProxy<Real> _bbandsKurtosis;
  SourceProxy<Real> _bbandsSkewness;
  SourceProxy<Real> _bbandsSpread;
  SourceProxy<Real> _hfcValue;
  SourceProxy<::essentia::VectorEx<Real> > _mfccs;
  SourceProxy<Real> _pitchValue;
  SourceProxy<Real> _pitchConfidence;
  SourceProxy<Real> _pitchSalienceValue;
  SourceProxy<Real> _silence20;
  SourceProxy<Real> _silence30;
  SourceProxy<Real> _silence60;
  SourceProxy<Real> _spectralComplexityValue;
  SourceProxy<Real> _crestValue;
  SourceProxy<Real> _decreaseValue;
  SourceProxy<Real> _energyValue;
  SourceProxy<Real> _ebandLow;
  SourceProxy<Real> _ebandMidLow;
  SourceProxy<Real> _ebandMidHigh;
  SourceProxy<Real> _ebandHigh;
  SourceProxy<Real> _flatness;
  SourceProxy<Real> _fluxValue;
  SourceProxy<Real> _rmsValue;
  SourceProxy<Real> _rolloffValue;
  SourceProxy<Real> _strongPeakValue;
  SourceProxy<Real> _zeroCrossingRate;

  SourceProxy<Real> _inharmonicityValue;
  SourceProxy<::essentia::VectorEx<Real> > _tristimulusValue;
  SourceProxy<Real> _odd2even;

  Algorithm *_barkBands, *_centralMoments, *_crest, *_decrease,
            *_distributionShape, *_energyBand_0, *_energyBand_1,
            *_energyBand_2, *_energyBand_3, *_energy, *_flatnessdb,
            *_flux, *_frameCutter, *_hfc, *_harmonicPeaks, *_inharmonicity,
            *_mfcc, *_oddToEvenHarmonicEnergyRatio, *_pitchDetection,
            *_pitchSalience, *_rms, *_rollOff, *_silenceRate, *_spectralComplexity,
            *_spectralPeaks, *_spectrum, *_strongPeak, *_tristimulus,
            *_square, *_windowing, *_zcr;

  scheduler::Network* _network;

  bool _configured;
  void clearAlgos();

 public:
  LowLevelSpectralExtractor();
  ~LowLevelSpectralExtractor();

  void declareParameters() {
    declareParameter("frameSize", "the frame size for computing low level features", "(0,inf)", 2048);
    declareParameter("hopSize", "the hop size for computing low level features", "(0,inf)", 1024);
    declareParameter("sampleRate", "the audio sampling rate", "(0,inf)", 44100.0);
  }

  void declareProcessOrder() {
    declareProcessStep(ChainFrom(_frameCutter));
  }

  void configure();
  void createInnerNetwork();

  static const char* name;
  static const char* category;
  static const char* description;
};

} // namespace streaming
} // namespace essentia

namespace essentia {
namespace standard {

class LowLevelSpectralExtractor : public Algorithm {
 protected:
  Input<::essentia::VectorEx<Real> > _signal;
  Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _barkBands;
  Output<::essentia::VectorEx<Real> > _kurtosis;
  Output<::essentia::VectorEx<Real> > _skewness;
  Output<::essentia::VectorEx<Real> > _spread;
  Output<::essentia::VectorEx<Real> > _hfc;
  Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _mfcc;
  Output<::essentia::VectorEx<Real> > _pitch;
  Output<::essentia::VectorEx<Real> > _pitchConfidence;
  Output<::essentia::VectorEx<Real> > _pitchSalience;
  Output<::essentia::VectorEx<Real> > _threshold_0;
  Output<::essentia::VectorEx<Real> > _threshold_1;
  Output<::essentia::VectorEx<Real> > _threshold_2;
  Output<::essentia::VectorEx<Real> > _spectralComplexity;
  Output<::essentia::VectorEx<Real> > _crest;
  Output<::essentia::VectorEx<Real> > _decrease;
  Output<::essentia::VectorEx<Real> > _energy;
  Output<::essentia::VectorEx<Real> > _energyBand_0;
  Output<::essentia::VectorEx<Real> > _energyBand_1;
  Output<::essentia::VectorEx<Real> > _energyBand_2;
  Output<::essentia::VectorEx<Real> > _energyBand_3;
  Output<::essentia::VectorEx<Real> > _flatnessdb;
  Output<::essentia::VectorEx<Real> > _flux;
  Output<::essentia::VectorEx<Real> > _rms;
  Output<::essentia::VectorEx<Real> > _rollOff;
  Output<::essentia::VectorEx<Real> > _strongPeak;
  Output<::essentia::VectorEx<Real> > _zeroCrossingRate;
  Output<::essentia::VectorEx<Real> > _inharmonicity;
  Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _tristimulus;
  Output<::essentia::VectorEx<Real> > _oddToEvenHarmonicEnergyRatio;

  bool _configured;

  streaming::Algorithm* _lowLevelExtractor;
  streaming::VectorInput<Real>* _vectorInput;
  scheduler::Network* _network;
  Pool _pool;

 public:

  LowLevelSpectralExtractor();
  ~LowLevelSpectralExtractor();

  void declareParameters() {
    declareParameter("frameSize", "the frame size for computing low level features", "(0,inf)", 2048);
    declareParameter("hopSize", "the hop size for computing low level features", "(0,inf)", 1024);
    declareParameter("sampleRate", "the audio sampling rate", "(0,inf)", 44100.0);
  }

  void configure();
  void createInnerNetwork();
  void compute();
  void reset();

  static const char* name;
  static const char* category;
  static const char* description;
};

}
}

#endif
