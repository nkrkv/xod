
#include <stdlib.h>
#include <string.h>

char* formatNumber(double val, int width, bool pzero, unsigned int prec, char* sout) {
    int decpt, sign, reqd, pad;
    const char *s, *e;
    char *p;
    char padc;
    s = fcvt(val, prec, &decpt, &sign);
    if (prec == 0 && decpt == 0) {
        s = (*s < '5') ? "0" : "1";
        reqd = 1;
    } else {
        reqd = strlen(s);
        if (reqd > decpt) reqd++;
        if (decpt == 0) reqd++;
    }
    if (sign) reqd++;
    p = sout;
    e = p + reqd;
    pad = width - reqd;
    if (sign && pzero) *p++ = '-';
    if (pad > 0) {
        e += pad;
        padc = pzero ? '0' : ' ';
        while (pad-- > 0) *p++ = padc;
    }
    if (sign && !pzero) *p++ = '-';
    if (decpt <= 0 && prec > 0) {
        *p++ = '0';
        *p++ = '.';
        e++;
        while ( decpt < 0 ) {
            decpt++;
            *p++ = '0';
        }
    }
    while (p < e) {
        *p++ = *s++;
        if (p == e) break;
        if (--decpt == 0) *p++ = '.';
    }
    if (width < 0) {
        pad = (reqd + width) * -1;
        while (pad-- > 0) *p++ = ' ';
    }
    *p = 0;
    return sout;
}
