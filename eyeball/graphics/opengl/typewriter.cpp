#include <eyeball/graphics/opengl/utils.h>

#include "typewriter.h"

namespace {

  const size_t FONT_GLYPHS = 256;
  const size_t MAX_TEXT = 2048;

  bool createTrueTypeFont(HDC hDC, char* name, LONG fontHeight, LONG fontWeight, BYTE fontItalic, BYTE fontUndLine, BYTE fontStrOut, HFONT& outFont, HFONT oldFont)
  {
    LOGFONTA lf;

    lf.lfHeight = fontHeight;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = fontWeight;
    lf.lfItalic = fontItalic;
    lf.lfUnderline = fontUndLine;
    lf.lfStrikeOut = fontStrOut;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_TT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = CLEARTYPE_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;
    strcpy(lf.lfFaceName, name);

    if ((outFont = CreateFontIndirectA(&lf)) == NULL)
    {
      return false;
    }
    else
    {
      oldFont = (HFONT)SelectObject(hDC, outFont);
    }

    return true;
  }
  
  bool createTrueTypeFontW(HDC hDC, wchar_t* name, LONG fontHeight, LONG fontWeight, BYTE fontItalic, BYTE fontUndLine, BYTE fontStrOut,
    HFONT& outFont, HFONT oldFont)
  {
    LOGFONTW lf;

    lf.lfHeight = fontHeight;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = fontWeight;
    lf.lfItalic = fontItalic;
    lf.lfUnderline = fontUndLine;
    lf.lfStrikeOut = fontStrOut;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_TT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;
    wcscpy(lf.lfFaceName, name);

    if ((outFont = CreateFontIndirectW(&lf)) == NULL)
    {
      return false;
    }
    else
    {
      oldFont = (HFONT)SelectObject(hDC, outFont);
    }

    return true;
  }
}

bool Typewriter::Font::create(HDC hDC, char* name, LONG fontHeight, LONG fontWeight, BYTE fontItalic, BYTE fontUndLine, BYTE fontStrOut)
{
  resetOpenGLError();
  if (hDC == INVALID_HANDLE_VALUE)
  {
    return false;
  }
  if (glIsList(m_fontDisplayList))
  {
    glDeleteLists(m_fontDisplayList, FONT_GLYPHS);
  }

  m_fontDisplayList = glGenLists(FONT_GLYPHS);

  HFONT oldFont = (HFONT)INVALID_HANDLE_VALUE;
  HFONT font = (HFONT)INVALID_HANDLE_VALUE;

  bool bret = createTrueTypeFont(hDC, name, fontHeight, fontWeight, fontItalic, fontUndLine, fontStrOut, font, oldFont);
  if (bret)
  {
    wglUseFontBitmapsA(hDC, m_alphabets[Alphabet::Ascii].first, FONT_GLYPHS, m_fontDisplayList);

    SelectObject(hDC, oldFont);
    m_hFont = font;
  }
  else
  {
    if (glIsList(m_fontDisplayList))
    {
      glDeleteLists(m_fontDisplayList, FONT_GLYPHS);
    }
  }

  m_hDC = hDC;

  OPENGL_CHECK_ERROR();

  return bret;
}

bool Typewriter::Font::wcreate(HDC hDC, wchar_t* name, LONG fontHeight, LONG fontWeight, BYTE fontItalic, BYTE fontUndLine, BYTE fontStrOut, Alphabet abet)
{
  resetOpenGLError();
  if (1 > abet || Alphabet::Num < abet)
  {
    return false;
  }
  if (hDC == INVALID_HANDLE_VALUE)
  {
    return false;
  }
  if (glIsList(m_fontDisplayList))
  {
    glDeleteLists(m_fontDisplayList, FONT_GLYPHS);
  }

  m_fontDisplayList = glGenLists(FONT_GLYPHS);

  HFONT oldFont = (HFONT)INVALID_HANDLE_VALUE;
  HFONT font = (HFONT)INVALID_HANDLE_VALUE;

  bool bret = createTrueTypeFontW(hDC, name, fontHeight, fontWeight, fontItalic, fontUndLine, fontStrOut, font, oldFont);
  if (bret)
  {
    wglUseFontBitmapsW(hDC, m_alphabets[abet].first, m_alphabets[abet].second - m_alphabets[abet].first, m_fontDisplayList);

    SelectObject(hDC, oldFont);
    m_hFont = font;
  }
  else
  {
    if (glIsList(m_fontDisplayList))
    {
      glDeleteLists(m_fontDisplayList, m_alphabets[abet].second - m_alphabets[abet].first);
    }
  }


  m_alphabet = abet;
  m_hDC = hDC;

  return bret;
}

void Typewriter::Font::displayedStringSize(const char* istring, size_t& pix_width, size_t& pix_height)
{
  SIZE size = { 0 };
  HFONT olfont = (HFONT)SelectObject(m_hDC, m_hFont);

  GetTextExtentPoint32A(m_hDC, istring, (int)strlen(istring), &size);

  pix_width = size.cx;
  pix_height = size.cy;

  SelectObject(m_hDC, olfont);
}

void Typewriter::Font::displayedStringSizeW(const wchar_t* istring, size_t& pix_width, size_t& pix_height)
{
  SIZE size = { 0 };
  HFONT olfont = (HFONT)SelectObject(m_hDC, m_hFont);

  GetTextExtentPoint32W(m_hDC, istring, (int)wcslen(istring), &size);

  pix_width = size.cx;
  pix_height = size.cy;

  SelectObject(m_hDC, olfont);
}

Typewriter::Font::~Font()
{
  if (glIsList(m_fontDisplayList))
  {
    glDeleteLists(m_fontDisplayList, m_alphabets[m_alphabet].second - m_alphabets[m_alphabet].first);
  }
}

void Typewriter::printf(Font* font_, unsigned long align, SpecialEffects effects, float r, float g, float b, float x, float y, const char* format, ...)
{
  char outBuffer[MAX_TEXT] = { 0 };
  va_list lst;
  va_start(lst, format);

  vsprintf_s(outBuffer, MAX_TEXT, format, lst);
  GLsizei str_len = (GLsizei)strlen(outBuffer);

  glColor3f(r, g, b);

  size_t diplStrSize[2] = { 0 };
  font_->displayedStringSize(outBuffer, diplStrSize[0], diplStrSize[1]);
  int viewport[4] = { 0 };
  glGetIntegerv(GL_VIEWPORT, viewport);

  // diplStrSize[1] *= (float)viewport[2] / (float)viewport[3];

  if (align & Alignment::Top)
  {
    y -= (static_cast<float>(diplStrSize[1]) / static_cast<float>(viewport[3]));
  }
  if (align & Alignment::CenterW)
  {
    x -= ((static_cast<float>(diplStrSize[0]) / static_cast<float>(viewport[2])) * 0.5f);
  }
  if (align & Alignment::CenterH)
  {
    y -= ((static_cast<float>(diplStrSize[1]) / static_cast<float>(viewport[3])) * 0.5f);
  }
  if ((align & Alignment::Right) && !(align & Alignment::CenterW))
  {
    x -= (static_cast<float>(diplStrSize[0]) / static_cast<float>(viewport[2]));

  }
  if ((align & Alignment::Bottom) && !(align & Alignment::CenterH))
  {
    // y -= (static_cast<float>(diplStrSize[1] / static_cast<float>(viewport[3]);
  }

  const double border_offset = 7.E-3;
  double normalized_dswidth = diplStrSize[0] / static_cast<float>(viewport[2]);
  double normalized_dsheight = diplStrSize[1] / static_cast<float>(viewport[3]);

  switch (effects)
  {
    case SpecialEffects::None:
    break;
    case SpecialEffects::Boxed:
    {
      glPushAttrib(GL_POLYGON_BIT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(2.0);
      glBegin(GL_QUADS);
      glVertex2d(x - border_offset, y - border_offset);
      glVertex2d(x + normalized_dswidth + border_offset, y - border_offset);
      glVertex2d(x + normalized_dswidth + border_offset, y + normalized_dsheight + border_offset);
      glVertex2d(x - border_offset, y + normalized_dsheight + border_offset);
      glEnd();
      glPopAttrib();

    }
    break;
    case SpecialEffects::HighlightInvert:
      glPushAttrib(GL_POLYGON_BIT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(2.0);
      glBegin(GL_QUADS);
      glVertex2d(x - border_offset, y - border_offset);
      glVertex2d(x + normalized_dswidth + border_offset, y - border_offset);
      glVertex2d(x + normalized_dswidth + border_offset, y + normalized_dsheight + border_offset);
      glVertex2d(x - border_offset, y + normalized_dsheight + border_offset);
      glEnd();
      glPopAttrib();
      glColor3f(1.0f - r, 1.0f - g, 1.0f - b);
    break;
    case SpecialEffects::HighlightOnBlack:
      glPushAttrib(GL_POLYGON_BIT);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      glLineWidth(2.0);
      glBegin(GL_QUADS);
      glVertex2d(x - border_offset, y - border_offset);
      glVertex2d(x + normalized_dswidth + border_offset, y - border_offset);
      glVertex2d(x + normalized_dswidth + border_offset, y + normalized_dsheight + border_offset);
      glVertex2d(x - border_offset, y + normalized_dsheight + border_offset);
      glEnd();
      glPopAttrib();
      glColor3f(0.0, 0.0, 0.0);
    break;
  }


  glRasterPos3f(x, y, 0);


  glPushAttrib(GL_LIST_BIT);
  glListBase(font_->displayListId());
  glCallLists(str_len, GL_UNSIGNED_BYTE, outBuffer);

  glPopAttrib();
}