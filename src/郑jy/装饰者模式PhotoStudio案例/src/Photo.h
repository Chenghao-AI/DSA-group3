#pragma once

#include "Image.h"

#include <memory>
#include <string>

//抽象组件，对应星巴兹案例中的 Beverage。
class Photo {
public:
    virtual ~Photo() = default;

    virtual std::string getDescription() const;
    virtual Image render() const = 0;

protected:
    std::string description = "Unknown Photo";
};

//具体照片和装饰器都使用同一个指针类型。
using PhotoPtr = std::shared_ptr<Photo>;
