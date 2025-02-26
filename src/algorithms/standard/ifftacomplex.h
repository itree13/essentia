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

#ifndef ESSENTIA_IFFTACOMPLEX_H
#define ESSENTIA_IFFTACOMPLEX_H

#include "algorithm.h"
#include "threading.h"
#include <complex>
#include <Accelerate/Accelerate.h>

namespace essentia {
namespace standard {

class IFFTAComplex : public Algorithm {

 protected:
  Input<::essentia::VectorEx<std::complex<Real> > > _fft;
  Output<::essentia::VectorEx<std::complex<Real> > > _signal;

 public:
  IFFTAComplex() {
    declareInput(_fft, "fft", "the input frame");
    declareOutput(_signal, "frame", "the IFFT of the input frame");
      
    fftSetup = NULL;
    accelBuffer.realp = NULL;
    accelBuffer.imagp = NULL;
    _fftPlanSize = 0;
  }

  ~IFFTAComplex();

  void declareParameters() {
    declareParameter("size", "the expected size of the input frame. This is purely optional and only targeted at optimizing the creation time of the FFT object", "[1,inf)", 1024);
    declareParameter("normalize", "wheter to normalize the output by the FFT length.", "{true,false}", true);
  }


  void compute();
  void configure();

  static const char* name;
  static const char* category;
  static const char* description;

 protected:
  FFTSetup fftSetup;
  int logSize;
  int _fftPlanSize;
  DSPSplitComplex accelBuffer;

  bool _normalize;

  void createFFTObject(int size);
};

} // namespace standard
} // namespace essentia

#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class IFFTAComplex : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<std::complex<Real> > > _fft;
  Source<::essentia::VectorEx<Real> > _signal;

 public:
  IFFTAComplex() {
    declareAlgorithm("IFFTC");
    declareInput(_fft, TOKEN, "fft");
    declareOutput(_signal, TOKEN, "frame");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_IFFTACOMPLEX_H
