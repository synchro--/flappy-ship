#ifndef _ELEMENTS_HXX_
#define _ELEMENTS_HXX_

#include "agl.hxx"
#include "log.hxx"
#include "types.hxx"

/*
* elements.hxx describes the different objects (elements) of the game
* Floor, Sky, ..., it will be expanded as the game gets more complex
* and new elements gets added.
*/

namespace elements {

  /*
   * The floor.
   * Construcor loads the texture and stores it in m_tex
   * Render function draw this tex
   * get_floor returns a singleton instance. This behaviour can be changed
   * in case we want to expand the game and get more instances of floors (or planes).
   */

  class Floor {
    private:
        float m_size, m_height;
        agl::env &m_env; //reference to env, needed in the constructor
        agl::Texture m_tex; //floor texture

        //construct the floor loading the texture
        Floor(const char *texture_filename);

    public:
        //friend function to load the texture and create a singleton
        friend Floor* get_floor(const char *filename);

        void render();
  }

  //get singleton instance of floor
  Floor* get_floor(const char *filename);

  //The Sky. Same mechanism as above
  class Sky {
    private:
        agl::env &m_env;
        agl::Texture m_tex;
        double m_radius;
        int m_lats, m_longs;

        //construct the floor loading the texture
        Sky(const char *texture_filename, double radius);

    public:
        //friend function to load the texture and create a singleton
        friend Sky* get_sky(const char *filename);

        void render();

        //accessors
        void set_params(double radius = 100.0,
                           int lats = 20, int longs = 20);

        inline decltype(m_radius) radius() { return m_radius};
  }

  //get singleton instance of floor
  Floor* get_sky(const char *filename);


  /*
   TODO class RING coming soon
   oppure trovare un'altra idea
   tipo paletti in cui si passa di fianco dalla direzione corretta
   e si fa quindi una sorta di slalom --> SpaceSlalom

   - come stabilire la direzione corretta e rilevera se si passa l'ostacolo dalla
   - parte giusta ? discutere con cillo
  */

}

#endif //_ELEMENTS_HXX_
