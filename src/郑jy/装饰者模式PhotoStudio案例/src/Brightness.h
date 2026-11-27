#pragma once

#include "EffectDecorator.h"

//具体装饰器，默认将 RGB 三个通道各提高 60。
class Brightness : public EffectDecorator {
public:
    explicit Brightness(PhotoPtr photo, int delta = 60);

    std::string getDescription() const override;
    Image render() const override;

private:
    int delta;
};
