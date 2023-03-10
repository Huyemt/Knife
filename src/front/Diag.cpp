/*
 * Author: Huyemt
 * Time: 2023/3/5
 */

#include "Diag.h"
#include <cstdarg>
#include <iostream>

namespace Front {

    void DiagException(const std::shared_ptr<Position>& position, const char* fmt, ...) {
        va_list ap;
        va_start(ap, fmt);

        int i = position->LineX;
        while (position->Source[i] != '\n') {
            fprintf(stderr, "%c", position->Source[i]);
            i++;
        }

        fprintf(stderr, "\n");
        fprintf(stderr, "%*s^ ", position->x-1, "");
        fprintf(stderr, "at line %d,column %d: ", position->y, position->x);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");

        va_end(ap);
        exit(0);
    }
}