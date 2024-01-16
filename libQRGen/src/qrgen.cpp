#include <qrgen.h>
#include <cstdio>
#include <codecvt>
#include <locale>
#include <string>
#include "qr.h"

using namespace std;


static QRGen_Symbol *convertSymbol(const Symbol &symbol);
static u16string fromUtf8(const char *data, size_t len);


extern "C" {

QRGen_Symbol *QRGen_encode(const char *data, size_t len) {
    Symbol symbol = QR::encode(fromUtf8(data, len));
    return convertSymbol(symbol);
}


struct QRGen_Symbol *QRGen_encode_ec(const char *data, size_t len, QRGen_ErrorCorrection ec) {
    Symbol symbol = QR::encode(fromUtf8(data, len), ec);
    return convertSymbol(symbol);
}


void QRGen_free_symbol(QRGen_Symbol *symbol) {
    if (symbol) {
        if (symbol->data) {
            free(symbol->data);
            symbol->data = nullptr;
        }
        free(symbol);
    }
}

} // extern "C"


static QRGen_Symbol *convertSymbol(const Symbol &symbol) {
    const int size = symbol.size();
    
    if (size == 0) {
        return nullptr;
    }
    
    QRGen_Symbol *result = reinterpret_cast<QRGen_Symbol*>(malloc(sizeof(QRGen_Symbol)));
    if (result == nullptr) {
        perror("malloc");
        return nullptr;
    }
    
    result->width = size;
    result->height = size;
    result->data = reinterpret_cast<bool*>(malloc(sizeof(bool) * size * size));
    if (result->data == nullptr) {
        perror("malloc");
        free(result);
        return nullptr;
    }
    
    // Cannot use memcpy because symbol.pixels() is packed, but result->data is not.
    for(size_t i = 0; i < symbol.pixels().size(); ++i) {
        result->data[i] = symbol.pixels()[i];
    }
    
    return result;
}


static u16string fromUtf8(const char *data, size_t len) {
    using CVT = codecvt_utf8_utf16<char16_t>;
    return wstring_convert<CVT, char16_t>{}.from_bytes(&data[0], &data[len]);
}
