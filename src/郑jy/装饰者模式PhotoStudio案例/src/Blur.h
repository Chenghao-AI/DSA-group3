#pragma once

#include "EffectDecorator.h"

//具体装饰器，使用有效邻域的均值模糊图像。
class Blur : public EffectDecorator {
public:
    explicit Blur(PhotoPtr photo, int radius = 0);

    std::string getDescription() const override;
    Image render() const override;

private:
    //半径为 0 时，按照片尺寸决定实际半径。
    int radius;
};
