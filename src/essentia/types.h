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

#ifndef ESSENTIA_TYPES_H
#define ESSENTIA_TYPES_H

#define NOMINMAX

#include <map>
#include <vector>
#include <cctype>
#include <cassert>
#include <sstream>
#include <typeinfo>
#include <string.h>
#include "config.h"
#include <unsupported/Eigen/CXX11/Tensor>



// fixed-size int types

#ifndef OS_WIN32

#include <inttypes.h>

typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;

typedef unsigned int uint;

#else // OS_WIN32

typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef __int16 sint16;
typedef __int32 sint32;
typedef __int64 sint64;

#endif // OS_WIN32

namespace essentia {
  
template<typename T>
class array_view {
  T* ptr_;
  std::size_t len_;
public:
  array_view() : ptr_(nullptr), len_(0) {}
  array_view(T* ptr, std::size_t len) noexcept: ptr_{ptr}, len_{len} {}

  T& operator[](size_t i) noexcept { return ptr_[i]; }
  const T& operator[](size_t i) const noexcept { return ptr_[i]; }
  auto size() const noexcept { return len_; }
  T* data() noexcept { return ptr_; }
  const T* data() const noexcept { return ptr_; }

  auto begin() noexcept { return ptr_; }
  auto end() noexcept { return ptr_ + len_; }
  const auto begin() const noexcept { return ptr_; }
  const auto end() const noexcept { return ptr_ + len_; }
  void clear() { ptr_ = nullptr; len_ = 0; }
};

template <typename T, typename VEC_T>
class VectorExT {
public:
  VectorExT() {}
  VectorExT(size_t size) : vec_(size) {}
  VectorExT(size_t count, const T& val) : vec_(count, val) {}
  VectorExT(std::initializer_list<T> _Ilist) : vec_(_Ilist) {}
  template <class InputIterator>
  VectorExT(InputIterator first, InputIterator last) : vec_(first, last) {}

  typedef size_t size_type;
  typedef T value_type;

  template <class VEC_ITERATOR>
  struct Iterator {   
    typedef T value_type;
    typedef value_type& reference;
		typedef value_type* pointer;
		typedef std::bidirectional_iterator_tag iterator_category;
		typedef std::ptrdiff_t difference_type;

    VEC_ITERATOR itr;
    T* view_pos = nullptr;

		reference operator *() {
			return view_pos ? *view_pos : (reference)*itr;
		}

		const value_type& operator *() const {
			return view_pos ? *view_pos : (const value_type&)*itr;
		}

		pointer operator ->() {
      return view_pos ? view_pos : (pointer)&(*itr);
		}

		/// Dereference iterator, returns const InputIteratorHelper or OutputIteratorHelper wrapper.
		const value_type *operator ->() const {
      return view_pos ? view_pos : (const value_type *)&(*itr);
		}

		Iterator operator ++() {
      if (view_pos)
        ++view_pos;
      else
        ++itr;
			return *this;
		}
  
    Iterator operator ++(int) {
      Iterator old = *this;
      if (view_pos)
        ++view_pos;
      else
        ++itr;
			return old;
		}
    
		Iterator operator --() {
      if (view_pos)
        --view_pos;
      else
        --itr;
			return *this;
		}

		bool operator ==(const Iterator &rhs) const {
			return (view_pos == rhs.view_pos && itr == rhs.itr);
		}

		bool operator !=(const Iterator &rhs) const {
			return !(view_pos == rhs.view_pos && itr == rhs.itr);
		}

    difference_type operator-(const Iterator& rhs) const {
      if (view_pos)
        return view_pos - rhs.view_pos;
      return itr - rhs.itr;
    }

    Iterator operator + (int d) {
      Iterator newitr;
      if (view_pos)
        newitr.view_pos = view_pos + d;
      else
        newitr.itr = itr + d;
      return newitr;
    }

    Iterator operator - (int d) {
      Iterator newitr;
      if (view_pos)
        newitr.view_pos = view_pos - d;
      else
        newitr.itr = itr - d;
      return newitr;
    }

    bool operator < (const Iterator& rhs) const {
      return view_pos ? view_pos < rhs.view_pos : itr < rhs.itr;
    }
  };

  typedef Iterator<typename ::std::vector<T>::iterator> iterator;
  typedef Iterator<typename ::std::vector<T>::const_iterator> const_iterator;

  void setReferenceData(T* data, size_t size) {
    view_ = array_view<T>(data, size);
    vec_.clear();
  }

  void push_back(const T& v) {
    make_vector().push_back(v);
  }

  T* data() {
    return (view_.size()) ? view_.data() : (T*)vec_.data();
  }

  const T* data() const {
    return (view_.size()) ? view_.data() : (const T*)vec_.data();
  }

  size_t size() const {
    return (view_.size()) ? view_.size() : vec_.size();
  }

  T& operator[](size_t i) {
    return (view_.size()) ? view_[i] : (T&)vec_[i];
  }

  const T& operator[](size_t i) const {
    return (view_.size()) ? view_[i] : (const T&)vec_[i];
  }

  T& at(size_t i) {
    return (view_.size()) ? view_[i] : (T&)vec_[i];
  }

  const T& at(size_t i) const {
    return (view_.size()) ? view_[i] : (const T&)vec_[i];
  }

  void resize(size_t new_size) {
    if (new_size <= view_.size()) {
      view_ = array_view<T>(view_.data(), new_size);
    } else {
      make_vector().resize(new_size);
    }
  }
  
  void resize (size_type n, const value_type& val) {
    clear();
    vec_.resize(n, val);
  }

  bool empty() const {
    return view_.size() == 0 && vec_.empty();
  }

  void clear() {
    view_.clear();
    vec_.clear();
  }

  iterator begin() {
    iterator it;
    it.view_pos = view_.begin();
    it.itr = vec_.begin();
    return it;
  }

  iterator end() {
    iterator it;
    it.view_pos = view_.size() ? view_.end() : nullptr;
    it.itr = vec_.end();
    return it;
  }

  const_iterator begin() const {
    const_iterator it;
    it.view_pos = view_.begin();
    it.itr = vec_.begin();
    return it;
  }

  const_iterator end() const {
    const_iterator it;
    it.view_pos = view_.size() ? view_.end() : nullptr;
    it.itr = vec_.end();
    return it;
  }

  T& front() {
    return *begin();
  }
  
  const T& front() const {
    return *begin();
  }

  T& back() {
    return (view_.size()) ? *(view_.begin() + view_.size() - 1) : (const T&)vec_.back();
  }
  
  const T& back() const {
    return (view_.size()) ? *(view_.begin() + view_.size() - 1) : (const T&)vec_.back();
  }

  void erase(iterator it) {
    if (view_.size()) {
      vec_.reserve(view_.size() - 1);
      vec_.assign(view_.begin(), it.view_pos);
      vec_.insert(vec_.end(), it.view_pos + 1, view_.end());
      view_.clear();
    } else {
      vec_.erase(it.itr);
    }
  }

  void reserve(size_t sz) {
    vec_.reserve(sz);
  }

  void assign(size_type n, const value_type& val) {
    make_vector().assign(n, val);
  }

  void assign(std::initializer_list<value_type> il) {
    make_vector().assign(il);
  }

  template <class Y>
  void assign(Iterator<Y> first, Iterator<Y> last) {
    make_vector().assign(first, last);
  }

  template <class Y>
  void insert(iterator position, Iterator<Y> first, Iterator<Y> last) {  
     if (view_.size()) {
      vec_.assign(view_.begin(), position.view_pos);
      vec_.insert(vec_.end(), first, last);
      vec_.insert(vec_.end(), position.view_pos + 1, view_.end());
      view_.clear();
    } else {
      vec_.insert(position.itr, first, last);
    }
  }

private:
  ::std::vector<T>& make_vector() {
    if (view_.size()) {
      vec_.assign(view_.begin(), view_.end());
    }
    view_.clear();
    return vec_;
  }

private:
  ::std::vector<VEC_T> vec_;
  array_view<T> view_;
};

template<class T>
struct VectorEx : public VectorExT<T, T> {
  using VectorExT<T, T>::VectorExT;
};
template<>
struct VectorEx<bool> : public VectorExT<bool, int> {
  using VectorExT<bool, int>::VectorExT;
};

}

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
namespace std {
    template <class _Arg, class _Result>
    struct unary_function { // base class for unary functions
        using argument_type = _Arg;
        using result_type = _Result;
    };

    template <class _Arg1, class _Arg2, class _Result>
    struct binary_function { // base class for binary functions
        using first_argument_type = _Arg1;
        using second_argument_type = _Arg2;
        using result_type = _Result;
    };
}
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#include "debugging.h"
#include "streamutil.h"

namespace essentia {

/**
 * The main typedef for real numbers.
 */
typedef float Real;


/**
 * Exception class for Essentia. It has a whole slew of different constructors
 * to make it as easy as possible to throw an exception with a descriptive
 * message.
 */
class EssentiaException : public std::exception {

 public:
  EssentiaException(const char* msg) : exception(), _msg(msg) {}
  EssentiaException(const std::string& msg) : exception(), _msg(msg) {}
  EssentiaException(const std::ostringstream& msg) : exception(), _msg(msg.str()) {}

  template <typename T, typename U>
  EssentiaException(const T& a, const U& b) : exception() {
    std::ostringstream oss; oss << a << b; _msg = oss.str();
  }

  template <typename T, typename U, typename V>
  EssentiaException(const T& a, const U& b, const V& c) : exception() {
    std::ostringstream oss; oss << a << b << c; _msg = oss.str();
  }

  template <typename T, typename U, typename V, typename W>
  EssentiaException(const T& a, const U& b, const V& c, const W& d) : exception() {
    std::ostringstream oss; oss << a << b << c << d; _msg = oss.str();
  }

  virtual ~EssentiaException() throw() {}
  virtual const char* what() const throw() { return _msg.c_str(); }

 protected:
  std::string _msg;

};


/**
 * Case-insensitive compare function for characters.
 */
inline bool case_insensitive_char_cmp(char a, char b) {
  return std::tolower(a) < std::tolower(b);
}

/**
 * Function object for comparing two strings in a case-insensitive manner.
 */
struct case_insensitive_str_cmp
  : public std::binary_function<const std::string&, const std::string&, bool> {
  bool operator()(const std::string& str1, const std::string& str2) const {
    return std::lexicographical_compare(str1.begin(), str1.end(),
                                        str2.begin(), str2.end(),
                                        case_insensitive_char_cmp);
  }
};


template <class T>
class OrderedMap : public ::essentia::VectorEx<std::pair<std::string, T*> > {
 public:
  typedef typename ::essentia::VectorEx<std::pair<std::string, T*> > BaseClass;

  int size() const { return (int)BaseClass::size(); }

  const std::pair<std::string, T*>& operator[](uint idx) const {
    return BaseClass::operator[](idx);
  }

  std::pair<std::string, T*>& operator[](uint idx) {
    return BaseClass::operator[](idx);
  }

  const T* operator[](const char* str) const {
    const uint size = this->size();
    for (uint i=0; i<size; i++) {
      if (charptr_cmp((*this)[i].first.c_str(), str) == 0) {
        return (*this)[i].second;
      }
    }

    throw EssentiaException("Value not found: '", str, "'\nAvailable keys: ", keys());
  }

  T* operator[](const char* str) {
    return const_cast<T*>(const_cast<const OrderedMap<T>*>(this)->operator[](str));
  }

  const T* operator[](const std::string& str) const {
    return operator[](str.c_str());
  }

  T* operator[](const std::string& str) {
    return operator[](str.c_str());
  }

  ::essentia::VectorEx<std::string> keys() const {
    ::essentia::VectorEx<std::string> result(this->size());
    for (int i=0; i<this->size(); i++) {
      result[i] = this->at(i).first;
    }
    return result;
  }

  void insert(const std::string& key, T* value) {
    this->push_back(std::make_pair(key, value));
  }
};



/**
 * Special version of a std::map that allows us to use the [] operator on a
 * const object. In this case, if the key is found, it returns the associated
 * value, otherwise it throws an exception.
 * If not used on a constant object, it also throws an exception if the key is
 * not found, in order to have a consistent behavior.
 * It also redefines the insert() method to be more convenient.
 */
template <typename KeyType, typename ValueType, typename Compare = std::less<KeyType> >
class EssentiaMap : public std::map<KeyType, ValueType, Compare> {

 public:
  typedef std::map<KeyType, ValueType, Compare> BaseClass;

  /**
   * Classic version of the map accessor.
   */
  ValueType& operator[](const KeyType& key) {
    typename BaseClass::iterator it = this->find(key);
    if (it == BaseClass::end()) {
      throw EssentiaException("Value not found: '", key, "'\nAvailable keys: ", keys());
    }
    return it->second;
  }

  /**
   * New version that can be called on a constant object and instead of
   * creating a new null object and inserting it in the map just throws an
   * exception.
   */
  const ValueType& operator[](const KeyType& key) const {
    typename BaseClass::const_iterator it = this->find(key);
    if (it == BaseClass::end()) {
      throw EssentiaException("Value not found: '", key, "'\nAvailable keys: ", keys());
    }
    return it->second;
  }

  std::pair<typename BaseClass::iterator, bool> insert(const KeyType& key, const ValueType& value) {
    return BaseClass::insert(std::make_pair(key, value));
  }

  ::essentia::VectorEx<std::string> keys() const {
    ::essentia::VectorEx<std::string> result;
    result.reserve(BaseClass::size());
    std::ostringstream stream;
    typename BaseClass::const_iterator it = this->begin();
    for (; it != this->end(); ++it) {
      stream.str("");
      stream << it->first;
      result.push_back(stream.str());
    }
    return result;
  }

};


/**
 * Type of map used for storing the description of the various fields.
 */
typedef EssentiaMap<std::string, std::string, string_cmp> DescriptionMap;




/**
 * Return @c true if the two given types are the same.
 */
#if SAFE_TYPE_COMPARISONS

// comparison of the type is done using the name() method, because type_info
// are not shared between different linking units.
inline bool sameType(const std::type_info& t1, const std::type_info& t2) {
  return strcmp(t1.name(), t2.name()) == 0;
}

#else // SAFE_TYPE_COMPARISONS

inline bool sameType(const std::type_info& t1, const std::type_info& t2) {
  return t1 == t2;
}

#endif // SAFE_TYPE_COMPARISONS


// defined in src/base/essentia.cpp
std::string nameOfType(const std::type_info& type);

/**
 * Subclasses of this interface have the ability to check their type against
 * another one.
 */
class TypeProxy {
 protected:
  std::string _name;

 public:
  TypeProxy() {}
  TypeProxy(const std::string& name) : _name(name) {}

  virtual ~TypeProxy() {}

  const std::string& name() const { return _name; }
  void setName(const std::string& name) { _name = name; }

  inline void checkType(const std::type_info& received,
                        const std::type_info& expected) const {
    if (!sameType(received, expected)) {
      std::ostringstream msg;
      msg << "Error when checking types. Expected: " << nameOfType(expected)
          << ", received: " << nameOfType(received);
      throw EssentiaException(msg);
    }
  }

  template <typename Type>
  void checkType() const {
    checkType(typeid(Type), typeInfo());
  }

  void checkSameTypeAs(const TypeProxy& obj) const {
    checkType(obj.typeInfo(), typeInfo());
  }

  void checkVectorSameTypeAs(const TypeProxy& obj) const {
    checkType(obj.vectorTypeInfo(), typeInfo());
  }

  virtual const std::type_info& typeInfo() const = 0;
  virtual const std::type_info& vectorTypeInfo() const = 0;
};

/**
 * Commodity function to return the name of the type used in a TypeProxy.
 */
inline std::string nameOfType(const TypeProxy& tproxy) {
  return nameOfType(tproxy.typeInfo());
}

/**
 * Commodity function to compare two TypeProxy using their respective type.
 */
inline bool sameType(const TypeProxy& lhs, const TypeProxy& rhs) {
  return sameType(lhs.typeInfo(), rhs.typeInfo());
}

/**
 * Use this macro in classes that derive from TypeProxy to automatically
 * make them type-aware
 */
#define USE_TYPE_INFO(TokenType)                           \
  virtual const std::type_info& typeInfo() const {         \
    return typeid(TokenType);                              \
  }                                                        \
  virtual const std::type_info& vectorTypeInfo() const {   \
    return typeid(::essentia::VectorEx<TokenType>);                 \
  }


/**
 * typedef used for identifying sinks for a given source.
 */
typedef int ReaderID;

/**
 * Type used to represent a mono audio sample.
 */
typedef Real AudioSample;

template <typename T>
class Tuple2 {
 public:
  T first;
  T second;

  const T& left() const { return first; }
  const T& right() const { return second; }
  const T& x() const { return first; }
  const T& y() const { return second; }

  T& left() { return first; }
  T& right() { return second; }
  T& x() { return first; }
  T& y() { return second; }
};

/**
 * Type used to represent a stereo sample.
 */
typedef Tuple2<Real> StereoSample;

/**
 * Macro used to define the rank (number of dimensions) of Essentia::Tensor.
 */
#define TENSORRANK 4

/**
 * Alias for Eigen::Tensor.
 * Store data in a rowMajor fashion to fit Tensorflow's behavior.
 * https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/framework/tensor_types.h
 */
template<typename T>
using Tensor = Eigen::Tensor<T, TENSORRANK, Eigen::RowMajor>;

/**
 * Alias for Eigen::TensorMap.
 */
template<typename T>
using TensorMap = Eigen::TensorMap<Tensor<T>, 0>;

/**
 * Alias for a 0-dimensional Eigen::Tensor.
 */
using TensorScalar = Eigen::Tensor<Real, 0, Eigen::RowMajor>;

/**
 * Alias for a 1-dimensional Eigen::Tensor.
 */
using Tensor1D = Eigen::Tensor<Real, 1, Eigen::RowMajor>;

/**
 * Alias for a 2-dimensional Eigen::Tensor.
 */
using Tensor2D = Eigen::Tensor<Real, 2, Eigen::RowMajor>;

/**
 * Alias for a 3-dimensional Eigen::Tensor.
 */
using Tensor3D = Eigen::Tensor<Real, 3, Eigen::RowMajor>;


namespace streaming {

/**
 * This class is used to retrieve information about a buffer, such as its size,
 * phantom size, etc...
 * It is also used to pass this information to a buffer so it can resize itself.
 */
class BufferInfo {
 public:
  int size;
  int maxContiguousElements;

  BufferInfo(int size = 0, int contiguous = 0) :
    size(size), maxContiguousElements(contiguous) {}
};

namespace BufferUsage {

/**
 * Usage types for buffer which serve as preset sizes. The user can then
 * only specify for which kind of processing he will use this buffer and doesn't
 * need to know the specifics of the buffer implementation.
 */
enum BufferUsageType {
  forSingleFrames,
  forMultipleFrames,
  forAudioStream,
  forLargeAudioStream
};

} // namespace BufferUsage

} // namespace streaming
} // namespace essentia

#endif // ESSENTIA_TYPES_H
