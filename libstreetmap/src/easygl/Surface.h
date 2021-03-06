#ifndef SURFACE_H
#define SURFACE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <cairo.h>
#include <cairo-xlib.h>

// This class implements a Cairo surface (buffer of pixel data for graphics)
// which can be initialized from a png file, and then the surface (bitmap)
// can be drawn anywhere on the screen and in many places on the screen
// if desired. Surfaces are created by the constructor from a filePath.
// The class reference counts how many copies of a surface have been
// made through assignment and copy construction, and deallocates the cairo
// data representing the surface only when the last copy is destroyed.

class Surface {
    public:
        Surface();
        Surface(const char* filePath);
        ~Surface();

        // assignment operator
        Surface& operator=(const Surface& rhs);
        // cctor
        Surface(const Surface& surface);

        void setSurface(const char* filePath);

        // Note: getSurface will return NULL if never properly initialized
        cairo_surface_t* getSurface() const;

    private:
        cairo_surface_t* mSurface;

        void safeDestroySurface();

        // static hash table to keep track of surfaces (reference count)
        static std::unordered_map<cairo_surface_t*, unsigned> surfaces;
};

#endif
