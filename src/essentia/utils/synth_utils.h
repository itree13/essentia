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


#ifndef ESSENTIA_SYNTH_UTILS_H
#define ESSENTIA_SYNTH_UTILS_H

#include <essentia/algorithmfactory.h>


namespace essentia{


void scaleAudioVector(::essentia::VectorEx<Real> &buffer, const Real scale);
//void mixAudioVectors(const ::essentia::VectorEx<Real> ina, const ::essentia::VectorEx<Real> inb, const Real gaina, const Real gainb, ::essentia::VectorEx<Real> &out);
void cleaningSineTracks(::essentia::VectorEx< ::essentia::VectorEx<Real> >&freqsTotal, const int minFrames);
void genSpecSines(::essentia::VectorEx<Real> iploc, ::essentia::VectorEx<Real> ipmag, ::essentia::VectorEx<Real> ipphase, ::essentia::VectorEx<std::complex<Real> > &outfft, const int fftSize);
void initializeFFT(::essentia::VectorEx<std::complex<Real> >&fft, int sizeFFT);

} // namespace essentia

#endif // ESSENTIA_SYNTH_UTILS_H
