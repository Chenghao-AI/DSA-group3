#pragma once

#include "EffectDecorator.h"

//具体装饰器，把照片边缘涂成绿色。
class Frame : public EffectDecorator {
public:
    explicit Frame(PhotoPtr photo);

    std::string getDescription() const override;
    Image render() const override;
};
