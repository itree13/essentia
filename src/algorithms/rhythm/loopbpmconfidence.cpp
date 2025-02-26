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

#include "loopbpmconfidence.h"
#include "essentiamath.h"

using namespace std;
using namespace essentia;

namespace essentia {
namespace standard {

const char* LoopBpmConfidence::name = "LoopBpmConfidence";
const char* LoopBpmConfidence::category = "Rhythm";
const char* LoopBpmConfidence::description = DOC("This algorithm takes an audio signal and a BPM estimate for that signal "
  "and predicts the reliability of the BPM estimate in a value from 0 to 1. The audio signal is assumed to be a musical loop "
  "with constant tempo. The confidence returned is based on comparing the duration of the signal with multiples of the BPM estimate "
  "(see [1] for more details)."
"\n"
"\n"
"References:\n"
"  [1] Font, F., & Serra, X. (2016). Tempo Estimation for Music Loops and a Simple Confidence Measure.\n"
"  Proceedings of the International Society for Music Information Retrieval Conference (ISMIR).\n\n");

void LoopBpmConfidence::configure() {
  _envelope->configure("sampleRate", parameter("sampleRate").toInt(),
                       "attackTime", 10.0,
                       "releaseTime", 10.0);
}

void LoopBpmConfidence::compute() {
  const Real& bpmEstimate = _bpmEstimate.get();
  Real& confidence = _confidence.get();

  if (bpmEstimate == 0){
    // if estimated bpm is 0, we already know that the confidence will be 0
    confidence = 0.0;
  } else {
    const ::essentia::VectorEx<Real>& signal = _signal.get();

    // Get original duration.
    int duration_samples = signal.size();

    // Check first that the signal is non-empty
    if (duration_samples!=0){
      // Compute envelope
      ::essentia::VectorEx<Real> envelope;
      _envelope->input("signal").set(signal);
      _envelope->output("signal").set(envelope);
      _envelope->compute();

      // Compute threshold
      Real threshold = *std::max_element(envelope.begin(), envelope.end()) * 0.05;

      // Find start position
      int start_position = 0;
      for (int i=0; i<(int)envelope.size(); i++){
        if (envelope[i] >= threshold){
          start_position = i;
          break;
        }
      }

      // Find end position
      int end_position = 0;
      for (int i=envelope.size() - 1; i>=0; i--){
        if (envelope[i] >= threshold){
          end_position = i;
          break;
        }
      }

      // Build vector with all durations to check
      ::essentia::VectorEx<int> durations_to_check;
      durations_to_check.resize(4);
      durations_to_check[0] = duration_samples;
      durations_to_check[1] = duration_samples - start_position;
      durations_to_check[2] = end_position;
      durations_to_check[3] = end_position - start_position;

      // Check all durations
      ::essentia::VectorEx<Real> confidences;
      confidences.resize(4);
      Real beatDuration = (60.0 * parameter("sampleRate").toReal()) / bpmEstimate;
      Real lambdaThreshold = beatDuration * 0.5;
      for (int i=0; i<(int)durations_to_check.size(); i++){
        int duration = durations_to_check[i];
        int minDistance = duration_samples; // Set maximum duration
        for (int j=1; j<128; j++) { // Iterate over possible beat lengths (1-127)
          int nBeatDuration = (int)round(beatDuration * j);
          int distance = abs(duration - nBeatDuration);
          if (distance < minDistance) {
            minDistance = distance;
          }
        }
        if (minDistance > lambdaThreshold) {
          confidences[i] = 0.0;
        } else {
          confidences[i] = 1.0 - (Real)minDistance / lambdaThreshold;
        }
      }  
      confidence = *std::max_element(confidences.begin(), confidences.end());
    } else {
      confidence = 0.0;  // Confidence set to zero for empty signals.
    }
  }
}

} // namespace standard
} // namespace essentia
