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

#ifndef ESSENTIA_PITCHFILTER_H
#define ESSENTIA_PITCHFILTER_H

#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class PitchFilter : public Algorithm {

 private:
  Input<::essentia::VectorEx<Real> > _pitchConfidence;
  Input<::essentia::VectorEx<Real> > _pitch;
  Output<::essentia::VectorEx<Real> > _pitchFiltered;

  bool _useAbsolutePitchConfidence;
  long long _minChunkSize;
  int _confidenceThreshold;

  bool areClose(Real num1, Real num2);
  void splitToChunks(const ::essentia::VectorEx <Real>& pitch,
    ::essentia::VectorEx <::essentia::VectorEx <Real> >& chunks,
    ::essentia::VectorEx <long long>& chunksIndexes,
    ::essentia::VectorEx <long long>& chunksSize);
  void joinChunks(const ::essentia::VectorEx <::essentia::VectorEx <Real> >& chunks, ::essentia::VectorEx <Real>& result);
  Real confidenceOfChunk(const ::essentia::VectorEx <Real>& pitchConfidence, long long chunkIndex, long long chunkSize);
  void correctOctaveErrorsByChunks(::essentia::VectorEx <Real>& pitch);
  void removeExtremeValues(::essentia::VectorEx <Real>& pitch);
  void correctJumps(::essentia::VectorEx <Real>& pitch);
  void filterNoiseRegions(::essentia::VectorEx <Real>& pitch);
  void correctOctaveErrors(::essentia::VectorEx <Real>& pitch);
  void filterChunksByPitchConfidence(::essentia::VectorEx <Real>& pitch, const ::essentia::VectorEx <Real>& pitchConfidence);

 public:
  PitchFilter() {
    declareInput(_pitch, "pitch", "vector of pitch values for the input frames [Hz]");
    declareInput(_pitchConfidence, "pitchConfidence", "vector of pitch confidence values for the input frames");
    declareOutput(_pitchFiltered, "pitchFiltered", "vector of corrected pitch values [Hz]");
  }

  ~PitchFilter() {
  };

  void declareParameters() {
    declareParameter("minChunkSize", "minumum number of frames in non-zero pitch chunks", "[0,inf)", 30);
    declareParameter("useAbsolutePitchConfidence", "treat negative pitch confidence values as positive (use with melodia guessUnvoiced=True)", "{true,false}", false);
    declareParameter("confidenceThreshold", "ratio between the average confidence of the most confident chunk and the minimum allowed average confidence of a chunk", "[0,inf)", 36);
  }

  void configure();
  void compute();

  static const char* name;
  static const char* category;
  static const char* description;

}; // class PitchFilter

} // namespace standard
} // namespace essentia


#include "streamingalgorithmwrapper.h"

namespace essentia {
namespace streaming {

class PitchFilter : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<Real> > _pitchConfidence;
  Sink<::essentia::VectorEx<Real> > _pitch;
  Source<::essentia::VectorEx<Real> > _pitchFiltered;

 public:
  PitchFilter() {
    declareAlgorithm("PitchFilter");
    declareInput(_pitchConfidence, TOKEN, "pitchConfidence");
    declareInput(_pitch, TOKEN, "pitch");
    declareOutput(_pitchFiltered, TOKEN, "pitchFiltered");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_PITCHFILTER_H
