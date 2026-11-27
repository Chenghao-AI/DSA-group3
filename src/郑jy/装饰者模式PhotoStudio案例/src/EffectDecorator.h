#pragma once

#include "Photo.h"

//抽象装饰器，对应星巴兹案例中的 CondimentDecorator。
class EffectDecorator : public Photo {
public:
    std::string getDescription() const override = 0;

protected:
    explicit EffectDecorator(PhotoPtr photo);

    //被包装的对象既可以是原图，也可以是另一个装饰器。
    const PhotoPtr photo;
};
