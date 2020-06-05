// implementation: Alin Stroe
// mail to: afstroe@gmail.com
// linkedin: https://www.linkedin.com/in/alinstroe/
// purpose: useful common definitions
// Copyright © 2020 Alin Stroe, all rights reserved

#ifndef __DEFINES_H__
#define __DEFINES_H__

#define STD_VECTOR(TYPE, VECTOR_TYPE_NAME) VECTOR_TYPE_NAME<TYPE, std::allocator<TYPE>>
#define TEMPLATE_TYPE_STD_VECTOR(TEMPLATE_NAME) template <class, class> class TEMPLATE_NAME

#define FOR(index, cap) for(auto index = 0; index < cap; ++index)

#define DECLARE_PROTECTED_TRIVIAL_ATTRIBUTE(TYPE, NAME)     \
  protected:                                                \
    TYPE m_##NAME;                                          \
  public:                                                   \
    /*! <br>                                                \
      \brief getter / setter for _##NAME <br>               \
      \return a reference to _##NAME <br>                   \
      */                                                    \
    inline TYPE& NAME() { return m_##NAME; }                \
                                                            \
  /*! <br>                                                  \
    \brief getter for _##NAME <br>                          \
    \return a constant reference to _##NAME <br>            \
    */                                                      \
  inline const TYPE& NAME() const { return m_##NAME; }

#define DECLARE_PROTECTED_TRIVIAL_PTR_ATTRIBUTE(TYPE, NAME) \
  protected:                                                \
    TYPE* m_##NAME;                                         \
  public:                                                   \
    /*! <br>                                                \
      \brief getter / setter for _##NAME <br>               \
      \return a reference to _##NAME <br>                   \
      */                                                    \
    inline TYPE*& NAME() { return m_##NAME; }               \
                                                            \
  /*! <br>                                                  \
    \brief getter for _##NAME <br>                          \
    \return a constant reference to _##NAME <br>            \
    */                                                      \
  inline const TYPE* NAME() const { return m_##NAME; }

#define DECLARE_SHARED_POINTER(ACCESS, TYPE, NAME)                    \
  ACCESS:                                                             \
    std::shared_ptr<TYPE> m_##NAME;                                   \
  public:                                                             \
    /*! <br>                                                          \
      \brief getter / setter for _##NAME <br>                         \
      \return a reference to _##NAME <br>                             \
      */                                                              \
    inline std::shared_ptr<TYPE>& NAME() { return m_##NAME; }         \
                                                                      \
  /*! <br>                                                            \
    \brief getter for _##NAME <br>                                    \
    \return a constant reference to _##NAME <br>                      \
    */                                                                \
  inline const std::shared_ptr<TYPE> NAME() const { return m_##NAME; }

static const size_t BENCHMARK_ITERATIONS = 1000;
static const size_t BENCHMARK_PROBLEM_SIZE = 2;

#endif // !__DEFINES_H__
