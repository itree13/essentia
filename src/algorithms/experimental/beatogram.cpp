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

#include "beatogram.h"
#include "essentiamath.h"

using namespace std;
using namespace essentia;
using namespace standard;

const char* Beatogram::name = "Beatogram";
const char* Beatogram::category = "Rhythm";
const char* Beatogram::description = DOC("This algorithm filters the loudness matrix given by BeatsLoudness algorithm in order to keep only the most salient beat band representation.\n"
"This algorithm has been found to be useful for estimating time signatures.\n"
"\n"
"Quality: experimental (not evaluated, do not use)");

void Beatogram::configure() {
  _windowSize = parameter("size").toInt();
}

void Beatogram::compute() {
  const ::essentia::VectorEx<Real>& loudness = _loudness.get();
  const ::essentia::VectorEx<::essentia::VectorEx<Real> >& loudnessBand = _loudnessBandRatio.get();
  ::essentia::VectorEx<::essentia::VectorEx<Real> >& beatogram = _beatogram.get();
  int nticks = loudnessBand.size();
  ::essentia::VectorEx<Real> meanRatiosPerTick(nticks, 0.0);
  ::essentia::VectorEx<Real> medianRatiosPerTick(nticks, 0.0);
  for (int i=0; i<nticks; i++) {
    meanRatiosPerTick[i] = mean(loudnessBand[i]);
    medianRatiosPerTick[i] = median(loudnessBand[i]);
  }
  // transpose loudnessBand so it is [bands x ticks]
  beatogram = transpose(loudnessBand);
  int nbands = beatogram.size();
  // compute mean ratios for each tick around a window of 16 beats:
  ::essentia::VectorEx<::essentia::VectorEx<Real> > meanRatiosPerBand(nbands, ::essentia::VectorEx<Real>(nticks));
  ::essentia::VectorEx<::essentia::VectorEx<Real> > medianRatiosPerBand(nbands, ::essentia::VectorEx<Real>(nticks));
  for (int iBand=0; iBand<nbands; iBand++) {
    for (int iTick=0; iTick<nticks; iTick++) {
      int start = max(0, iTick - _windowSize/2);
      int end = min(nticks, start + _windowSize);
      if (end == nticks) start = end-_windowSize;
      const ::essentia::VectorEx<Real>& band = beatogram[iBand];
      ::essentia::VectorEx<Real> window(band.begin()+start, band.begin()+end);
      meanRatiosPerBand[iBand][iTick] = mean(window);
      medianRatiosPerBand[iBand][iTick] = median(window);
    }
  }
  // filter out beatogram:
  for (int iBand=0; iBand<nbands; iBand++) {
    for (int iTick=0; iTick<nticks; iTick++) {
      Real bandThresh = max(medianRatiosPerBand[iBand][iTick],
                            meanRatiosPerBand[iBand][iTick]);
      Real tickThresh = max(medianRatiosPerTick[iTick],
                            meanRatiosPerTick[iTick]);
      if (beatogram[iBand][iTick] < bandThresh &&
          beatogram[iBand][iTick] <= tickThresh) {
        beatogram[iBand][iTick] = 0.0;
      }
      else {
        beatogram[iBand][iTick] *= loudness[iTick];
      }
    }
  }
}
