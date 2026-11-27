#include "EffectDecorator.h"

#include <stdexcept>
#include <utility>

EffectDecorator::EffectDecorator(PhotoPtr wrapped)
    : photo(std::move(wrapped)) {
    if (!photo) {
        throw std::invalid_argument("An effect must wrap a Photo.");
    }
}
