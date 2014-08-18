//          Matthew Avery Coder 2012 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
#pragma once
#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <functional>

namespace util {

typedef uint8_t byte_t;
typedef std::vector<byte_t> bytes_t;

namespace bytes {

namespace k {


/**
 * @brief byte to bit shift helper function
 *
 * @param offset number of bytes to shift. syntactic sugar
 *
 * @return number of bits to shift
 */
constexpr int shift(const unsigned int offset) {
  return offset * 8;
}


/**
 * @brief binary mask helper function
 *
 * @param offset the byte offset that the mask should apply to.syntactic sugar
 *
 * @return returns bits set to 1 at the given offset
 */
constexpr uint64_t mask(const unsigned int offset) {
  return 0xFFul << shift(offset);
}

namespace x32 {
constexpr int bytes = 4;
constexpr int bits = 32;
}

namespace x64 {
constexpr int bytes = 8;
constexpr int bits = 64;
}

constexpr byte_t all = 0xFF;
constexpr byte_t none = 0x00;

constexpr byte_t bit_1 = 0x01;
constexpr byte_t bit_2 = 0x02;
constexpr byte_t bit_3 = 0x04;
constexpr byte_t bit_4 = 0x08;
constexpr byte_t bit_5 = 0x10;
constexpr byte_t bit_6 = 0x20;
constexpr byte_t bit_7 = 0x40;
constexpr byte_t bit_8 = 0x80;

constexpr byte_t not_bit_1 = 0x0FE;
constexpr byte_t not_bit_2 = 0xFD;
constexpr byte_t not_bit_3 = 0xFB;
constexpr byte_t not_bit_4 = 0xF7;
constexpr byte_t not_bit_5 = 0xEF;
constexpr byte_t not_bit_6 = 0xDF;
constexpr byte_t not_bit_7 = 0xBF;
constexpr byte_t not_bit_8 = 0x7F;

constexpr byte_t top_7_bits = not_bit_1;
constexpr byte_t top_6_bits = top_7_bits &not_bit_2;
constexpr byte_t top_5_bits = top_6_bits &not_bit_3;
constexpr byte_t top_4_bits = top_5_bits &not_bit_4;
constexpr byte_t top_3_bits = top_4_bits &not_bit_5;
constexpr byte_t top_2_bits = top_3_bits &not_bit_6;

constexpr byte_t bottom_7_bits = not_bit_8;
constexpr byte_t bottom_6_bits = bottom_7_bits &not_bit_7;
constexpr byte_t bottom_5_bits = bottom_6_bits &not_bit_6;
constexpr byte_t bottom_4_bits = bottom_5_bits &not_bit_5;
constexpr byte_t bottom_3_bits = bottom_4_bits &not_bit_4;
constexpr byte_t bottom_2_bits = bottom_3_bits &not_bit_3;

constexpr uint64_t byte_mask = 0xFF;

//constexpr uint64_t byte_1_mask = mask(0);
//constexpr uint64_t byte_2_mask = mask(1);
//constexpr uint64_t byte_3_mask = mask(2);
//constexpr uint64_t byte_4_mask = mask(3);
//constexpr uint64_t byte_5_mask = mask(4);
//constexpr uint64_t byte_6_mask = mask(5);
//constexpr uint64_t byte_7_mask = mask(6);
//constexpr uint64_t byte_8_mask = mask(7);

constexpr int byte_shift = 8;
//constexpr int byte_1_shift = shift(0);
//constexpr int byte_2_shift = shift(1);
//constexpr int byte_3_shift = shift(2);
//constexpr int byte_4_shift = shift(3);
//constexpr int byte_5_shift = shift(4);
//constexpr int byte_6_shift = shift(5);
//constexpr int byte_7_shift = shift(6);
//constexpr int byte_8_shift = shift(7);
}

/**
 * @brief check if  are set
 *
 * @param byte
 * @param mask
 * @param bits
 *
 * @return 
 */
constexpr bool is_set(byte_t byte, byte_t mask, byte_t bits) {
  return ((byte & mask) == bits);
}

constexpr bool is_set(byte_t byte, byte_t mask) {
  return ((byte & mask) == mask);
}

inline void set(byte_t &byte, byte_t bit, const bool set) {
  if (set) {
    byte |= bit;
  } else {
    byte &= ~bit;
  }
  return;
}

template<class T>
constexpr byte_t get(const unsigned int offset, const T v) {
  return static_cast<byte_t>((v & k::mask(offset)) >> k::shift(offset));
}

constexpr bool is_32_bit_in_64_bit(const byte_t*in) {
  return (in[0] == 0 && in[1] == 0 && in[2] == 0 && in[3] == 0);
}

inline void write_64_bits(const uint64_t &v, void *void_out) {
  byte_t* out = static_cast<byte_t*>(void_out);
  out[0] = get(7, v);
  out[1] = get(6, v);
  out[2] = get(5, v);
  out[3] = get(4, v);
  out[4] = get(3, v);
  out[5] = get(2, v);
  out[6] = get(1, v);
  out[7] = get(0, v);
}

inline void write_32_bits(const uint32_t &v, byte_t *void_out) {
  byte_t* out = static_cast<byte_t*>(void_out);
  out[0] = get(3, v);
  out[1] = get(2, v);
  out[2] = get(1, v);
  out[3] = get(0, v);
}

inline void write_16_bits(const uint16_t &v, byte_t *void_out) {
  byte_t* out = static_cast<byte_t*>(void_out);
  out[0] = get(1, v);
  out[1] = get(0, v);
}

inline void write_size_to_64_bits(const size_t &v, byte_t *out) {
  //conditionals are constant
#if defined(__LP64__) || defined(_WIN64)
  write_64_bits(static_cast<uint64_t>(v), out);
#else
  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  write_32_bits(static_cast<uint32_t>(v), &out[4]);
#endif

}

constexpr uint64_t read_64_bits(byte_t const *v) {
  return (static_cast<uint64_t>(v[0]) << k::shift(7))
         | (static_cast<uint64_t>(v[1]) << k::shift(6))
         | (static_cast<uint64_t>(v[2]) << k::shift(5))
         | (static_cast<uint64_t>(v[3]) << k::shift(4))
         | (static_cast<uint64_t>(v[4]) << k::shift(3))
         | (static_cast<uint64_t>(v[5]) << k::shift(2))
         | (static_cast<uint64_t>(v[6]) << k::shift(1))
         | (static_cast<uint64_t>(v[7]) << k::shift(0));
}

constexpr uint32_t read_32_bits(byte_t const *v) {
  return (static_cast<uint32_t>(v[0]) << k::shift(3))
         | (static_cast<uint32_t>(v[1]) << k::shift(2))
         | (static_cast<uint32_t>(v[2]) << k::shift(1))
         | (static_cast<uint32_t>(v[3]) << k::shift(0));
}

constexpr uint16_t read_16_bits(byte_t const *v) {
  return static_cast<uint16_t>((static_cast<uint16_t>(v[0]) << k::shift(1))
                            | (static_cast<uint16_t>(v[1]) << k::shift(0)));
}

constexpr size_t read_size_bits_from_64_bits(byte_t const *v) {
#if defined(__LP64__) || defined(_WIN64)
  return static_cast<size_t>(read_64_bits(v));
#else
  return static_cast<size_t>(read_32_bits(v + 4));
#endif
}

template<typename B, typename I>
void multi_copy(B buffer, I begin, I end) {
  std::copy(begin, end, buffer);
}

template<typename B, typename I, typename ... Args>
void multi_copy(B buffer, I begin, I end, Args ... args) {
  std::copy(begin, end, buffer);
  multi_copy(buffer + std::distance(begin, end), args ...);
}

inline const byte_t *search(const byte_t *begin, const byte_t *end, const byte_t find) {
  while (begin != end) {
    if (find == *begin) { return begin; }
    ++begin;
  }
  return nullptr;
}

inline const byte_t *search(const byte_t *begin, const byte_t *end,
  const byte_t *find_begin, const byte_t *find_end) {
  auto find_len = std::distance(find_begin, find_end);
  while (begin != end - find_len) {
    const byte_t* search_begin = search(begin,end,*find_begin);
    if(search_begin == nullptr || std::memcmp(search_begin,find_begin,find_len) != 0) {
      ++begin;
    } else {
      return search_begin;
    }
  }
  return nullptr;
}

//generic way to view a contiguous set of bytes, regardless of their sign or container
class View {
  public:

    typedef byte_t value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;

    View(std::string &b): data_((byte_t*) b.data()),size_(b.size()){}

    View(std::vector<char> &b): data_((byte_t*) b.data()),size_(b.size()){}
    View(std::vector<unsigned char> &b): data_((byte_t*) b.data()),size_(b.size()){}

    template<size_t N> View(std::array<char,N> &b) : data_((byte_t*) b.data()),size_(N){}
    template<size_t N> View(std::array<unsigned char,N> &b) : data_((byte_t*) b.data()),size_(N){}

    template<size_t N> View(char (&b)[N]) : data_((byte_t*) &b[0]),size_(N){}
    template<size_t N> View(unsigned char (&b)[N]) : data_((byte_t*) &b[0]),size_(N){}

    View(const char *ptr, size_t N) : data_((byte_t*) ptr),size_(N){}
    View(char *ptr, size_t N) : data_((byte_t*) ptr),size_(N){}
    View(const unsigned char *ptr, size_t N) : data_((byte_t*) ptr),size_(N){}
    View(unsigned char *ptr, size_t N) : data_((byte_t*) ptr),size_(N){}

    iterator begin(){ return iterator(data_);}
    iterator end()  { return iterator(data_ + size_); }
    const_iterator begin() const { return const_iterator(data_);}
    const_iterator end() const { return const_iterator(data_ + size_); }
    const_iterator cbegin() const { return const_iterator(data_);}
    const_iterator cend() const { return const_iterator(data_ + size_); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(cbegin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    const_pointer data() const {return data_;}
    pointer data(){return data_;}
    size_type size() const { return size_;}
    void refresh(pointer data, size_type size) { data_ = data, size_ = size; }

  private: 
    pointer data_;
    size_type size_;

};

class ConstView {
  public:

    typedef byte_t value_type;
    typedef const value_type* const_pointer;
    typedef const value_type* const_iterator;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;

    ConstView(const View &v) : data_(v.data()),size_(v.size()){}
    ConstView(const std::string &b) : data_((const_pointer) b.data()),size_(b.size()){}
    ConstView(std::string &b): data_((const_pointer) b.data()),size_(b.size()){}

    ConstView(const std::vector<char> &b) : data_((const_pointer) b.data()),size_(b.size()){}
    ConstView(const std::vector<unsigned char> &b) : data_((const_pointer) b.data()),size_(b.size()){}
    ConstView(std::vector<char> &b): data_((const_pointer) b.data()),size_(b.size()){}
    ConstView(std::vector<unsigned char> &b): data_((const_pointer) b.data()),size_(b.size()){}

    template<size_t N> ConstView(const std::array<char,N> &b) : data_((const_pointer) b.data()),size_(N){}
    template<size_t N> ConstView(const std::array<unsigned char,N> &b) : data_((const_pointer) b.data()),size_(N){}
    template<size_t N> ConstView(std::array<char,N> &b) : data_((const_pointer) b.data()),size_(N){}
    template<size_t N> ConstView(std::array<unsigned char,N> &b) : data_((const_pointer) b.data()),size_(N){}

    template<size_t N> ConstView(const char (&b)[N]) : data_((const_pointer) &b[0]),size_(N){}
    template<size_t N> ConstView(const unsigned char (&b)[N]) : data_((const_pointer) &b[0]),size_(N){}
    template<size_t N> ConstView(char (&b)[N]) : data_((const_pointer) &b[0]),size_(N){}
    template<size_t N> ConstView(unsigned char (&b)[N]) : data_((const_pointer) &b[0]),size_(N){}

    ConstView(const char *ptr, size_t N) : data_((const_pointer) ptr),size_(N){}
    ConstView(char *ptr, size_t N) : data_((const_pointer) ptr),size_(N){}
    ConstView(const unsigned char *ptr, size_t N) : data_((const_pointer) ptr),size_(N){}
    ConstView(unsigned char *ptr, size_t N) : data_((const_pointer) ptr),size_(N){}

    const_iterator begin() const { return const_iterator(data_);}
    const_iterator end() const { return const_iterator(data_ + size_); }
    const_iterator cbegin() const { return const_iterator(data_);}
    const_iterator cend() const { return const_iterator(data_ + size_); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(cbegin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    const_pointer data() const {return data_;}
    size_type size() const { return size_;}

  private: 
    const_pointer data_;
    size_type size_;

};


//a set of callbacks that provides a uniform way of trying to resize containers of contiguous bytes. 
//In the case of containers that allow for resizing (vector,string), the resize actually occurs
//In the case of containers that aren't resizeable, if the resize attempt is shrinkage, it will succeed, or else it will fail
inline bool default_resize(size_t size,byte_t byte,View &view) { return view.size() == size; }
inline bool no_resize(size_t old_size, size_t new_size) { return old_size == new_size; }

template <class T> bool array_resize(T *ptr, size_t N, size_t size,byte_t byte,View &view, size_t &output_size) { 
    if(size > N) {
      return false;
    } else {
      std::fill(ptr+size,ptr+N,byte);
      view.refresh(view.data(),size);
      output_size = size;
      return true;
    }
}

template <class T> bool stl_resize(T &container, size_t size,byte_t byte,View &view) { 
    container.resize(size,(typename T::value_type) byte);
    view.refresh((byte_t*) container.data(), size);
    return true;
}

typedef std::function<bool(size_t,byte_t,View &)> resize_callback_t;

inline resize_callback_t resize_callback(std::string &b) { return std::bind(&stl_resize<std::string>,std::ref(b),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3); }
inline resize_callback_t resize_callback(std::vector<char> &b) { return std::bind(&stl_resize<std::vector<char>>,std::ref(b),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3); }
inline resize_callback_t resize_callback(std::vector<unsigned char> &b) { return std::bind(&stl_resize<std::vector<unsigned char>>,std::ref(b),std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);}
template<size_t N> resize_callback_t resize_callback(std::array<char,N> &b) { return std::bind(&no_resize,N,std::placeholders::_1); }
template<size_t N> resize_callback_t resize_callback(std::array<unsigned char,N> &b) { return std::bind(&no_resize,N,std::placeholders::_1); }
inline resize_callback_t resize_callback(char *ptr, size_t N)  { return std::bind(&no_resize,N,std::placeholders::_1); }
inline resize_callback_t resize_callback(unsigned char *ptr, size_t N)  { return std::bind(&no_resize,N,std::placeholders::_1); }
template<size_t N> resize_callback_t resize_callback(char (&b)[N]) { return std::bind(&no_resize,N,std::placeholders::_1); }
template<size_t N> resize_callback_t resize_callback(unsigned char (&b)[N]) { return std::bind(&no_resize,N,std::placeholders::_1); }
inline resize_callback_t resize_callback(char *ptr, size_t N, size_t &output_size)  { return std::bind(&array_resize<char>,ptr,N,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::ref(output_size)); }
inline resize_callback_t resize_callback(unsigned char *ptr, size_t N, size_t &output_size)  { return std::bind(&array_resize<unsigned char>,ptr,N,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::ref(output_size));  }
template<size_t N> resize_callback_t resize_callback(char (&b)[N], size_t &output_size) { return std::bind(&array_resize<char>,&b[0],N,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::ref(output_size)); }
template<size_t N> resize_callback_t resize_callback(unsigned char (&b)[N], size_t &output_size) { return std::bind(&array_resize<unsigned char>,&b[0],N,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::ref(output_size)); }
template<size_t N> resize_callback_t resize_callback(std::array<char,N> &b, size_t &output_size)  { return std::bind(&array_resize<char>,&b[0],N,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::ref(output_size)); }
template<size_t N> resize_callback_t resize_callback(std::array<unsigned char,N> &b, size_t &output_size){ return std::bind(&array_resize<unsigned char>,&b[0],N,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,std::ref(output_size)); }
}
}
