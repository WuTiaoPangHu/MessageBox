#ifndef QMESBOXWIDGET_H
#define QMESBOXWIDGET_H
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class QPropertyAnimation;
class QParallelAnimationGroup;
class QMutex;
//========class QMesBoxWidget========//
/**
 * @brief 主题枚举
 * 提供三种主题：默认主题、亮色主题、暗色主题
 *
 * @brief The Theme enum
 *There are three themes available: default theme, light theme, and dark theme
 */

enum Theme{
    ClassicTheme,                                   //默认主题
    LightTheme,                                     //亮色主题
    DarkTheme                                       //暗色主题
};
/**
 * @class QMesBoxWidget
 * @brief 自定义右下角消息提示框  Customize the message prompt box in the lower right corner
 * 该类提供了一个消息框样式，支持不同主题（如亮色和暗色主题），
 * 具备从屏幕外升入屏幕右下角的动画效果，可修改窗口进程、退场动画时长，
 * 以及消息框持续时间。此类适用于提示用户信息、警告和成功消息的应用场景。
 * 主要功能：
 *  - 支持自定义主题颜色切换，满足不同界面风格需求；
 *  - 支持自定义标题名称；
 *  - 提供动画效果，添加阴影效果；
 *  - 可根据需求设置消息框显示时间，控制消息框停留时间，并为用户显示剩余时间；
 *  - 提供关闭按钮，允许用户手动关闭；
 *  - 自动获取屏幕分辨率，消息框大小随分辨率改变。
 * @brief This class provides a message box style that supports different themes (such as bright and dark themes),
 * With the animation effect of rising from the screen to the bottom right corner of the screen, you can modify the window process, exit animation duration,
 * And the message box duration. This class applies to scenarios where user information, warnings, and success messages are displayed.
 * Main functions:
 *  - Support custom theme color switching to meet different interface style needs;
 *  - Support custom title name;
 *  - Provide animation effect, add shadow effect;
 *  - The display time of the message box can be set according to the demand, the residence time of the message box can be controlled,
 *  and the remaining time can be displayed for the user;
 *  - Provide a close button to allow users to manually close;
 *  - Automatically gets the screen resolution, the message box size changes with the resolution.
 * @author [Rain]
 * @version 1.0
 * @date [2024/12/16]
 */
class QMesBoxWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief The AnimationMode enum
     */
    enum AnimationMode:int{
        NoAnimation = 0x00,                                                     //无动画
        OpacityAnimation = 0x01,                                                //仅透明度动画
        PosAnimation = 0x02,                                                    //仅位置动画
        AllAnimation = 0xFF                                                     //全部动画 OpacityAnimation|PosAnimation
    };

public:
    /**
     * @brief MesBox            静态调用方法，带主题设置和动画时间与不带两种，默认为 ClassicTheme in 1 out 1 keep 3
     * @brief setMesBox         静态主题设置方法，可设置主题和动画时间以及保持时间，全局控制
     * @param themeType         主题
     * @param title             标题
     * @param text              文本
     * @param AniInTime         动画进入时间
     * @param AniOutTime        动画退出时间
     * @param KeepTime          窗口保持时间
     */
    static void MesBox(Theme themeType,const QString& title,const QString& text,
                             quint32 AniInTime,quint32 AniOutTime,quint32 KeepTime);//静态调用方法
    static void setMesBox(Theme themeType,quint32 AniInTime,quint32 AniOutTime,quint32 KeepTime);//设置主题 加载、退出、保持时间
    static void MesBox(const QString& title,const QString& text);               //通用静态方法




//===================private========================//
private:
    explicit QMesBoxWidget();
    void show();
    static void styleSheet(Theme themeType,QWidget *widget);                   //主题切换
    void closeEvent(QCloseEvent *event) override;                              //关闭时间重载
    void animationIn();                                                         // 动画进入
    void animationOut();                                                        // 动画退出

    void initUI();                                                              // 初始化UI
    void initAnimation();                                                       // 初始化动画
    void initTimer();                                                           // 初始化定时器

    void stopAnimation();                                                       // 中断动画与定时器
    bool checkTime();                                                           // 检测时间
private:
    AnimationMode mode;                                                         // 动画类型
    QParallelAnimationGroup* m_aniGroupIn = nullptr;                            // 进入动画组
    QParallelAnimationGroup* m_aniGroupOut = nullptr;                           // 退出动画组
    QPropertyAnimation* m_opacityIn = nullptr,* m_positionIn = nullptr,
                     * m_opacityOut = nullptr,* m_positionOut = nullptr;         // 透明度动画 位置动画
    QTimer* hideTimer = nullptr;                                                 // 定时器
    Theme m_theme = ClassicTheme;                                                // 默认主题

    QVBoxLayout *mainLayout;
    QFrame *frame;
    QVBoxLayout *frameLayout;
    QWidget *titleArea;
    QHBoxLayout *titleLayout;
    QLabel *titleLabel;
    QLabel *countLabel;
    QPushButton *btnClose;
    QLabel *contentLabel;

    quint32 hideCount;                                                          // 倒计时
    quint32 m_AnimationInTime = 1;                                              // 动画加载时间
    quint32 m_AnimationOutTime = 1;                                             // 动画退出时间
    quint32 m_AnimationDispalyTime = 3;                                         // 窗口显示时间
    quint32 m_temporaryAnimaIn = 1;
    quint32 m_temporaryAnimaOut = 1;
    quint32 m_temporaryAnimaDis = 3;
    QString m_content;                                                          // 文本

    static QMesBoxWidget* mP_instance;                                          //静态实例
    static QMutex m_mutex;                                                      //锁
};

#endif // QMESBOXWIDGET_H
