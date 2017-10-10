#ifndef _FONTS_H_
#define _FONTS_H_

// STL
#include <vector>
#include <string>

//OpenGL and SDL 
#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// custom libs 
#include "agl.h" 

/* 
 * A light and fast library to load and use TTF in OpenGL.  
 * The only way to do render TTF in OpenGL is to render each glyph as a texture, which of course
 * carries a painful overhead on the game at runtime. 
 * The solution is to load an atlas of chars of the selected TTF as a vector
 * of textures and store in memory. Later on, when we'll need to render text 
 * we'll just render it as a list of textured quads from pre-loaded textures.
 */
 
 namespace fonts {
 using agl::TexID; 
 
 // starting offset of the ASCII chars
 static const auto ASCII_SPACE_CODE = 0x20; 
 // ending offset of the ASCII chars 
 static const auto ASCII_DEL_CODE = 0x7F; 

 // optimized X GLubyte version of a Glyph 
 class Glyph {
 private: 
  // members 
  char m_letter; // freetype glyph index
  TexID m_texID; 

  GLubyte m_minx;  
  GLubyte m_miny;  
  GLubyte m_advance; // number of pixels to advance on x axis
  GLubyte m_maxx; 
  GLubyte m_maxy;

 public: 
    // accessors 
    // Note: accessors use decltype as the exact type of each member 
    // can be changed due to memory optimization 
    
    inline decltype(m_letter) get_letter() { return m_letter; }
    inline decltype(m_texID) get_textureID() { return m_texID; }
    inline decltype(m_advance) get_advance() { return m_advance; }
    inline decltype(m_minx) get_minX() { return m_minx; }
    inline decltype(m_miny) get_minY() { return m_miny; }
    inline decltype(m_maxx) get_maxX() { return m_maxx; }
    inline decltype(m_maxy) get_maxY() { return m_maxy; }
    
    Glyph(char m_letter, TexID textureID, GLubyte m_minx, GLubyte m_miny, GLubyte m_advance,
          GLubyte m_maxx, GLubyte m_maxy); 
};
 

// Abstract GL TextRenderer 
// Responsible of loading the TTF and initialize the texture atlas vec. 

class AGLTextRenderer {
private:
// the texture atlas vector
std::vector<Glyph> m_glyphs;
int m_font_outline; 
int m_font_height; 
TTF_Font *m_font_ptr; 
agl::Env& m_env; // cache envinronment 

inline Glyph &get_glyph_at(size_t index) { return m_glyphs.at(index-ASCII_SPACE_CODE); }
void loadTextureVector(); 

// prevent to call cons, use friend function instead 
AGLTextRenderer(const char *font_path, size_t font_size); 

public: 
 void renderChar(char letter, int x_o, int y_o); 
 void renderText(char *str, int x_o, int y_o); 
 int get_width(const char *str); 
 
 inline decltype(m_font_height) get_height() { return m_font_height; }
 
 // quit gracefully
 virtual ~AGLTextRenderer(); 
 // return singleton instance 
 friend AGLTextRenderer* getTextRenderer(); 

}; 

// singleton loader 
AGLTextRenderer* getTextRenderer(const char * font_path, size_t font_size); 

} // namespace 

#endif 