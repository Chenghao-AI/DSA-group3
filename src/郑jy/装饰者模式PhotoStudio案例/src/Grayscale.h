#pragma once

#include "EffectDecorator.h"

//具体装饰器，把当前图像转为灰度图。
class Grayscale : public EffectDecorator {
public:
    explicit Grayscale(PhotoPtr photo);

    std::string getDescription() const override;
    Image render() const override;
};
