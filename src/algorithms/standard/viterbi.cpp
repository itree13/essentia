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

#include "viterbi.h"
#include "essentiamath.h"

using namespace std;
using namespace essentia;
using namespace standard;

const char* Viterbi::name = "Viterbi";
const char* Viterbi::category = "Statistics";
const char* Viterbi::description = DOC("This algorithm estimates the most-likely path by Viterbi algorithm. It is used in PitchYinProbabilistiesHMM algorithm.\n"
"\n"
"This Viterbi algorithm returns the most likely path. The internal variable calculation uses double for a better precision.\n"
"\n"
"References:\n"
"  [1] M. Mauch and S. Dixon, \"pYIN: A Fundamental Frequency Estimator\n"
"  Using Probabilistic Threshold Distributions,\" in Proceedings of the\n"
"  IEEE International Conference on Acoustics, Speech, and Signal Processing\n"
"  (ICASSP 2014)Project Report, 2004");

void Viterbi::compute() {

  const ::essentia::VectorEx<::essentia::VectorEx<Real> >& obs = _observationProbabilities.get();
  const ::essentia::VectorEx<Real>& init = _initialization.get();
  const ::essentia::VectorEx<int>& from = _fromIndex.get();
  const ::essentia::VectorEx<int>& to = _toIndex.get();
  const ::essentia::VectorEx<Real>&transProb = _transitionProbabilities.get();

  if (obs.size() == 0 || init.size() == 0 || from.size() == 0 || to.size() == 0 || transProb.size() == 0) {
    throw EssentiaException("Viterbi: one of the inputs has size zero");
  }

  ::essentia::VectorEx<int>& path = _path.get();

  int nState = init.size();
  int nFrame = obs.size();
  
  // check for consistency    
  int nTrans = transProb.size();
  
  // declaring variables, use double for a better precision
  ::essentia::VectorEx<double> delta = ::essentia::VectorEx<double>(nState);
  ::essentia::VectorEx<double> oldDelta = ::essentia::VectorEx<double>(nState);
  ::essentia::VectorEx<::essentia::VectorEx<int> > psi; //  "matrix" of remembered indices of the best transitions
  
  _tempPath.resize(nFrame);

  double deltasum = 0;

  // initialise first frame
  for (int iState = 0; iState < nState; ++iState)
  {
      oldDelta[iState] = init[iState] * obs[0][iState];
      deltasum += oldDelta[iState];
  }

  for (int iState = 0; iState < nState; ++iState)
  {
      oldDelta[iState] /= deltasum; // normalise (scale)
  }

  psi.push_back(::essentia::VectorEx<int>(nState,0));

  // rest of forward step
  for (int iFrame = 1; iFrame < nFrame; ++iFrame)
  {
      deltasum = 0;
      psi.push_back(::essentia::VectorEx<int>(nState,0));

      // calculate best previous state for every current state
      int fromState;
      int toState;
      double currentTransProb;
      double currentValue;
      
      // this is the "sparse" loop
      for (int iTrans = 0; iTrans < nTrans; ++iTrans)
      {
          fromState = from[iTrans];
          toState = to[iTrans];
          currentTransProb = transProb[iTrans];
          
          currentValue = oldDelta[fromState] * currentTransProb;
          if (currentValue > delta[toState])
          {
              delta[toState] = currentValue; // will be multiplied by the right obs later!
              psi[iFrame][toState] = fromState;
          }            
      }
      
      for (int jState = 0; jState < nState; ++jState)
      {
          delta[jState] *= obs[iFrame][jState];
          deltasum += delta[jState];
      }

      if (deltasum > 0)
      {
          for (int iState = 0; iState < nState; ++iState)
          {
              oldDelta[iState] = delta[iState] / deltasum; // normalise (scale)
              delta[iState] = 0;
          }
      } else
      {
          E_WARNING("WARNING: Viterbi has been fed some zero probabilities, at least they become zero at frame " <<  iFrame << " in combination with the model.");
          for (int iState = 0; iState < nState; ++iState)
          {
              oldDelta[iState] = 1.0/nState;
              delta[iState] = 0;
          }
      }
  }

  // initialise backward step, use double for a better precision
  double bestValue = 0;
  for (int iState = 0; iState < nState; ++iState)
  {
      double currentValue = oldDelta[iState];
      if (currentValue > bestValue)
      {
          bestValue = currentValue;            
          _tempPath[nFrame-1] = iState;
      }
  }

  // rest of backward step
  for (int iFrame = nFrame-2; iFrame != -1; --iFrame)
  {
      _tempPath[iFrame] = psi[iFrame+1][_tempPath[iFrame+1]];
  }

  path = _tempPath;
}
