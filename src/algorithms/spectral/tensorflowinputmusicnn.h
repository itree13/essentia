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

#ifndef ESSENTIA_TENSORFLOWINPUTMUSICNN_H
#define ESSENTIA_TENSORFLOWINPUTMUSICNN_H

#include "essentiamath.h"
#include "algorithm.h"
#include "algorithmfactory.h"


namespace essentia {
namespace standard {

class TensorflowInputMusiCNN : public Algorithm {

 protected:
  Input<::essentia::VectorEx<Real> > _frame;
  Output<::essentia::VectorEx<Real> > _bands;

  Algorithm* _windowing;
  Algorithm* _spectrum;
  Algorithm* _melBands;
  Algorithm* _shift;
  Algorithm* _compression;

 public:
  TensorflowInputMusiCNN() {
    declareInput(_frame, "frame", "the audio frame");
    declareOutput(_bands, "bands", "the log compressed mel bands");

    _windowing = AlgorithmFactory::create("Windowing");
    _spectrum = AlgorithmFactory::create("Spectrum");
    _melBands = AlgorithmFactory::create("MelBands");
    _shift = AlgorithmFactory::create("UnaryOperator");
    _compression = AlgorithmFactory::create("UnaryOperator");
  }

  ~TensorflowInputMusiCNN() {
    if (_windowing) delete _windowing;
    if (_spectrum) delete _spectrum;
    if (_melBands) delete _melBands;
    if (_shift) delete _shift;
    if (_compression) delete _compression;
  }

  void declareParameters() {}

  void configure();
  void compute();

  static const char* name;
  static const char* category;
  static const char* description;

 protected:
  ::essentia::VectorEx<Real> _windowedFrame;
  ::essentia::VectorEx<Real> _spectrumFrame;
  ::essentia::VectorEx<Real> _melBandsFrame;
  ::essentia::VectorEx<Real> _shiftedFrame;
};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class TensorflowInputMusiCNN : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<Real> > _frame;
  Source<::essentia::VectorEx<Real> > _bands;

 public:
  TensorflowInputMusiCNN() {
    declareAlgorithm("TensorflowInputMusiCNN");
    declareInput(_frame, TOKEN, "frame");
    declareOutput(_bands, TOKEN, "bands");
  }
};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_TENSORFLOWINPUTMUSICNN_H
