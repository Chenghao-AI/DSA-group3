#pragma once

#include "EffectDecorator.h"

//具体装饰器，在照片中心及右下角添加红色标记。
class Watermark : public EffectDecorator {
public:
    explicit Watermark(PhotoPtr photo);

    std::string getDescription() const override;
    Image render() const override;
};
