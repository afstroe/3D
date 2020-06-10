#ifndef __TYPEWRITER_H__
#define __TYPEWRITER_H__

#include <Windows.h>
#include <gl/GL.h>

#include <eyeball/utils/enum.h>
#include <utility>

class Typewriter
{
public:
  Enum(Alignment,
    Whatever,
    Bottom = 1 << 0,
    Right = 1 << 1,
    CenterW = 1 << 2,
    CenterH = 1 << 3,
    Top = 1 << 4
  );


  Enum(SpecialEffects,
    None,
    Boxed,
    HighlightInvert,
    HighlightOnBlack
  );

  class Font
  {
  protected:
    Enum(AlphabetStart,
      None,
      Ascii = 0,
      Latin = 0,
      Greek = 0x374,
      Cyrillic = 0x401,
      Hebrew = 0x591,
      Arabic = 0x600
    );

    Enum(AlphabetEnd,
      None,
      Ascii = 0xFF,
      Latin = 0x2AF,
      Greek = 0x3FF,
      Cyrillic = 0x513,
      Hebrew = 0x5F4,
      Arabic = 0x76D
    );
  public:
    Enum(Alphabet,
      Ascii,
      Latin,
      Greek,
      Cyrillic,
      Hebrew,
      Arabic,
      Num
    );

  public:
    inline const Alphabet alphabet() const
    {
      return m_alphabet;
    }

    bool create(HDC hDC, char* name, LONG fontHeight, LONG fontWeight, BYTE fontItalic,
      BYTE fontUndLine, BYTE fontStrOut);
    bool wcreate(HDC hDC, wchar_t* name, LONG fontHeight, LONG fontWeight, BYTE fontItalic,
      BYTE fontUndLine, BYTE fontStrOut, Alphabet abet);
    void displayedStringSize(const char* istring, size_t& pix_width, size_t& pix_height);

    void displayedStringSizeW(const wchar_t* istring, size_t& pix_width, size_t& pix_height);

    void alphabetInterval(AlphabetStart& abet_start, AlphabetEnd& abet_end)
    {
      abet_start = (m_alphabets[m_alphabet]).first;
      abet_end = (m_alphabets[m_alphabet]).second;
    }

    Font()
    {
      m_alphabets[Alphabet::Ascii] = std::make_pair(AlphabetStart::Ascii, AlphabetEnd::Ascii);
      m_alphabets[Alphabet::Latin] = std::make_pair(AlphabetStart::Latin, AlphabetEnd::Latin);
      m_alphabets[Alphabet::Greek] = std::make_pair<AlphabetStart, AlphabetEnd>(AlphabetStart::Greek, AlphabetEnd::Greek);
      m_alphabets[Alphabet::Cyrillic] = std::make_pair<AlphabetStart, AlphabetEnd>(AlphabetStart::Cyrillic, AlphabetEnd::Cyrillic);
      m_alphabets[Alphabet::Hebrew] = std::make_pair<AlphabetStart, AlphabetEnd>(AlphabetStart::Hebrew, AlphabetEnd::Hebrew);
      m_alphabets[Alphabet::Arabic] = std::make_pair<AlphabetStart, AlphabetEnd>(AlphabetStart::Arabic, AlphabetEnd::Arabic);
      m_fontDisplayList = 0xFFFFFFFF;
    }

    ~Font();

    const GLint displayListId() const
    {
      return m_fontDisplayList;
    }

  protected:
    std::pair<AlphabetStart, AlphabetEnd> m_alphabets[Alphabet::Num];
    Alphabet m_alphabet;/*!< the font's alphabet */
    GLint m_fontDisplayList;
    HDC m_hDC;
    HFONT m_hFont;
  };

  void printf(Font* font_, unsigned long align, SpecialEffects effects, float r, float g, float b, float x, float y, const char* format, ...);

  void wprintf(Font* font_, unsigned long align, SpecialEffects effects, float r, float g, float b, float x, float y, const wchar_t* format, ...);
};
#endif