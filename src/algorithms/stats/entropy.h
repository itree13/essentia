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

#ifndef ESSENTIA_ENTROPY_H
#define ESSENTIA_ENTROPY_H

#include "algorithm.h"

namespace essentia {
namespace standard {

class Entropy : public Algorithm {

 protected:
  Input<::essentia::VectorEx<Real> > _array;
  Output<Real> _entropy;

 public:
  Entropy() {
    declareInput(_array, "array", "the input array (cannot contain negative values, and must be non-empty)");
    declareOutput(_entropy, "entropy", "the entropy of the input array");
  }

  void declareParameters() {}

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

class Entropy : public StreamingAlgorithmWrapper {

 protected:
  Sink<::essentia::VectorEx<Real> > _array;
  Source<Real> _entropy;

 public:
  Entropy() {
    declareAlgorithm("Entropy");
    declareInput(_array, TOKEN, "array");
    declareOutput(_entropy, TOKEN, "entropy");
  }
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_ENTROPY_H
