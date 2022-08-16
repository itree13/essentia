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

#ifndef ESSENTIA_PITCHCONTOURSEGMENTATION_H
#define ESSENTIA_PITCHCONTOURSEGMENTATION_H

#include "algorithm.h"
#include "algorithmfactory.h"

namespace essentia {
namespace standard {

class PitchContourSegmentation : public Algorithm {

 private:
  Input<::essentia::VectorEx<Real> > _pitch;
  Input<::essentia::VectorEx<Real> > _signal;
  Output<::essentia::VectorEx<Real> > _onset;
  Output<::essentia::VectorEx<Real> > _duration;
  Output<::essentia::VectorEx<Real> > _MIDIpitch;
    
  Algorithm* frameCutter;
  Algorithm* RMS;

 public:
  PitchContourSegmentation() {
    declareInput(_pitch, "pitch", "estimated pitch contour [Hz]");
    declareInput(_signal, "signal", "input audio signal");
    declareOutput(_onset, "onset", "note onset times [s]");
    declareOutput(_duration, "duration", "note durations [s]");
    declareOutput(_MIDIpitch, "MIDIpitch", "quantized MIDI pitch value");
  }

  void declareParameters() {
    declareParameter("minDuration", "minimum note duration [s]", "(0,inf)", 0.1);
    declareParameter("tuningFrequency", "tuning reference frequency  [Hz]", "(0,22000)", 440);
    declareParameter("sampleRate", "sample rate of the audio signal", "(0,inf)", 44100);
    declareParameter("hopSize", "hop size of the extracted pitch", "(0,inf)", 128);
    declareParameter("rmsThreshold", "zscore threshold for note segmentation", "(-inf,0)", -2);
    declareParameter("pitchDistanceThreshold", "pitch threshold for note segmentation [cents]", "(0,inf)", 60);
  }

  void compute();
  void configure();
  void reSegment();

  static const char* name;
  static const char* category;
  static const char* description;

 protected:
  Real _minDur;
  int minDurPitchSamples;
  Real _tuningFreq;
  Real _hopSize;
  Real _sampleRate;
  ::essentia::VectorEx<Real> startC;
  ::essentia::VectorEx<Real> endC;
  ::essentia::VectorEx<Real> pitch;
  int _hopSizeFeat;
  int _frameSizeFeat;
  Real _pitchDistanceThreshold;
  Real _rmsThreshold;

};

} // namespace standard
} // namespace essentia

// TO DO: STREAMING MODE!

#endif // ESSENTIA_PITCHCONTOURSEGMENTATION_H
