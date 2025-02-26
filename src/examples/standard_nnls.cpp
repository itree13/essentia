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

#include <iostream>
#include <fstream>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>
#include <essentia/pool.h>
#include "credit_libav.h"

using namespace std;
using namespace essentia;
using namespace essentia::standard;

int main(int argc, char* argv[]) {

  if (argc != 3) {
    cout << "Error: incorrect number of arguments." << endl;
    cout << "Usage: " << argv[0] << " audio_input yaml_output" << endl;
    creditLibAV();
    exit(1);
  }

  string audioFilename = argv[1];
  string outputFilename = argv[2];

  // register the algorithms in the factory(ies)
  essentia::init();

  Pool pool;

  /////// PARAMS //////////////
  int sampleRate = 44100;
  int frameSize = 16384;
  int hopSize = 1024 * 2;

  // we want to compute the NNLS chroma of a file: we need the create the following:
  // audioloader -> framecutter -> windowing -> Spectrum -> LogSpectrum -> NNLSChroma

  AlgorithmFactory& factory = standard::AlgorithmFactory::instance();

  Algorithm* audio = factory.create("MonoLoader",
                                    "filename", audioFilename,
                                    "sampleRate", sampleRate);

  Algorithm* fc    = factory.create("FrameCutter",
                                    "frameSize", frameSize,
                                    "hopSize", hopSize);

  Algorithm* w     = factory.create("Windowing",
                                    "type", "hamming",
                                    "normalized", false);

  Algorithm* spec            = factory.create("Spectrum");
  Algorithm* logfreqspectrum = factory.create("LogSpectrum",
                                              "frameSize", 8192 + 1);
  Algorithm* nnls            = factory.create("NNLS");




  /////////// CONNECTING THE ALGORITHMS ////////////////
  cout << "-------- connecting algos ---------" << endl;

  // Audio -> FrameCutter
  ::essentia::VectorEx<Real> audioBuffer;

  audio->output("audio").set(audioBuffer);
  fc->input("signal").set(audioBuffer);

  // FrameCutter -> Windowing -> Spectrum
  ::essentia::VectorEx<Real> frame, windowedFrame;

  fc->output("frame").set(frame);
  w->input("frame").set(frame);

  w->output("frame").set(windowedFrame);
  spec->input("frame").set(windowedFrame);

  // Spectrum -> LogSpectrum -> NNLSChroma
  Real tuninig;
  ::essentia::VectorEx<Real> spectrum, logFreqSpectrum, tuningFrames, meanTuning;
  ::essentia::VectorEx<::essentia::VectorEx<Real> > logSpectrumFrames, tunedLogFreqSpectrum, semitoneSpectrum, bassChromagram, chromagram;

  spec->output("spectrum").set(spectrum);
  logfreqspectrum->input("spectrum").set(spectrum);

  logfreqspectrum->output("logFreqSpectrum").set(logFreqSpectrum);
  logfreqspectrum->output("meanTuning").set(meanTuning);
  logfreqspectrum->output("localTuning").set(tuninig);


  nnls->input("logSpectrogram").set(logSpectrumFrames);
  nnls->input("meanTuning").set(meanTuning);
  nnls->input("localTuning").set(tuningFrames);

  nnls->output("tunedLogfreqSpectrum").set(tunedLogFreqSpectrum);
  nnls->output("semitoneSpectrum").set(semitoneSpectrum);
  nnls->output("bassChromagram").set(bassChromagram);
  nnls->output("chromagram").set(chromagram);



  /////////// STARTING THE ALGORITHMS //////////////////
  cout << "-------- start processing " << audioFilename << " --------" << endl;

  audio->compute();

  while (true) {

    // compute a frame
    fc->compute();

    // if it was the last one (ie: it was empty), then we're done.
    if (!frame.size()) {
      break;
    }

    // if the frame is silent, just drop it and go on processing
    if (isSilent(frame)) continue;

    w->compute();
    spec->compute();
    logfreqspectrum->compute();

    logSpectrumFrames.push_back(logFreqSpectrum);
    tuningFrames.push_back(tuninig);
  }

  nnls->compute();

  pool.add("lowlevel.nnlsBase", chromagram);

  // aggregate the results
  Pool aggrPool; // the pool with the aggregated NNLS Chroma values
  const char* stats[] = { "mean", "var", "min", "max" };

  Algorithm* aggr = AlgorithmFactory::create("PoolAggregator",
                                             "defaultStats", arrayToVector<string>(stats));

  aggr->input("input").set(pool);
  aggr->output("output").set(aggrPool);
  aggr->compute();

  // write results to file
  cout << "-------- writing results to file " << outputFilename << " ---------" << endl;

  Algorithm* output = AlgorithmFactory::create("YamlOutput",
                                               "filename", outputFilename);
  output->input("pool").set(aggrPool);
  output->compute();

  delete audio;
  delete fc;
  delete w;
  delete spec;
  delete nnls;
  delete logfreqspectrum;
  delete aggr;
  delete output;

  essentia::shutdown();

  return 0;
}
