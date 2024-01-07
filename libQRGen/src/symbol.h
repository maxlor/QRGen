// Copyright 2024 Benjamin Lutz.
// 
// This file is part of QRGen. QRGen is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.

#ifndef SYMBOL_H
#define SYMBOL_H

#include <array>
#include <cstdint>
#include <vector>
#include "qrgen.h"


/**
 * Handle the drawing of the QR symbol, i.e. which pixels go where.
 */
class Symbol {
public:
    enum class PixelType : uint8_t { Unset, Data, Blank, FinderPattern, Separator, TimingPattern,
                                     AlignmentPattern, FormatInformation, VersionInformation };
    
    /**
     * Create a symbol with the given \a version. Versions must be in the
     * range 1-40, otherwise the symbol will not be valid (size() will return
     * 0).
     */
    Symbol(uint8_t version);
    
    /**
     * The symbol's size in pixels. This method returns a scalar which is
     * applicable to both the height and width, since QR Symbols are square.
     */
    size_t size() const;
    
    /** Returns the pixel data, row by row. */
    const std::vector<bool> pixels() const;
    
    /** Returns the pixel type, row by row. */
    const std::vector<PixelType> pixelType() const;
    
    /**
     * The value of the given pixel. The coordinates \a x and \a y must be
     * in the range [0, size()). For invalid coordinates, \c false is
     * returned.
     */
    bool pixel(int x, int y) const; 
    uint32_t highlight(int x, int y) const;
    
    void highlightCodeword(size_t codewordNo, uint32_t highlight);
    void setData(const std::vector<uint8_t> &data, QR::ErrorCorrection ec, uint8_t mask = 255);
    
private:
    struct Position {
        int x;
        int y;
        bool upwards;

        bool valid() const;
    };

    void drawAlignmentPatterns();
    void drawCodewords(const std::vector<uint8_t> &data, uint8_t mask);
    void drawDarkModule();
    void drawFinderPatterns();
    void drawFormatInformation(uint8_t mask, QR::ErrorCorrection ec);
    void drawTimingPatterns();
    void drawVersionInformation();

    unsigned int evaluate() const;
    unsigned int evaluateAdjacentSameColor() const;
    unsigned int evaluateSameColorBlocks() const;
    unsigned int evaluate11311Pattern() const;
    unsigned int evaluateDarkProportion() const;
        
    size_t toIndex(int x, int y) const;
    size_t toIndex(const Position &position) const;
    bool valid(int x, int y) const;
    void drawPixel(int x, int y, bool color, PixelType pixelType);
    void drawRect(int x, int y, int w, int h, bool color, PixelType pixelType);
    std::array<Position, 8> position(int codeword);
    Position nextPosition(Position position) const;
    Position startPosition() const;

    static uint_fast16_t formatInformation(uint8_t mask, QR::ErrorCorrection ec);
    
    const int _version;
    const int _size;
    std::vector<bool> _pixels;
    std::vector<PixelType> _pixelType;
    std::vector<uint32_t> _highlight;
};

#endif // SYMBOL_H
