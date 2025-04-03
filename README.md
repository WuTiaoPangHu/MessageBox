# MessageBox

MessageBox 是一个基于 Qt 的消息提示框，提供了以下特性：

## 特性
- **内置三种主题**（经典、浅色、深色）
- **支持主题切换**
- **可设置动画**（进入、退出）
- **窗口保持时间可配置**
- **仅支持静态调用**
- **自适应分辨率**
- **窗口右下角冒泡弹出**

## 使用方法

### 1. 添加头文件并调用静态方法
```cpp
#include "qmesboxwidget.h"

//  单次提示 不影响设置过的数据
QMesBoxWidget::MesBox(ClassicTheme, "提示", "这是一个消息提示框", 1, 1, 5);
// 设置主题及动画时间
QMesBoxWidget::setMesBox(LightTheme,1,2,5);
// 第二种方式 未进行设置 默认主题 动画时间默认1s 保持时间默认3s
QMesBoxWidget::MesBox("提示","这是第二种");
```

### 2. 主题类型
```cpp
enum Theme {
    ClassicTheme, // 经典主题
    LightTheme,   // 浅色主题
    DarkTheme     // 深色主题
};
```

### 3. 参数说明
```cpp
QMesBoxWidget::MesBox(Theme themeType, const QString& title, const QString& text,
                      quint32 AniInTime, quint32 AniOutTime, quint32 KeepTime);
```
- `themeType`：主题类型，可选 `ClassicTheme`、`LightTheme`、`DarkTheme`
- `title`：消息框标题
- `text`：消息内容
- `AniInTime`：动画进入时间（毫秒）
- `AniOutTime`：动画退出时间（毫秒）
- `KeepTime`：窗口保持时间（毫秒）

## 版本
开发使用Qt 6.3.2

## 效果
![image](https://github.com/user-attachments/assets/89eda80c-f2cd-45d1-9c55-ae2c7354d49c)
![image](https://github.com/user-attachments/assets/18ed3b9b-8d58-4bd2-ba82-6aaf40114595)


## 安装
1. 下载或克隆本项目：
   ```sh
   git clone https://github.com/WuTiaoPangHu/MessageBox.git
   ```
2. 在 Qt 项目中添加 `qmesboxwidget.cpp` 和 `qmesboxwidget.h`，也可以使用静态库的方式。
3. 直接调用 `QMesBoxWidget::MesBox(...)` 即可使用。
4. 静态块未编译，源码以及测试模块，需要静态库自行修改CMake进行编译。

## 许可证
本项目遵循 **MIT License**，可自由使用、修改和分发。

## 贡献
如果你有任何改进建议或发现了 Bug，欢迎提交 Issue 或 Pull Request，刚入门还是一个菜鸡各位大佬多多担待，QAQ。

---
**GitHub 地址**：[MessageBox](https://github.com/yourusername/MessageBox)

