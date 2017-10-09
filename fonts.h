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
 
 // starting offset of the ASCII characters 
 using ASCII_space_code = 0x20; 
 
 // optimized 12 byte version of a Glyph 
 
 class Glyph {
 public: 
    // accessors 
    // Note: accessors use decltype as the exact type of each member 
    // can be changed due to memory optimization 
    
    inline decltype(m_letter) get_letter() { return m_letter; }
    inline decltype(m_texID) get_textureID() { return m_texID; }
    inline decltype(m_width) get_width() { return m_width; }
    inline decltype(m_height) get_heght() { return m_height; }
    inline decltype(m_advance) get_advance() { return m_advance; }
    inline decltype(m_minx) get_minx() { return m_minx; }
    inline decltype(m_miny) get_miny() { return m_miny; }
    inline decltype(m_maxx) get_maxx() { return m_maxx; }
    inline decltype(m_maxy) get_maxy() { return m_maxy; }
    
    Glyph(char m_letter, TexID textureID, byte m_width, byte m_height, byte m_advance,
          char m_bearingX, char m_bearingY, byte m_textureX, byte m_textureY); 

 
 private: 
  // members 
  char m_letter; // freetype glyph index
  TexID m_texID; 

  byte m_minx;  
  byte m_miny;  
  byte m_advance; // number of pixels to advance on x axis
  byte m_maxx; 
  byte m_maxy; 
};
 

// Abstract GL TextRenderer 
// Responsible of loading the TTF and initialize the texture atlas vec. 

class AGLTextRenderer {
public: 
 void renderChar(char letter, int x_o, int y_o); 
 void renderText(const char *str, int x_o, int y_o); 
 int get_width(const char *str); 
 
 inline decltype(m_font_height) AGLTextRenderer::get_height() { return m_font_height; }
 
 // return singleton instance 
 friend AGLTextRenderer& getTextRenderer(); 
private:
    // the texture atlas vector
    std::vector<Glyph> m_glyphs;
    int m_font_height; 
    TTF_Font *m_font_ptr; 
    
    inline Glyph &get_glyph_at(size_t index) { return m_glyphs.at(index-ASCII_space_code); }
    void loadTextureVector(); 
    
    // prevent to call private cons, use friend function instead 
    AGLTextRenderer(); 
    virtual ~AGLTextRenderer(); 
}; 

// singleton loader 
AGLTextRenderer& getTextRenderer(const char * font_path, size_t font_path); 

} // namespace 

#endif 