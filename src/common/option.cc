#include "common/option.h"
#include "util/comparator.h"


namespace koishidb {
    Option::Option(): cmp(new Comparator()) {}
};