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

#ifndef ESSENTIA_ROGUEVECTOR_H
#define ESSENTIA_ROGUEVECTOR_H

#include <vector>
#include "types.h"

namespace essentia {


template <typename T>
class RogueVector : public VectorEx<T> {
  T* data_ = nullptr;
  size_t size_ = 0;
 public:
  RogueVector(T* tab = 0, size_t size = 0) {
    VectorEx<T>::setReferenceData(tab, size);
  }

  RogueVector(uint size, T value) : VectorEx<T>(size, value) {}

  RogueVector(const RogueVector<T>& v) : VectorEx<T>() {
    VectorEx<T>::setReferenceData(const_cast<T*>(v.data()), v.size());
  }

  ~RogueVector() {
  }

  // Those need to be implementation specific
  void setData(T* data) { data_ = data; VectorEx<T>::setReferenceData(data_, size_);}
  void setSize(size_t size) { size_ = size; VectorEx<T>::setReferenceData(data_, size_); }
};

// // Clang/LLVM implementation
// #if defined(__clang__) || defined(__EMSCRIPTEN__)

// // TODO: this is a big hack that relies on clang/libcpp not changing the memory
// //       layout of the VectorEx (very dangerous, but works for now...)

// template <typename T>
// void RogueVector<T>::setData(T* data) { *reinterpret_cast<T**>(this) = data; }

// template <typename T>
// void RogueVector<T>::setSize(size_t size) {
//     T** start = reinterpret_cast<T**>(this);
//     *(start+1) = *start + size;
//     *(start+2) = *start + size;
// }

// // Linux implementation
// #elif defined(OS_LINUX) || defined(OS_MINGW)

// template <typename T>
// void RogueVector<T>::setData(T* data) { this->_M_impl._M_start = data; }

// template <typename T>
// void RogueVector<T>::setSize(size_t size) {
//   this->_M_impl._M_finish = this->_M_impl._M_start + size;
//   this->_M_impl._M_end_of_storage = this->_M_impl._M_start + size;
// }

// // Windows implementation
// #elif defined(OS_WIN32)

// template <typename T>
// void RogueVector<T>::setData(T* data) {
//   this->_Myfirst() = data;
// }

// template <typename T>
// void RogueVector<T>::setSize(size_t size) {
//   this->_Mylast() = this->_Myfirst() + size;
//   this->_Myend() = this->_Myfirst() + size;
// }

// #endif

} // namespace essentia

#endif // ESSENTIA_ROGUEVECTOR_H
