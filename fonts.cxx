#include "agl.h"

namespace agl {

// Glyph constructor
Glyph::Glyph(char letter, TexID textureID, GLubyte minx, GLubyte maxx,
             GLubyte miny, GLubyte maxy, GLubyte advance)
    : m_letter(letter), m_texID(textureID), m_minx(minx), m_miny(miny),
      m_maxx(maxx), m_maxy(maxy), m_advance(advance) {}

// TextRenderer: return the singleton instance
AGLTextRenderer *getTextRenderer(const char *font_path, size_t font_size) {
  const static auto TAG = __func__;
  lg::i(TAG, "Creating a text renderer, loading the font %s with size %zu",
        font_path, font_size);

  // having a unique ptr ensures the TextRenderer will be called only during the
  // main
  static std::unique_ptr<AGLTextRenderer> s_ptr(nullptr);
  if (!s_ptr) {
    s_ptr.reset(new AGLTextRenderer(font_path, font_size));
  }

  return s_ptr.get();
}

AGLTextRenderer::AGLTextRenderer(const char *font_path, size_t font_size)
    : m_env(agl::get_env()) {
  const static auto TAG = __func__;

  // Init font for text drawing
  if (TTF_Init() == -1) {
    lg::e(TAG, "Cannot init font %s", font_path);
    exit(EXIT_FAILURE);
  }

  m_font_ptr = TTF_OpenFont(font_path, font_size);
  if (!m_font_ptr) {
    lg::e(TAG, "TTF_OpenFont: %s\n", TTF_GetError());
  }

  // disable kerning since it's not useful for this application
  TTF_SetFontKerning(m_font_ptr, 0);

  // get font m_font_outline
  m_font_outline = TTF_GetFontOutline(m_font_ptr);
  m_font_height = TTF_FontHeight(m_font_ptr);
  loadTextureVector();
}

void AGLTextRenderer::loadTextureVector() {
  static const auto TAG = __func__;
  SDL_Surface *surface;
  SDL_Color color = {255, 255, 255, 255};

  int miny, maxy, advance, minx, maxx;
  agl::TexID texbind;
  // for (char i = ASCII_SPACE_CODE; i < ASCII_DEL_CODE; ++i) {
  for (char ch = ' '; ch < '~'; ++ch) {
    // cache glyph metrics and texture
    TTF_GlyphMetrics(m_font_ptr, ch, &minx, &maxx, &miny, &maxy, &advance);

    // Render the font on a surface as Blended : slower but high quality
    // if performance is suffering, try switch to Solid
    if (!(surface = TTF_RenderGlyph_Blended(m_font_ptr, ch, color))) {
      lg::e(TAG, "%s\n", TTF_GetError());
    }

    // generate texture ID
    glGenTextures(1, &texbind);

    glBindTexture(GL_TEXTURE_2D, texbind);
    // create Texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // create Glyph
    Glyph glyph(ch, texbind, minx, maxx, miny, maxy, advance);
    // append glyph texture at the end of the atlas
    m_glyphs.push_back(glyph);
    SDL_FreeSurface(surface);
  }
}

// Reminder: x_o, y_o is the top-left origin
int AGLTextRenderer::render(int x_o, int y_o, const char *str) {
  // for (; *str; ++str)
  for (const char *c = str; (*c) != '\0'; ++c) {
    renderChar(x_o, y_o, *c);
    // get next x_o-position
    x_o += get_glyph_at(*c).get_advance();
  }

  // return end of the string
  return x_o;
}

int AGLTextRenderer::render(int x_o, int y_o, std::string &str) {
  return render(x_o, y_o, str.c_str());
}

int AGLTextRenderer::renderf(int x_o, int y_o, const char *fmt, ...) {
  std::va_list ap;
  // process argument list
  va_start(ap, fmt);

  char *buf; //= nullptr;
  vasprintf(&buf, fmt, ap);

  va_end(ap);

  // finally call the render function
  int ret = render(x_o, y_o, buf);
  std::free(buf);

  return ret;
}

// private
// Reminder: x_o, y_o is the top-left origin
// Get Glyph -
void AGLTextRenderer::renderChar(int x_o, int y_o, char letter) {
  // check on letter todo
  if (letter > '~' || letter < ' ') {
    lg::e(__func__, "Out of range char");
  }

  Glyph glyph = get_glyph_at(letter);

  // We want to draw text over our scene, so no need of Depth Testing
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

  // Blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Texture
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, glyph.get_textureID());

  // Draw texture with quads
  glBegin(GL_QUADS);
  {
    /*
    // bottom-left
    glTexCoord2f(0, 0);
      glVertex2f(x_o, y_o + m_font_height);


      // top-left
      glTexCoord2f(0, 1);
      glVertex2f(x_o, y_o);

      // top-right
      glTexCoord2f(1, 1);
      glVertex2f(x_o+ glyph.get_maxX(), y_o);

      // bottom-right
      glTexCoord2f(1, 0);
      glVertex2f(x_o + glyph.get_maxX(), y_o + m_font_height);
  */
    // bottom-left
    glTexCoord2f(0, 0);
    glVertex2f(x_o - m_font_outline, (y_o + m_font_height - m_font_outline));

    // top-left
    glTexCoord2f(0, 1);
    glVertex2f(x_o - m_font_outline, y_o - m_font_outline);

    // top-right
    glTexCoord2f(1, 1);
    glVertex2f((x_o + glyph.get_maxX() - m_font_outline),
               (y_o - m_font_outline));

    // bottom-right
    glTexCoord2f(1, 0);
    glVertex2f((x_o + glyph.get_maxX() - m_font_outline),
               (y_o + m_font_height - m_font_outline));
  }

  glEnd();

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  // Renable Z-buffer and Lighting
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

int AGLTextRenderer::get_width(const char *str) {
  int width;
  TTF_SizeText(m_font_ptr, str, &width, NULL);
  return width;
}

AGLTextRenderer::~AGLTextRenderer() { TTF_CloseFont(m_font_ptr); }

} // namespace agl