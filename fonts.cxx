#include "fonts.h"

namespace fonts {
    
    AGLTextRenderer& getTextRenderer(const char *font_path, size_t font_size) {
        const static auto TAG = __func__; 
        lg::i(TAG, "Creating a text renderer, loading the font from %s", font_path);
        
      // having a unique ptr ensures the TextRenderer will be called only during the main
      static std::unique_ptr<AGLTextRenderer> s_ptr(nullptr); 
      if(!s_ptr) {
          s_ptr.reset(new AGLTextRenderer(font_path, font_size)); 
      }
    
    return s_ptr.get(); 
    }
    
   
}



AGLTextRenderer::AGLTextRenderer(char *font_path, size_t fontSize) {
    const static auto TAG = __func__; 
    
    // Init font for text drawing
        if (TTF_Init() == -1) {
        lg::e(TAG, "Cannot init font %s", font_path)
        // exit(1);
        }
    
    font = TTF_OpenFont(fontPath, fontSize);
        if (!font) {
        fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
        }
    // disable kerning since we are not able to make any use of it
    TTF_SetFontKerning(font, 0);
    
    // get font outline
    outline = TTF_GetFontOutline(font);
    fontHeight = TTF_FontHeight(font);
    loadTextureVector();
}


void AGLTextRenderer::loadTextureVector() {
    SDL_Surface *surface;
    SDL_Color color = {255, 255, 255, 255};
    int maxy, miny, advance, minx, maxx;
    GLuint texture;
    for (char i = 0x20; i < 0x7F; ++i) {
    // cache glyph metrics and texture
    TTF_GlyphMetrics(font, i, &minx, &maxx, &miny, &maxy, &advance);
    if (!(surface = TTF_RenderGlyph_Blended(font, i, color))) {
    fprintf(stderr, "%s\n", TTF_GetError());
    }
    glGenTextures(1, &texture);
    Glyph glyph(i, advance, miny, maxy, minx, maxx, texture);
    glBindTexture(GL_TEXTURE_2D, glyph.getTextureID());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glyphs.push_back(glyph);
    SDL_FreeSurface(surface);
    }
}

// Reminder: x_o, y_o is the top-left origin
void TextRenderer::renderText(char *str, int x_o, int y_o) {
    for (char *c = str; (*c) != '\0'; ++c) {
     renderChar(*c, x, y);
     x += getGlyph(*c).getAdvance();
    }
}

// Reminder: x_o, y_o is the top-left origin
void AGLTextRenderer::renderChar(char letter, int x_o, int y_o) {
    Glyph glyph = getGlyph(letter);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, glyph.getTextureID());
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x - outline, y + fontHeight - outline);
    glTexCoord2f(0, 1);
    glVertex2f(x - outline, y - outline);
    glTexCoord2f(1, 1);
    glVertex2f(x + glyph.getMaxX() - outline, y - outline);
    glTexCoord2f(1, 0);
    glVertex2f(x + glyph.getMaxX() - outline, y + fontHeight - outline);
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

int AGLTextRenderer::get_width(const char *str) {
    int width;
    TTF_SizeText(font, str, &width, NULL);
    return width;
}



void AGLTextRenderer::~AGLTextRenderer() {
    TTF_CloseFont(m_font);
   // TTF_Quit(); this is in Env, should it be moved? 
}