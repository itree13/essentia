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

#include "tensorflowpredictvggish.h"

using namespace std;

namespace essentia {
namespace streaming {

const char* TensorflowPredictVGGish::name = essentia::standard::TensorflowPredictVGGish::name;
const char* TensorflowPredictVGGish::category = essentia::standard::TensorflowPredictVGGish::category;
const char* TensorflowPredictVGGish::description = essentia::standard::TensorflowPredictVGGish::description;

TensorflowPredictVGGish::TensorflowPredictVGGish() : AlgorithmComposite(),
    _frameCutter(0), _tensorflowInputVGGish(0), _vectorRealToTensor(0), _tensorToPool(0),
    _tensorflowPredict(0), _poolToTensor(0), _tensorToVectorReal(0), _configured(false) {

  declareInput(_signal, 4096, "signal", "the input audio signal sampled at 16 kHz");
  declareOutput(_predictions, 0, "predictions", "the output values from the model node named after `output`");
}


void TensorflowPredictVGGish::createInnerNetwork() {
  AlgorithmFactory& factory = AlgorithmFactory::instance();

  _frameCutter            = factory.create("FrameCutter");
  _tensorflowInputVGGish  = factory.create("TensorflowInputVGGish");
  _vectorRealToTensor     = factory.create("VectorRealToTensor");
  _tensorToPool           = factory.create("TensorToPool");
  _tensorflowPredict      = factory.create("TensorflowPredict");
  _poolToTensor           = factory.create("PoolToTensor");
  _tensorToVectorReal     = factory.create("TensorToVectorReal");

  _tensorflowInputVGGish->output("bands").setBufferType(BufferUsage::forMultipleFrames);

  _signal                                  >> _frameCutter->input("signal");
  _frameCutter->output("frame")            >> _tensorflowInputVGGish->input("frame");
  _tensorflowInputVGGish->output("bands")  >> _vectorRealToTensor->input("frame");
  _vectorRealToTensor->output("tensor")    >>  _tensorToPool->input("tensor");
  _tensorToPool->output("pool")            >>  _tensorflowPredict->input("poolIn");
  _tensorflowPredict->output("poolOut")    >>  _poolToTensor->input("pool");
  _poolToTensor->output("tensor")          >>  _tensorToVectorReal->input("tensor");

  attach(_tensorToVectorReal->output("frame"), _predictions);

  _network = new scheduler::Network(_frameCutter);
}


void TensorflowPredictVGGish::clearAlgos() {
  if (!_configured) return;
  delete _network;
}


TensorflowPredictVGGish::~TensorflowPredictVGGish() {
  clearAlgos();
}


void TensorflowPredictVGGish::reset() {
  AlgorithmComposite::reset();
}


void TensorflowPredictVGGish::configure() {
  if (_configured) {
    clearAlgos();
  }

  createInnerNetwork();

  int patchHopSize = parameter("patchHopSize").toInt();
  string lastPatchMode = parameter("lastPatchMode").toString();
  bool accumulate = parameter("accumulate").toBool();
  int patchSize = parameter("patchSize").toInt();
  int batchSize = parameter("batchSize").toInt();

  if (accumulate) batchSize = -1;

  // Hardcoded parameters matching the training setup:
  // https://github.com/tensorflow/models/blob/master/research/audioset/vggish/mel_features.py
  int frameSize = 400;
  int hopSize = 160;
  int numberBands = 64;
  ::essentia::VectorEx<int> inputShape({batchSize, 1, patchSize, numberBands});

  _frameCutter->configure("frameSize", frameSize, "hopSize", hopSize);

  _vectorRealToTensor->configure("shape", inputShape,
                                 "lastPatchMode", lastPatchMode,
                                 "patchHopSize", patchHopSize);
  
  _configured = true;

  string input = parameter("input").toString();
  string output = parameter("output").toString();
  string isTrainingName = parameter("isTrainingName").toString();

  _tensorToPool->configure("namespace", input);

  _poolToTensor->configure("namespace", output);

  string graphFilename = parameter("graphFilename").toString();
  string savedModel = parameter("savedModel").toString();

  _tensorflowPredict->configure("graphFilename", graphFilename,
                                "savedModel", savedModel,
                                "inputs", ::essentia::VectorEx<string>({input}),
                                "outputs", ::essentia::VectorEx<string>({output}),
                                "isTrainingName", isTrainingName);
}

} // namespace streaming
} // namespace essentia


namespace essentia {
namespace standard {

const char* TensorflowPredictVGGish::name = "TensorflowPredictVGGish";
const char* TensorflowPredictVGGish::category = "Machine Learning";
const char* TensorflowPredictVGGish::description = DOC(
  "This algorithm makes predictions using VGGish-based models.\n"
  "\n"
  "Internally, it uses TensorflowInputVGGish for the input feature extraction "
  "(mel bands). It feeds the model with patches of 96 mel bands frames and "
  "jumps a constant amount of frames determined by `patchHopSize`.\n"
  "\n"
  "By setting the `batchSize` parameter to -1 or 0 the patches are stored to run a single "
  "TensorFlow session at the end of the stream. This allows to take advantage "
  "of parallelization when GPUs are available, but at the same time it can be "
  "memory exhausting for long files.\n"
  "\n"
  "The recommended pipeline is as follows::\n"
  "\n"
  "  MonoLoader(sampleRate=16000) >> TensorflowPredictVGGish\n"
  "\n"
  "Note: This algorithm does not make any check on the input model so it is "
  "the user's responsibility to make sure it is a valid one.\n"
  "\n"
  "References:\n"
  "\n"
  "1. Gemmeke, J. et. al., AudioSet: An ontology and human-labelled dataset "
  "for audio events, ICASSP 2017\n\n"
  "2. Hershey, S. et. al., CNN Architectures for Large-Scale Audio "
  "Classification, ICASSP 2017\n\n"
  "3. Supported models at https://essentia.upf.edu/models/\n\n");


TensorflowPredictVGGish::TensorflowPredictVGGish() {
    declareInput(_signal, "signal", "the input audio signal sampled at 16 kHz");
    declareOutput(_predictions, "predictions", "the output values from the model node named after `output`");

    createInnerNetwork();
  }


TensorflowPredictVGGish::~TensorflowPredictVGGish() {
  delete _network;
}


void TensorflowPredictVGGish::createInnerNetwork() {
  _tensorflowPredictVGGish = streaming::AlgorithmFactory::create("TensorflowPredictVGGish");
  _vectorInput = new streaming::VectorInput<Real>();

  *_vectorInput  >> _tensorflowPredictVGGish->input("signal");
  _tensorflowPredictVGGish->output("predictions") >>  PC(_pool, "predictions");

  _network = new scheduler::Network(_vectorInput);
}


void TensorflowPredictVGGish::configure() {
  _tensorflowPredictVGGish->configure(INHERIT("graphFilename"),
                                      INHERIT("savedModel"),
                                      INHERIT("input"),
                                      INHERIT("output"),
                                      INHERIT("isTrainingName"),
                                      INHERIT("patchHopSize"),
                                      INHERIT("accumulate"),
                                      INHERIT("lastPatchMode"),
                                      INHERIT("patchSize"),
                                      INHERIT("batchSize"));
}


void TensorflowPredictVGGish::compute() {
  const ::essentia::VectorEx<Real>& signal = _signal.get();
  ::essentia::VectorEx<::essentia::VectorEx<Real> >& predictions = _predictions.get();

  if (!signal.size()) {
    throw EssentiaException("TensorflowPredictVGGish: empty input signal");
  }

  _vectorInput->setVector(&signal);

  _network->run();

  try {
    predictions = _pool.value<::essentia::VectorEx<::essentia::VectorEx<Real> > >("predictions");
  }
  catch (EssentiaException&) {
    predictions.clear();
  }

  reset();
}


void TensorflowPredictVGGish::reset() {
  _network->reset();
  _pool.remove("predictions");
}

} // namespace standard
} // namespace essentia
