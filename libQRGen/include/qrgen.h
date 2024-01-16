// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#ifndef QRGEN_H
#define QRGEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>


#if defined(_WIN32)
# define QRGEN_DECL_EXPORT __declspec(dllexport)
# define QRGEN_DECL_IMPORT __declspec(dllimport)
#else
# define QRGEN_DECL_EXPORT
# define QRGEN_DECL_IMPORT
#endif

#ifdef LIBQRGEN_COMPILE
#define QRGEN_EXPORT QRGEN_DECL_EXPORT
#else
#define QRGEN_EXPORT QRGEN_DECL_IMPORT
#endif


enum QRGen_ErrorCorrection { QRGen_EC_L = 0, QRGen_EC_M = 1, QRGen_EC_Q = 2, QRGen_EC_H = 3 };


/**
 * Contains a QR Code.
 */
struct QRGen_Symbol {
    int width;  ///< The width of the QR code in number of pixels
    int height; ///< The height of the QR code in number of pixels
    
    /**
     * The pixel data as a bool array. \a data contains the pixels as bools,
     * row-by-row top to bottom, and left-to-right within a row. \c true
     * corresponds to a black pixel, \c false to a white one.
     * 
     * There are a total of \a width * \a height elements in \a data.
     */
    bool *data;
};


/**
 * Create a QR code from \a data using defaults. The defaults are:
 *   - error correction level M
 *   - use best mask
 *   - use smallest version possible.
 *   
 * A pointer to a QRGen_Symbol struct is returned. It's width and height are
 * is non-zero on success, and are both 0 if the QR code could not be created.
 * 
 * The returned symbol must be deallocated using QRGen_free_symbol().
 * 
 * @param data   an UTF-8-encoded string
 * @param len    the number of bytes in \a data.
 * @param the QR Code, or a QRGen_Symbol struct filled with zeroes.
 */
struct QRGen_Symbol *QRGen_encode(const char *data, size_t len) QRGEN_EXPORT;


/**
 * Same as QRGen_encode(), but also lets you specify the error correction
 * level which is used.
 * 
 * @see ::QRGen_Symbol
 */
struct QRGen_Symbol *QRGen_encode_ec(const char *data, size_t len, QRGen_ErrorCorrection ec) QRGEN_EXPORT;


/** Free the memory used by \a symbol. */
void QRGen_free_symbol(QRGen_Symbol *symbol) QRGEN_EXPORT;



#ifdef __cplusplus
} // extern "C"
#endif

#endif // QRGEN_H
