#ifndef __ENUM_H__
#define __ENUM_H__

#define ENUM(N, FIRST_ENUMERANT, ...) enum N##_enum { FIRST_ENUMERANT = 0, __VA_ARGS__, }; Enum<N##_enum> N;

#define Enum(TypeName, FirstEnumerant, ...)                                             \
class TypeName                                                                          \
{                                                                                       \
public:                                                                                 \
  enum EnumType{                                                                        \
    FirstEnumerant = 0,                                                                 \
    __VA_ARGS__,                                                                        \
  }m_enum;                                                                              \
  TypeName(){m_enum = FirstEnumerant;}                                                  \
  TypeName(int enumerant){m_enum = static_cast<EnumType>(enumerant);}                   \
  TypeName(EnumType enumerant){m_enum = enumerant;}                                     \
  operator int(){return static_cast<int>(m_enum);}                                      \
  TypeName& operator ++ () {int& enum_ref = (int&)m_enum; ++enum_ref; return *this;}    \
  TypeName& operator ++ (int) {int& enum_ref = (int&)m_enum; enum_ref++; return *this;} \
}

#endif