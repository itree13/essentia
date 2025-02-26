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

#ifndef ESSENTIA_VECTORREALACCUMULATOR_H
#define ESSENTIA_VECTORREALACCUMULATOR_H

#include "streamingalgorithmcomposite.h"
#include "vectoroutput.h"

namespace essentia {
namespace streaming {

class VectorRealAccumulator : public AlgorithmComposite {
 protected:
  SinkProxy<::essentia::VectorEx<Real> > _value;
  Source<::essentia::VectorEx<::essentia::VectorEx<Real> > > _array;
  ::essentia::VectorEx<::essentia::VectorEx<Real> > _accu;
  Algorithm* _vectorOutput;

 public:
  VectorRealAccumulator();
  ~VectorRealAccumulator();

  void declareParameters() {}

  void reset();
  AlgorithmStatus process();

  void declareProcessOrder() {
    declareProcessStep(ChainFrom(_vectorOutput));
    declareProcessStep(SingleShot(this));
  }

  static const char* name;
  static const char* category;
  static const char* description;
};

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_VECTORREALACCUMULATOR_H