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
#ifndef ESSENTIA_CHROMACROSSSIMILARITY_H
#define ESSENTIA_CHROMACROSSSIMILARITY_H
#include "algorithmfactory.h"
#include <complex>

namespace essentia {
namespace standard {

class ChromaCrossSimilarity : public Algorithm {
  protected:
   Input<::essentia::VectorEx<::essentia::VectorEx<Real> > > _queryFeature;
   Input<::essentia::VectorEx<::essentia::VectorEx<Real> > > _referenceFeature;
   Output<::essentia::VectorEx<::essentia::VectorEx<Real> > > _csm;
  public:
   ChromaCrossSimilarity() {
    declareInput(_queryFeature, "queryFeature", "frame-wise chromagram of the query song (e.g., a HPCP)");
    declareInput(_referenceFeature, "referenceFeature", "frame-wise chromagram of the reference song (e.g., a HPCP)");
    declareOutput(_csm, "csm", "2D binary cross-similarity matrix of the query and reference features");
   }

   void declareParameters() {
    declareParameter("frameStackStride", "stride size to form a stack of frames (e.g., 'frameStackStride'=1 to use consecutive frames; 'frameStackStride'=2 for using every second frame)", "[1,inf)", 1);
    declareParameter("frameStackSize", "number of input frames to stack together and treat as a feature vector for similarity computation. Choose 'frameStackSize=1' to use the original input frames without stacking", "[0,inf)", 9);
    declareParameter("binarizePercentile", "maximum percent of distance values to consider as similar in each row and each column", "[0,1]", 0.095);
    declareParameter("oti", "whether to transpose the key of the reference song to the query song by Optimal Transposition Index [1]", "{true,false}", true);
    declareParameter("noti", "number of circular shifts to be checked for Optimal Transposition Index [1]", "[0,inf)", 12);
    declareParameter("otiBinary", "whether to use the OTI-based chroma binary similarity method [3]", "{true,false}", false);
    declareParameter("streaming", "whether to accumulate the input 'queryFeature' in the euclidean similarity matrix calculation on each compute() method call", "{true,false}", false);
  }

   void configure();
   void compute();
   void reset();

   static const char* name;
   static const char* category;
   static const char* description;

  protected:
   int _frameStackStride;
   int _frameStackSize;
   Real _binarizePercentile;
   int _noti;
   bool _oti;
   bool _otiBinary;
   bool _streaming;
   Real _mathcCoef;
   Real _mismatchCoef;
   bool _status;
   int _otiIdx;
   int _iterIdx;
   size_t queryFeatureSize;
   size_t referenceFeatureSize;
   ::essentia::VectorEx<::essentia::VectorEx<Real> > queryFeature;
   ::essentia::VectorEx<::essentia::VectorEx<Real> > referenceFeature;
   ::essentia::VectorEx<::essentia::VectorEx<Real> >  _queryFeatureStack;
   ::essentia::VectorEx<::essentia::VectorEx<Real> >  _referenceFeatureStack;
   ::essentia::VectorEx<Real> _thresholdQuery;
   ::essentia::VectorEx<Real> _thresholdReference;
   ::essentia::VectorEx<::essentia::VectorEx<Real> > _pdistances;
   ::essentia::VectorEx<::essentia::VectorEx<Real> > _accumEucDistances;
   ::essentia::VectorEx<Real> getColsAtVecIndex(::essentia::VectorEx<::essentia::VectorEx<Real> >& inputMatrix, int index) const; 
};

} // namespace standard
} // namespace essentia

#include "streamingalgorithm.h"

namespace essentia {
namespace streaming {

class ChromaCrossSimilarity : public Algorithm {
 protected:
  Sink<::essentia::VectorEx<Real> > _queryFeature;
  Source<::essentia::VectorEx<Real> > _csm;

  // params variables
  int _frameStackStride;
  int _frameStackSize;
  Real _binarizePercentile;
  int _noti;
  bool _oti;
  bool _otiBinary;
  Real _mathcCoef;
  Real _mismatchCoef;
  Real _minFramesSize;
  ::essentia::VectorEx<::essentia::VectorEx<Real> > _referenceFeature;
  ::essentia::VectorEx<::essentia::VectorEx<Real> > _referenceFeatureStack;
  ::essentia::VectorEx<::essentia::VectorEx<Real> > _outputSimMatrix;

 public:
  ChromaCrossSimilarity() : Algorithm() {
    declareInput(_queryFeature, 10, "queryFeature", "input chromagram of the query song. (eg: a HPCP)");
    declareOutput(_csm, 1, "csm", "2D binary cross-similarity matrix of the query and reference chromagram");
  }

  ~ChromaCrossSimilarity() {}

  AlgorithmStatus process();
  void reset();

  void declareParameters() {
    declareParameter("referenceFeature", "2D matrix corresponds to the input chromagram of the reference song. (eg. a HPCP)", "", ::essentia::VectorEx<::essentia::VectorEx<Real> >());
    declareParameter("frameStackStride", "stride size to form a stack of frames (e.g., 'frameStackStride'=1 to use consecutive frames; 'frameStackStride'=2 for using every second frame)", "[1,inf)", 1);
    declareParameter("frameStackSize", "number of input frames to stack together and treat as a feature vector for similarity computation. Choose 'frameStackSize=1' to use the original input frames without stacking", "[0,inf)", 9);
    declareParameter("binarizePercentile", "maximum percent of distance values to consider as similar in each row and each column", "[0,1]", 0.095);
    declareParameter("oti", "optimal transposition index of the query and reference song if known", "[0,inf]", 0);
    declareParameter("otiBinary", "whether to use the OTI-based chroma binary similarity method", "{true,false}", false);
  }

  void configure();

  static const char* name;
  static const char* category;
  static const char* description;

};

} // namespace streaming
} // namespace essentia
 #endif // ESSENTIA_CHROMACROSSSIMILARITY_H
