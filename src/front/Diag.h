/*
 * Author: Huyemt
 * Time: 2023/3/5
 */

#ifndef KNIFE_DIAG_H
#define KNIFE_DIAG_H

#include "Lexer.h"

namespace Front {
    void DiagException(const std::shared_ptr<Position>& position, const char* fmt, ...);
}

#endif //KNIFE_DIAG_H
