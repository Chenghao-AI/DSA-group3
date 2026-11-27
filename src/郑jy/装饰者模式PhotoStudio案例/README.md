# PhotoStudio 案例讲解说明

## 文件夹结构

| 路径 | 内容 |
| --- | --- |
| [PhotoStudio案例讲解.md](PhotoStudio案例讲解.md) | 装饰者模式的课堂讲解与配图 |
| [PhotoStudio案例.pptx](PhotoStudio案例.pptx) | 课堂演示文稿 |
| [CMakeLists.txt](CMakeLists.txt) | C++17 构建入口 |
| [src/](src/) | 抽象组件、具体照片、各装饰器、图像读写和演示代码 |
| [pictures/](pictures/) | 配图、实拍照片和运行结果 |
| [pictures/figures/code/](pictures/figures/code/) | 七张 macOS 深色窗口风格的 C++ 代码图，覆盖类定义、委托调用和客户端包装 |
| [pictures/figures/diagrams/](pictures/figures/diagrams/) | [可编辑的 draw.io 图稿](pictures/figures/diagrams/PhotoStudio.drawio)与四页导出的 PNG |
| [pictures/photos/](pictures/photos/) | 三张实拍输入照片 |
| [pictures/results/](pictures/results/) | 实际运行结果、对照图和[运行日志](pictures/results/demo-log.txt) |
| [pictures/results/full-process/](pictures/results/full-process/) | 三张照片的完整装饰结果与实拍总览 |
| [pictures/results/full-process/banff-steps/](pictures/results/full-process/banff-steps/) | 摩雷恩湖从原图到最后一层装饰的六张 PNG 与逐步处理对照图 |
| [pictures/results/order-experiments/](pictures/results/order-experiments/) | 四组换序实验，每组目录含两张结果 PNG 和对应的对照图 |
| [pictures/results/brightness-repeat/](pictures/results/brightness-repeat/) | 提亮一次和提亮两次的结果 |

### 源码入口

| 角色 | 声明 | 实现 |
| --- | --- | --- |
| 抽象组件 | [Photo.h](src/Photo.h) | [Photo.cpp](src/Photo.cpp) |
| 抽象装饰器 | [EffectDecorator.h](src/EffectDecorator.h) | [EffectDecorator.cpp](src/EffectDecorator.cpp) |
| 人像照片 | [PortraitPhoto.h](src/PortraitPhoto.h) | [PortraitPhoto.cpp](src/PortraitPhoto.cpp) |
| 风景照片 | [LandscapePhoto.h](src/LandscapePhoto.h) | [LandscapePhoto.cpp](src/LandscapePhoto.cpp) |
| 黑白滤镜 | [Grayscale.h](src/Grayscale.h) | [Grayscale.cpp](src/Grayscale.cpp) |
| 水印 | [Watermark.h](src/Watermark.h) | [Watermark.cpp](src/Watermark.cpp) |
| 相框 | [Frame.h](src/Frame.h) | [Frame.cpp](src/Frame.cpp) |
| 亮度 | [Brightness.h](src/Brightness.h) | [Brightness.cpp](src/Brightness.cpp) |
| 模糊 | [Blur.h](src/Blur.h) | [Blur.cpp](src/Blur.cpp) |

命令行入口见 [PhotoStudio.cpp](src/PhotoStudio.cpp)，批量示例与实测像素检查见 [PhotoStudioDemo.cpp](src/PhotoStudioDemo.cpp)。像素、图像矩阵、图像读写和结果输出的文件分别以 `Pixel`、`Image`、`ImageIO` 和 `RenderResult` 命名。

## 编译和运行

在本目录使用 Windows PowerShell、CMake、Ninja 和 MinGW-w64 的 `g++`。图像读写使用 Windows 自带的 WIC，读取 JPEG 或 PNG，输出 PNG。

```powershell
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\PhotoStudio.exe --demo | Tee-Object -FilePath pictures/results/demo-log.txt
```

批量示例默认从 `pictures/photos/` 读取照片，在 `pictures/results/` 下生成 18 张 PNG。自定义实验按处理顺序填写效果名称，使用 `--steps` 可同时保存每一层的结果。

```powershell
.\build\PhotoStudio.exe --steps landscape pictures/photos/landscape-banff.jpg pictures/results/custom/study.png grayscale watermark
```

## 教材与素材来源

- 类图、包装图和代码组织参考《Head First 设计模式》第 3 章。可查阅[O’Reilly 第 3 章](https://www.oreilly.com/library/view/head-first-design/9781492077992/ch03.html)和[教材作者的星巴兹源码](https://github.com/bethrobson/Head-First-Design-Patterns/tree/master/src/headfirst/designpatterns/decorator/starbuzz)。
- 埃琳 柯林斯人像来自 NASA，由 scikit-image 收录，资料页标注为公有领域。 [scikit-image 样例说明](https://scikit-image.org/docs/stable/api/skimage.data.html#skimage.data.astronaut)
- 摩雷恩湖由 Gorgo 拍摄并发布至公有领域，使用 1280×960 版本。 [Wikimedia Commons](https://commons.wikimedia.org/wiki/File:Moraine_Lake_17092005.jpg)
- 纽约航拍由 Carol M. Highsmith 拍摄，来自美国国会图书馆印刷与摄影部 Carol M. Highsmith 档案，资料页标注无已知发表限制。 [美国国会图书馆](https://www.loc.gov/pictures/item/2011631202/)
- 代码配图参考 Carbon 的语法高亮和窗口排版，以及 macOS 窗口的红、黄、绿按钮。 [Carbon 样式说明](https://carbon.now.sh/about) [Apple 窗口说明](https://support.apple.com/en-euro/guide/macbook-pro/apd7ae4404d0/2026/mac/27)
