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

#include "pool.h"
#include "algorithmfactory.h"
#include <algorithm> // for std::sort


using namespace std;
using namespace TNT;

namespace essentia {

void Pool::clear() {
  GLOBAL_LOCK;

  _poolReal.clear();
  _poolVectorReal.clear();
  _poolString.clear();
  _poolVectorString.clear();
  _poolArray2DReal.clear();
  _poolTensorReal.clear();
  _poolStereoSample.clear();
  _poolSingleReal.clear();
  _poolSingleString.clear();
  _poolSingleVectorReal.clear();
  _poolSingleVectorString.clear();
  _poolSingleTensorReal.clear();
}

void Pool::checkIntegrity() const {
  // grab locks for all data structures
  GLOBAL_LOCK;

  ::essentia::VectorEx<string> descNames = descriptorNamesNoLocking();
  std::sort(descNames.begin(), descNames.end());

  for (int i=0; i<int(descNames.size()-1); ++i) {
    if (descNames[i] == descNames[i+1]) {
      throw EssentiaException("Pool: there exists a DescriptorName that contains two types of data: ", descNames[i]);
    }
  }

  // TODO: check that there exist no descriptors that have child descriptors and
  //       values
}


// this implementation makes the assumption that the key 'name' only exists in
// one of the sub-pools, as enforced by checkIntegrity
void Pool::remove(const string& name) {

  #define SEARCH_AND_DESTROY(t, tname)                                         \
  {                                                                            \
    MutexLocker lock(mutex##tname);                                            \
    map<string, t >::iterator i = _pool##tname.find(name);                     \
    if (i != _pool##tname.end()) {                                             \
      _pool##tname.erase(i);                                                   \
      return;                                                                  \
    }                                                                          \
  }

  SEARCH_AND_DESTROY(Real, SingleReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<Real>, Real);
  SEARCH_AND_DESTROY(::essentia::VectorEx<Real>, SingleVectorReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<::essentia::VectorEx<Real> >, VectorReal);

  SEARCH_AND_DESTROY(string, SingleString);
  SEARCH_AND_DESTROY(::essentia::VectorEx<string>, String);
  SEARCH_AND_DESTROY(::essentia::VectorEx<string>, SingleVectorString);
  SEARCH_AND_DESTROY(::essentia::VectorEx<::essentia::VectorEx<string> >, VectorString);

  SEARCH_AND_DESTROY(::essentia::VectorEx<TNT::Array2D<Real> >, Array2DReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<Tensor<Real> >, TensorReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<StereoSample>, StereoSample);

  #undef SEARCH_AND_DESTROY
}

void Pool::removeNamespace(const string& ns) {

  #define SEARCH_AND_DESTROY(t, tname)                              \
  {                                                                 \
    MutexLocker lock(mutex##tname);                                 \
    map<string, t >::iterator it = _pool##tname.begin();            \
    int pos = 0;                                                    \
    /*temp iterator that keeps track of the position in the map*/   \
    /*so we dont start from the beginning on the next iteration*/   \
    map<string, t>::iterator tmpIt = _pool##tname.begin();          \
    while (it != _pool##tname.end()) {                              \
      string::size_type strIdx = it->first.find(ns+".");            \
      if (strIdx==0) {                                              \
        _pool##tname.erase(it);                                     \
        if (pos == 0) it = _pool##tname.begin();                    \
        else it = tmpIt;                                            \
      }                                                             \
      else {                                                        \
        tmpIt = it;                                                 \
        ++pos;                                                      \
        ++it;                                                       \
      }                                                             \
    }                                                               \
  }

  SEARCH_AND_DESTROY(Real, SingleReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<Real>, Real);
  SEARCH_AND_DESTROY(::essentia::VectorEx<Real>, SingleVectorReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<::essentia::VectorEx<Real> >, VectorReal);

  SEARCH_AND_DESTROY(string, SingleString);
  SEARCH_AND_DESTROY(::essentia::VectorEx<string>, String);
  SEARCH_AND_DESTROY(::essentia::VectorEx<string>, SingleVectorString);  
  SEARCH_AND_DESTROY(::essentia::VectorEx<::essentia::VectorEx<string> >, VectorString);

  SEARCH_AND_DESTROY(::essentia::VectorEx<Tensor<Real> >, TensorReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<TNT::Array2D<Real> >, Array2DReal);
  SEARCH_AND_DESTROY(::essentia::VectorEx<StereoSample>, StereoSample);

  #undef SEARCH_AND_DESTROY
}


::essentia::VectorEx<string> Pool::descriptorNames() const {
  ::essentia::VectorEx<string> descNames;
  int i=0;

  #define ADD_DESC_NAMES(type, tname)                                          \
  {                                                                            \
    MutexLocker lock(mutex##tname);                                            \
    descNames.resize(descNames.size() + _pool##tname.size());                  \
    for (map<string, type >::const_iterator it = _pool##tname.begin();         \
         it != _pool##tname.end();                                             \
         ++it) {                                                               \
      descNames[i++] = it->first;                                              \
    }                                                                          \
  }

  ADD_DESC_NAMES(Real, SingleReal);
  ADD_DESC_NAMES(::essentia::VectorEx<Real>, Real);
  ADD_DESC_NAMES(::essentia::VectorEx<Real>, SingleVectorReal);
  ADD_DESC_NAMES(::essentia::VectorEx<::essentia::VectorEx<Real> >, VectorReal);
  ADD_DESC_NAMES(string, SingleString);
  ADD_DESC_NAMES(::essentia::VectorEx<string>, String);
  ADD_DESC_NAMES(::essentia::VectorEx<string>, SingleVectorString);  
  ADD_DESC_NAMES(::essentia::VectorEx<::essentia::VectorEx<string> >, VectorString);
  ADD_DESC_NAMES(::essentia::VectorEx<TNT::Array2D<Real> >, Array2DReal);
  ADD_DESC_NAMES(::essentia::VectorEx<Tensor<Real> >, TensorReal);
  ADD_DESC_NAMES(Tensor<Real>, SingleTensorReal);
  ADD_DESC_NAMES(::essentia::VectorEx<StereoSample>, StereoSample);

  #undef ADD_DESC_NAMES

  return descNames;
}

::essentia::VectorEx<string> Pool::descriptorNames(const std::string& ns) const {
  ::essentia::VectorEx<string> descNames;
  #define ADD_DESC_NAMES(type, tname)                            \
  {                                                              \
    MutexLocker lock(mutex##tname);                              \
    map<string, type>::const_iterator it = _pool##tname.begin(); \
    while (it != _pool##tname.end()) {                           \
      if (it->first.find(ns+".") == 0)                           \
        descNames.push_back(it->first);                          \
      ++it;                                                      \
    }                                                            \
  }

  ADD_DESC_NAMES(Real, SingleReal);
  ADD_DESC_NAMES(::essentia::VectorEx<Real>, Real);
  ADD_DESC_NAMES(::essentia::VectorEx<Real>, SingleVectorReal);
  ADD_DESC_NAMES(::essentia::VectorEx<::essentia::VectorEx<Real> >, VectorReal);
  ADD_DESC_NAMES(string, SingleString);
  ADD_DESC_NAMES(::essentia::VectorEx<string>, String);
  ADD_DESC_NAMES(::essentia::VectorEx<string>, SingleVectorString);
  ADD_DESC_NAMES(::essentia::VectorEx<::essentia::VectorEx<string> >, VectorString);
  ADD_DESC_NAMES(::essentia::VectorEx<TNT::Array2D<Real> >, Array2DReal);
  ADD_DESC_NAMES(::essentia::VectorEx<Tensor<Real> >, TensorReal);
  ADD_DESC_NAMES(Tensor<Real>, SingleTensorReal);
  ADD_DESC_NAMES(::essentia::VectorEx<StereoSample>, StereoSample);

  #undef ADD_DESC_NAMES

  return descNames;
}


// WARNING: this function assumes that a GLOBAL_LOCK is already obtained
::essentia::VectorEx<string> Pool::descriptorNamesNoLocking() const {
  ::essentia::VectorEx<string> descNames(_poolReal.size()         +
                           _poolVectorReal.size()   +
                           _poolString.size()       +
                           _poolVectorString.size() +
                           _poolArray2DReal.size()  +
                           _poolTensorReal.size()  +
                           _poolStereoSample.size() +
                           _poolSingleReal.size()   +
                           _poolSingleString.size() +
                           _poolSingleVectorReal.size() + 
                           _poolSingleVectorString.size() +
                           _poolSingleTensorReal.size());
  int i=0;

  #define ADD_DESC_NAMES(type, tname)                                          \
  for (map<string, type >::const_iterator it = _pool##tname.begin();           \
       it != _pool##tname.end();                                               \
       ++it) {                                                                 \
    descNames[i++] = it->first;                                                \
  }

  ADD_DESC_NAMES(Real, SingleReal);
  ADD_DESC_NAMES(::essentia::VectorEx<Real>, Real);
  ADD_DESC_NAMES(::essentia::VectorEx<Real>, SingleVectorReal);
  ADD_DESC_NAMES(::essentia::VectorEx<::essentia::VectorEx<Real> >, VectorReal);
  ADD_DESC_NAMES(string, SingleString);
  ADD_DESC_NAMES(::essentia::VectorEx<string>, String);
  ADD_DESC_NAMES(::essentia::VectorEx<string>, SingleVectorString);
  ADD_DESC_NAMES(::essentia::VectorEx<::essentia::VectorEx<string> >, VectorString);
  ADD_DESC_NAMES(::essentia::VectorEx<TNT::Array2D<Real> >, Array2DReal);
  ADD_DESC_NAMES(::essentia::VectorEx<Tensor<Real> >, TensorReal);
  ADD_DESC_NAMES(::essentia::VectorEx<StereoSample>, StereoSample);


  #undef ADD_DESC_NAMES

  return descNames;
}

void Pool::validateKey(const string& name) {
  ::essentia::VectorEx<std::string> allNames = descriptorNamesNoLocking();
  for (int i=0; i<int(allNames.size()); ++i) {
    /* first check if name already exists in another sub-pool */
    if (name == allNames[i]) {
      throw EssentiaException("Pool: Cannot set/add/merge value to the pool under "
                              "the name '"+name+"' because that name already exists but "
                              "contains a different data type than value");
    }
    /* now check if adding this new key will result in a parent descriptor
     * having a value and child descriptors (there are 2 cases where this can
     * happen)*/
    if ( name.find(allNames[i] + ".") == 0 ) {
      throw EssentiaException("Pool: Cannot set/add/merge value to the pool under the name '"+name+
                              "' because '"+name+"' has a parent descriptor name already in "
                              "the pool (e.g. '"+allNames[i]+"')");
    }

    if ( allNames[i].find(name+".") == 0 ) {
      throw EssentiaException("Pool: Cannot add/set/merge value to the pool under "
                              "the name '"+name+"' because '"+name+"' has child descriptor "
                              "names (e.g. '"+allNames[i]+"')");
    }
  }
}

#define SPECIALIZE_ADD_IMPL(type, tname)                                     \
void Pool::add(const string& name, const type& value, bool validityCheck) {  \
                                                                             \
  /* first check if the pool has ever seen this key before, if it has, we can
   * just add it, if not, we need to run some validation tests */            \
  {                                                                          \
    MutexLocker lock(mutex##tname);                                          \
    if (validityCheck && !isValid(value)) {                                  \
      throw EssentiaException("Pool::add value contains invalid numbers (NaN or inf)");\
    }                                                                        \
    if (_pool##tname.find(name) != _pool##tname.end()) {                     \
      _pool##tname[name].push_back(value);                                   \
      return;                                                                \
    }                                                                        \
  }                                                                          \
  /* validating will require checking all sub-pools, acquire a global lock*/ \
  GLOBAL_LOCK                                                                \
  validateKey(name);                                                         \
  _pool##tname[name].push_back(value);                                       \
}


SPECIALIZE_ADD_IMPL(Real, Real);
SPECIALIZE_ADD_IMPL(::essentia::VectorEx<Real>, VectorReal);
SPECIALIZE_ADD_IMPL(string, String);
SPECIALIZE_ADD_IMPL(::essentia::VectorEx<string>, VectorString);
SPECIALIZE_ADD_IMPL(StereoSample, StereoSample);


void Pool::add(const string& name, const Tensor<Real>& value, bool validityCheck) {
  /* first check if the pool has ever seen this key before, if it has, we can
   * just add it, if not, we need to run some validation tests */
  {
    MutexLocker lock(mutexTensorReal);
    if (validityCheck && !isValid(value)) {
      throw EssentiaException("Pool::add tensor contains invalid numbers (NaN or inf)");
    }
    if (_poolTensorReal.find(name) != _poolTensorReal.end()) {
      _poolTensorReal[name].push_back(Tensor<Real>(value));
      return;
    }
  }
  GLOBAL_LOCK
  validateKey(name);
  _poolTensorReal[name].push_back(Tensor<Real>(value));
}

// special add for Array2d<Real>
// Array2D needs a special add that cannot be implemented in the macro because
// we need to call the function copy(), or otherwise we only get references
void Pool::add(const string& name, const Array2D<Real>& value, bool validityCheck) {
  /* first check if the pool has ever seen this key before, if it has, we can
   * just add it, if not, we need to run some validation tests */
  {
    MutexLocker lock(mutexArray2DReal);
    if (validityCheck && !isValid(value)) {
      throw EssentiaException("Pool::add array contains invalid numbers (NaN or inf)");
    }
    if (_poolArray2DReal.find(name) != _poolArray2DReal.end()) {
      _poolArray2DReal[name].push_back(value.copy());
      return;
    }
  }
  GLOBAL_LOCK
  validateKey(name);
  _poolArray2DReal[name].push_back(value.copy());
}

#define SPECIALIZE_SET_IMPL(type, tname)                                     \
void Pool::set(const string& name, const type& value, bool validityCheck) {  \
                                                                             \
  /* first check if the pool has ever seen this key before, if it has, we can
   * just set it, if not, we need to run some validation tests */            \
  {                                                                          \
    MutexLocker lock(mutexSingle##tname);                                    \
    if (validityCheck && !isValid(value)) {                                  \
      throw EssentiaException("Pool::set value contains invalid numbers (NaN or inf)");\
    }                                                                        \
    if (_poolSingle##tname.find(name) != _poolSingle##tname.end()) {         \
      _poolSingle##tname[name] = value;                                      \
      return;                                                                \
    }                                                                        \
  }                                                                          \
  GLOBAL_LOCK                                                                \
  validateKey(name);                                                         \
  _poolSingle##tname[name] = value;                                          \
}

SPECIALIZE_SET_IMPL(Real, Real)
SPECIALIZE_SET_IMPL(string, String)
SPECIALIZE_SET_IMPL(::essentia::VectorEx<Real>, VectorReal)
SPECIALIZE_SET_IMPL(::essentia::VectorEx<string>, VectorString)

// special set for Tensor<Real>
void Pool::set(const string& name, const Tensor<Real>& value, bool validityCheck) {
  /* first check if the pool has ever seen this key before, if it has, we can
   * just add it, if not, we need to run some validation tests */
  {
    MutexLocker lock(mutexSingleTensorReal);
    if (validityCheck && !isValid(value)) {
      throw EssentiaException("Pool::set tensor contains invalid numbers (NaN or inf)");
    }
    if (_poolSingleTensorReal.find(name) != _poolSingleTensorReal.end()) {
      _poolSingleTensorReal[name].resize(value.dimensions());
      _poolSingleTensorReal[name] = value;
      return;
    }
  }
  GLOBAL_LOCK
  validateKey(name);

  _poolSingleTensorReal[name].resize(value.dimensions());
  _poolSingleTensorReal[name] = value;
}


void Pool::merge(Pool& p, const string& mergeType) {

  #define MERGE_POOL(t, tname) {                                                     \
    ::essentia::VectorEx<string> descNames;                                                        \
    descNames.reserve(p.get##tname##Pool().size());                                  \
    {                                                                                \
      MutexLocker lock(p.mutex##tname);                                              \
      for (map<string, ::essentia::VectorEx<t> >::const_iterator it = p.get##tname##Pool().begin();\
           it != p.get##tname##Pool().end();                                         \
           ++it) {                                                                   \
        descNames.push_back(it->first);                                              \
      }                                                                              \
    }                                                                                \
    for (int i=0; i < int(descNames.size()); ++i) {                                  \
      merge(descNames[i], p.value<::essentia::VectorEx<t> >(descNames[i]), mergeType);             \
    }                                                                                \
  }

  #define MERGE_SINGLE_POOL(t, tname) {                                      \
    ::essentia::VectorEx<string> descNames;                                                \
    descNames.reserve(p.get##tname##Pool().size());                          \
    {                                                                        \
      MutexLocker lock(p.mutex##tname);                                    \
      for (map<string, t>::const_iterator it = p.get##tname##Pool().begin(); \
           it != p.get##tname##Pool().end();                                 \
           ++it) {                                                           \
        descNames.push_back(it->first);                                      \
      }                                                                      \
    }                                                                        \
    for (int i=0; i < int(descNames.size()); ++i) {                          \
      mergeSingle(descNames[i], p.value<t>(descNames[i]), mergeType);        \
    }                                                                        \
  }

  // single value:
  MERGE_SINGLE_POOL(Real, SingleReal);
  MERGE_SINGLE_POOL(string, SingleString);
  MERGE_SINGLE_POOL(::essentia::VectorEx<Real>, SingleVectorReal);
  MERGE_SINGLE_POOL(::essentia::VectorEx<string>, SingleVectorString);
  MERGE_SINGLE_POOL(Tensor<Real>, SingleTensorReal);

  // multiple value:
  MERGE_POOL(Real, Real);
  MERGE_POOL(string, String);
  MERGE_POOL(::essentia::VectorEx<Real>, VectorReal);
  MERGE_POOL(::essentia::VectorEx<string>, VectorString);
  MERGE_POOL(StereoSample, StereoSample);
  MERGE_POOL(TNT::Array2D<Real>, Array2DReal);
  MERGE_POOL(Tensor<Real>, TensorReal);

  #undef MERGE_SINGLE_POOL
  #undef MERGE_POOL
}

#define SPECIALIZE_MERGE_IMPL(type, tname)                                                             \
void Pool::merge(const string& name, const ::essentia::VectorEx<type>& value, const string& mergeType) {             \
  if (value.empty()) return;                                                                           \
                                                                                                       \
  /* first check if the pool has ever seen this key before, if it has, we can
   * just add it, if not, we need to run some validation tests */                                      \
  {                                                                                                    \
    MutexLocker lock(mutex##tname);                                                                    \
    map<string, ::essentia::VectorEx<type> >::iterator it = _pool##tname.find(name);                                 \
    if (it != _pool##tname.end()) {                                                                    \
      if (mergeType == "") {                                                                           \
        throw EssentiaException("Pool::merge, cannot merge descriptor names with the same name:" +     \
                                name + " unless a merge type (\"append\", \"replace\" or " +           \
                                "\"interleave\") is specified");                                       \
      }                                                                                                \
      else if (mergeType=="append") {                                                                  \
        _pool##tname[name].reserve(_pool##tname[name].size()+value.size());                            \
        for(int i=0; i<int(value.size()); i++) {                                                       \
          _pool##tname[name].push_back(value[i]);                                                      \
        }                                                                                              \
      }                                                                                                \
      else if (mergeType == "replace") {                                                               \
        _pool##tname.erase(it);                                                                        \
        _pool##tname.insert(make_pair(name, value));                                                   \
      }                                                                                                \
      else if (mergeType=="interleave") {                                                              \
        if (value.size() != _pool##tname[name].size()) {                                               \
          throw EssentiaException("Pool::merge, cannot interleave descriptors with different sizes :", name);\
        }                                                                                              \
        ::essentia::VectorEx<type> temp = _pool##tname[name];                                                        \
        _pool##tname.erase(it);\
        _pool##tname[name].push_back(temp[0]);                                                         \
        _pool##tname[name].push_back(value[0]);                                                        \
        _pool##tname[name].reserve(2*temp.size());                                                     \
        for (int i=1; i<(int)temp.size(); i++) {                                                       \
          _pool##tname[name].push_back(temp[i]);                                                       \
          _pool##tname[name].push_back(value[i]);                                                      \
        }                                                                                              \
        return;\
      }                                                                                                \
      else {                                                                                           \
        throw EssentiaException("Pool::merge, unknown merge type: ", mergeType);                       \
      }                                                                                                \
      return;                                                                                          \
    }                                                                                                  \
  }                                                                                                    \
  GLOBAL_LOCK                                                                                          \
  validateKey(name);                                                                                   \
  _pool##tname[name].push_back(value[0]);                                                              \
  _pool##tname[name].reserve(value.size());                                                            \
  for (int i=1; i<(int)value.size(); ++i) {                                                            \
    _pool##tname[name].push_back(value[i]);                                                            \
  }                                                                                                    \
}

SPECIALIZE_MERGE_IMPL(Real, Real);
SPECIALIZE_MERGE_IMPL(::essentia::VectorEx<Real>, VectorReal);
SPECIALIZE_MERGE_IMPL(string, String);
SPECIALIZE_MERGE_IMPL(::essentia::VectorEx<string>, VectorString);
SPECIALIZE_MERGE_IMPL(StereoSample, StereoSample);
SPECIALIZE_MERGE_IMPL(Tensor<Real>, TensorReal);

#define SPECIALIZE_MERGE_SINGLE_IMPL(type, tname)                                                      \
void Pool::mergeSingle(const string& name, const type& value, const string& mergeType) {               \
                                                                                                       \
  /* first check if the pool has ever seen this key before, if it has, we can
   * just add it, if not, we need to run some validation tests */                                      \
  {                                                                                                    \
    MutexLocker lock(mutexSingle##tname);                                                              \
    map<string, type>::iterator it = _poolSingle##tname.find(name);                                    \
    if (it != _poolSingle##tname.end()) {                                                              \
      if (mergeType == "replace") {                                                                    \
        _poolSingle##tname.erase(it);                                                                  \
        _poolSingle##tname.insert(make_pair(name, value));                                             \
      }                                                                                                \
      else {                                                                                           \
        throw EssentiaException("Pool::mergeSingle, values for single value descriptors can only be"   \
                                " replaced and neither appended nor interleaved. Consider replacing "  \
                                + name + " with the new value or pool::remove + pool::add");           \
      }                                                                                                \
      return;                                                                                          \
    }                                                                                                  \
  }                                                                                                    \
  GLOBAL_LOCK                                                                                          \
  validateKey(name);                                                                                   \
  _poolSingle##tname.insert(make_pair(name, value));                                                   \
}

SPECIALIZE_MERGE_SINGLE_IMPL(Real, Real)
SPECIALIZE_MERGE_SINGLE_IMPL(string, String)
SPECIALIZE_MERGE_SINGLE_IMPL(::essentia::VectorEx<Real>, VectorReal)
SPECIALIZE_MERGE_SINGLE_IMPL(::essentia::VectorEx<string>, VectorString)
SPECIALIZE_MERGE_SINGLE_IMPL(Tensor<Real>, TensorReal)


void Pool::merge(const string& name, const ::essentia::VectorEx<Array2D<Real> >& value, const string& mergeType) {
  /* first check if the pool has ever seen this key before, if it has, we can
   * just add it, if not, we need to run some validation tests */
  {
    MutexLocker lock(mutexArray2DReal);
    map<string, ::essentia::VectorEx<Array2D<Real> > >::iterator it = _poolArray2DReal.find(name);
    if (it != _poolArray2DReal.end()) {
      if (mergeType == "") {
        throw EssentiaException("Pool::merge, cannot merge descriptor names with the same name:" +
                                name + " unless a merge type (\"append\", \"replace\" or " +
                                "\"interleave\") is specified");
      }
      else if (mergeType=="append") {
        _poolArray2DReal[name].reserve(_poolArray2DReal[name].size()+value.size());
        for(int i=0; i<int(value.size()); i++) {
          _poolArray2DReal[name].push_back(value[i].copy());
        }
      }
      else if (mergeType == "replace") {
        _poolArray2DReal.erase(it);
        _poolArray2DReal[name].reserve(value.size());
        for(int i=0; i<int(value.size()); i++) {
          _poolArray2DReal[name].push_back(value[i].copy());
        }
      }
      else if (mergeType=="interleave") {
        if (value.size() != _poolArray2DReal[name].size()) {
          throw EssentiaException("Pool::merge, cannot interleave descriptors with different sizes :", name);
        }
        ::essentia::VectorEx<Array2D<Real> > temp = _poolArray2DReal[name];
        _poolArray2DReal.erase(it);
        _poolArray2DReal[name].push_back(temp[0].copy());
        _poolArray2DReal[name].push_back(value[0].copy());
        _poolArray2DReal[name].reserve(2*temp.size());
        for (int i=1; i<(int)temp.size(); i++) {
          _poolArray2DReal[name].push_back(temp[i].copy());
          _poolArray2DReal[name].push_back(value[i].copy());
        }
        return;
      }
      else {
        throw EssentiaException("Pool::merge, unknown merge type: ", mergeType);
      }
      return;
    }
  }
  GLOBAL_LOCK
  validateKey(name);
  _poolArray2DReal[name].push_back(value[0].copy());
  _poolArray2DReal[name].reserve(value.size());
  for (int i=1; i<(int)value.size(); ++i) {
    _poolArray2DReal[name].push_back(value[i].copy());
  }
}

bool Pool::isSingleValue(const string& name) {
  #define SEARCH_SINGLE(t, tname)                                              \
  {                                                                            \
    MutexLocker lock(mutex##tname);                                            \
    map<string, t >::iterator i = _pool##tname.find(name);                     \
    if (i != _pool##tname.end()) {                                             \
      return true;                                                             \
    }                                                                          \
  }
  SEARCH_SINGLE(Real, SingleReal);
  SEARCH_SINGLE(::essentia::VectorEx<Real>, SingleVectorReal);
  SEARCH_SINGLE(string, SingleString);
  SEARCH_SINGLE(::essentia::VectorEx<string>, SingleVectorString);
  SEARCH_SINGLE(Tensor<Real>, SingleTensorReal);

  #undef SEARCH_SINGLE
  return false;
}

} // namespace essentia
