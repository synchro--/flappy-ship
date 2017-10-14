#include "agl.h"

namespace agl {
    
 Glyph::Glyph(char letter, TexID textureID, GLubyte minx, GLubyte miny, GLubyte advance,
    GLubyte maxx, GLubyte maxy) : m_letter(letter), m_texID(textureID), m_minx(minx), m_miny(miny), m_maxx(maxx), m_maxy(maxy), m_advance(advance) {} 

    // return the singleton instance 
    AGLTextRenderer* getTextRenderer(const char *font_path, size_t font_size) {
        const static auto TAG = __func__; 
        lg::i(TAG, "Creating a text renderer, loading the font %s with size %zu", font_path, font_size);
        
      // having a unique ptr ensures the TextRenderer will be called only during the main
      static std::unique_ptr<AGLTextRenderer> s_ptr(nullptr); 
      if(!s_ptr) {
          s_ptr.reset(new AGLTextRenderer(font_path, font_size)); 
      }
    
    return s_ptr.get(); 
    }
    
AGLTextRenderer::AGLTextRenderer(const char *font_path, size_t font_size) 
: m_env(agl::get_env())
{
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

    int maxy, miny, advance, minx, maxx;
    agl::TexID texbind;
    for (char i = ASCII_SPACE_CODE ; i < ASCII_DEL_CODE; ++i) {
        // cache glyph metrics and texture
        TTF_GlyphMetrics(m_font_ptr, i, &minx, &maxx, &miny, &maxy, &advance);
        // Render the font on a surface as Blended : slower but high quality 
        // if performance is suffering, try switch to Solid
        if (!(surface = TTF_RenderGlyph_Blended(m_font_ptr, i, color))) {
            lg::e(TAG,"%s\n", TTF_GetError());
        }

        // generate texture ID 
        glGenTextures(1, &texbind);
        // create Glyph and bind it  
        Glyph glyph(i, texbind, miny, maxy, advance, minx, maxx);
        glBindTexture(GL_TEXTURE_2D, glyph.get_textureID());
        // create Texture 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, surface->pixels);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        // append texture at the end of the atlas 
        m_glyphs.push_back(glyph);
        SDL_FreeSurface(surface);
    }
}

// Reminder: x_o, y_o is the top-left origin
void AGLTextRenderer::renderText(char *str, int x_o, int y_o) {
    for (char *c = str; (*c) != '\0'; ++c) {
     renderChar(*c, x_o, y_o);
     // get next x_o-position 
     x_o += get_glyph_at(*c).get_advance();
    }
}

// Reminder: x_o, y_o is the top-left origin
// Get Glyph - 
void AGLTextRenderer::renderChar(char letter, int x_o, int y_o) {
    Glyph glyph = get_glyph_at(letter);
    // We want to draw text over our scene, so no need of Depth Testing 
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    //Blending 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, glyph.get_textureID());
    
    // Draw texture with quads 
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 0);
        glVertex2f(x_o - m_font_outline, y_o + (m_font_height - m_font_outline));
        glTexCoord2f(0, 1);
        glVertex2f(x_o - m_font_outline, y_o - m_font_outline);
        glTexCoord2f(1, 1);
        glVertex2f((x_o + glyph.get_maxX() - m_font_outline), (y_o - m_font_outline));
        glTexCoord2f(1, 0);
        glVertex2f((x_o + glyph.get_maxX() - m_font_outline), (y_o + m_font_height - m_font_outline));
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



AGLTextRenderer::~AGLTextRenderer() {
    TTF_CloseFont(m_font_ptr);
}

} // namespace