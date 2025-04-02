#include "qmesboxwidget.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QGuiApplication>
#include <QTimer>
#include <QMutex>
#include <QScreen>
#include <QMutexLocker>
#include <QCloseEvent>

//==========QMesBoxWidget============//


QMesBoxWidget* QMesBoxWidget::mP_instance = nullptr; //初始化 静态实例
QMutex QMesBoxWidget::m_mutex;                       //锁
/**
 * @brief QMesBoxWidget 构造函数
 * @param text  消息 message
 * @param m_AnimationInTime 动画加载时间
 * @param m_AnimationOutTime 动画退出时间
 * @param m_AnimationDispalyTime 窗口保持时间
 */
QMesBoxWidget::QMesBoxWidget():
    QWidget(nullptr)
{
    initUI();
    initAnimation();
    initTimer();
}
/**
 * @brief QMesBoxWidget::closeEvent
 * 关闭事件重写，释放资源
 * Disable event rewriting to free resources
 */
void QMesBoxWidget::closeEvent(QCloseEvent *event){
    hideTimer->stop();
    event->accept();  // 接受关闭事件
}

/**
 * @brief QMesBoxWidget::initUI
 * 初始化UI，跟随屏幕设置窗口大小
 * Initialize the UI and follow the screen to set the window size
 */
void QMesBoxWidget::initUI(){
    //获取屏幕分辨率
    QScreen *screen = QGuiApplication::primaryScreen();
    if(!screen){
        this->setFixedSize(300,160);
        qDebug()<<"Unable to retrieve screen resolution";
    }else{
        QRect screenGeometry = screen->availableGeometry();
        int screenWidth = screenGeometry.width();
        int screenHeight = screenGeometry.height();
        // 设置弹窗大小为屏幕大小的比例
        double widthRatio = 0.2;  // 弹窗宽度占屏幕宽度
        double heightRatio = 0.2; // 弹窗高度占屏幕高度
        int windowWidth = screenWidth * widthRatio;
        int windowHeight = screenHeight * heightRatio;
        this->setFixedSize(windowWidth,windowHeight);
    }

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);  // 去掉默认的标题栏,不在任务栏显示
    this->setAttribute(Qt::WA_TranslucentBackground, true);       // 背景透明

    // 设置主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    // 创建 frame 容器
    frame = new QFrame(this);
    frame->setObjectName("frame");
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    // 设置 frame 的布局
    frameLayout = new QVBoxLayout(frame);
    frameLayout->setSpacing(0);
    frameLayout->setContentsMargins(1, 1, 1, 1);
    frameLayout->setStretch(0, 2); // 设置布局拉伸因子
    frameLayout->setStretch(1, 5);

    // 创建标题区域
    titleArea = new QWidget(frame);
    titleArea->setObjectName("titleArea");
    titleArea->setMinimumSize(0,50);
    titleArea->setFixedHeight(28); // 减小标题栏的高度

    titleLayout = new QHBoxLayout(titleArea);
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setStretch(0, 2);
    titleLayout->setStretch(1, 2);
    titleLayout->setStretch(2, 1);

    // 标题标签
    titleLabel = new QLabel("提示", titleArea);
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    titleLabel->setIndent(15);

    // 倒计时标签
    countLabel = new QLabel("Close:" + QString::number(m_AnimationDispalyTime)+"s", titleArea);
    countLabel->setObjectName("countLabel");
    countLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // 关闭按钮
    btnClose = new QPushButton(titleArea);
    btnClose->setObjectName("btnClose");
    btnClose->setMinimumSize(16, 16);
    btnClose->setMaximumSize(25, 25);
    btnClose->setText("×");
    btnClose->setFont(QFont("Arial", 20, QFont::Bold));

    // 添加控件到标题布局
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(countLabel);
    titleLayout->addWidget(btnClose);

    // 添加标题区域到 frame 布局
    frameLayout->addWidget(titleArea);

    // 内容标签
    contentLabel = new QLabel(m_content, frame);
    contentLabel->setObjectName("contentLabel");
    contentLabel->setAlignment(Qt::AlignCenter);
    contentLabel->setWordWrap(true);
    frameLayout->addWidget(contentLabel);

    // 添加 frame 到主布局
    mainLayout->addWidget(frame);

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(-5, -5);              // 设置阴影偏移
    shadowEffect->setBlurRadius(20);           // 设置模糊半径
    shadowEffect->setColor(QColor(0, 0, 0, 160)); // 设置阴影颜色（带透明度）
    this->frame->setGraphicsEffect(shadowEffect);    // 应用阴影效果到 frame
    this->mainLayout->setContentsMargins(10, 10, 10, 10); // 让阴影四周都有距离
    frame->setGeometry(10, 10, this->width() - 20, this->height() - 20); // 确保frame大小合适

    // 绑定关闭按钮信号
    connect(btnClose, &QPushButton::clicked, this, &QWidget::close);
}

/**
  * @brief QMesBoxWidget::styleSheet
  * @param themeType 主题类型
  * @param widget    需要设置窗口
  * 样式表设置   Style sheet Settings
  */
void QMesBoxWidget::styleSheet(Theme themeType, QWidget *widget){
    if(!widget){
        return;
    }
    switch (themeType) {
    case ClassicTheme:
        // 设置样式
        widget->setStyleSheet(R"(
    #frame {
        background-color: rgb(70, 70, 70);  /* 深灰色背景 */
        border: none;
        border-radius: 12px;
    }
    #titleArea {
        background-color: rgb(90, 90, 90);  /* 深灰色稍微浅一点 */
        border-radius: 8px;
    }
    #titleLabel {
        background-color: transparent;
        color: rgb(255, 255, 255);  /* 白色文字 */
        font-size: 12px;
    }
    #contentLabel {
        background-color: transparent;
        padding: 10px 20px;
        color: rgb(200, 200, 200);  /* 浅灰色文字 */
        font-size: 12px;
    }
    #countLabel {
        background-color: transparent;
        color: rgb(100, 150, 255);  /* 浅蓝色文字 */
    }
    #btnClose {
        background: transparent;
        border: none;
        border-radius: 10px;
        padding: 5px;
    }
    #btnClose:hover {
        background-color: rgb(255, 80, 80);  /* 橙红色背景 */
        color: white;
    }
    #btnClose:pressed {
        background-color: rgb(220, 50, 50);  /* 按下时的红色 */
    }
)");
        break;
    case LightTheme:

        widget->setStyleSheet(R"(
    #frame {
        background-color: rgb(255, 255, 255);  /* 白色背景 */
        border: none;
        border-radius: 12px;
    }
    #titleArea {
        background-color: rgb(240, 240, 240);  /* 浅灰色 */
        border-radius: 8px;
    }
    #titleLabel {
        background-color: transparent;
        color: rgb(0, 0, 0);  /* 黑色文字 */
        font-size: 12px;
    }
    #contentLabel {
        background-color: transparent;
        padding: 10px 20px;
        color: rgb(80, 80, 80);  /* 深灰色文字 */
        font-size: 12px;
    }
    #countLabel {
        background-color: transparent;
        color: rgb(0, 120, 255);  /* 蓝色文字 */
    }
    #btnClose {
        background: transparent;
        border: none;
        border-radius: 10px;
        padding: 5px;
    }
    #btnClose:hover {
        background-color: rgb(255, 80, 80);  /* 橙红色背景 */
        color: white;
    }
    #btnClose:pressed {
        background-color: rgb(220, 50, 50);  /* 按下时的红色 */
    }
)");

        break;
    case DarkTheme:
        widget->setStyleSheet(R"(
    #frame {
        background-color: rgb(30, 30, 30);  /* 深灰色背景 */
        border: none;
        border-radius: 12px;
    }
    #titleArea {
        background-color: rgb(50, 50, 50);  /* 稍浅的深灰色 */
        border-radius: 8px;
    }
    #titleLabel {
        background-color: transparent;
        color: rgb(255, 255, 255);  /* 白色文字 */
        font-size: 12px;
    }
    #contentLabel {
        background-color: transparent;
        padding: 10px 20px;
        color: rgb(200, 200, 200);  /* 浅灰色文字 */
        font-size: 12px;
    }
    #countLabel {
        background-color: transparent;
        color: rgb(100, 150, 255);  /* 浅蓝色文字 */
    }
    #btnClose {
        background: transparent;
        border: none;
        border-radius: 10px;
        padding: 5px;
    }
    #btnClose:hover {
        background-color: rgb(255, 80, 80);  /* 橙红色背景 */
        color: white;
    }
    #btnClose:pressed {
        background-color: rgb(220, 50, 50);  /* 按下时的红色 */
    }
)");
        break;
    }
}

/**
 * @brief QMesBoxWidget::initAnimation
 * 初始化动画，将所需动画添加到动画组中
 * Initializes the animation, adding the desired animation to the animation group
 */
void QMesBoxWidget::initAnimation(){
    quint32 inTime = m_AnimationInTime*1000;
    quint32 outTime = m_AnimationOutTime*1000;
    mode = AnimationMode::AllAnimation;  // 设置启用透明度和位置动画
    m_aniGroupIn = new QParallelAnimationGroup(this);
    m_aniGroupOut = new QParallelAnimationGroup(this);
    //透明度动画
    m_opacityIn = new QPropertyAnimation(this,"windowOpacity");
    m_opacityOut = new QPropertyAnimation(this,"windowOpacity");
    //判断是否设置了透明度动画
    if(mode&AnimationMode::OpacityAnimation){
        m_opacityIn->setDuration(inTime);
        m_opacityIn->setStartValue(0);
        m_opacityOut->setDuration(outTime);
        m_opacityOut->setStartValue(0);
    }else{
        m_opacityIn->setDuration(0);
        m_opacityIn->setStartValue(1);
        m_opacityOut->setDuration(0);
        m_opacityOut->setStartValue(1);
    }
    m_opacityIn->setEndValue(1);
    m_opacityOut->setEndValue(1);
    m_aniGroupIn->addAnimation(m_opacityIn);
    m_aniGroupOut->addAnimation(m_opacityOut);
    //位置动画
    m_positionIn = new QPropertyAnimation(this,"pos");
    m_positionOut = new QPropertyAnimation(this,"pos");
    QScreen* screen = QGuiApplication::primaryScreen();
    if(screen){
        const QRect desk_rect = screen->availableGeometry();
        const QPoint hide_pos{ desk_rect.width() - this->width(),
                              desk_rect.height() };
        const QPoint position{ desk_rect.width() - this->width(),
                              desk_rect.height() - this->height() };
        //判断是否设置了此模式的动画
        if (mode&AnimationMode::PosAnimation){
            m_positionIn->setDuration(inTime);
            m_positionIn->setStartValue(hide_pos);
            m_positionOut->setDuration(outTime);
            m_positionOut->setStartValue(hide_pos);
        }
        else{
            m_positionIn->setDuration(0);
            m_positionIn->setStartValue(position);
            m_positionOut->setDuration(0);
            m_positionOut->setStartValue(position);
        }
        m_positionIn->setEndValue(position);
        m_positionOut->setEndValue(position);
    }
    m_aniGroupIn->addAnimation(m_positionIn);
    m_aniGroupOut->addAnimation(m_positionOut);

}

/**
 * @brief QMesBoxWidget::initTimer
 * 初始化定时器，设置定时器为1S，通过hidecount控制倒计时
 * Initialize the timer, set the timer to 1S, and control the countdown through hidecount
 */
void QMesBoxWidget::initTimer(){
    hideTimer = new QTimer();
    hideTimer->setInterval(1000);
    connect(hideTimer,&QTimer::timeout,[this]{
        if(hideCount >=  1){
            hideCount--;
            QString text = QStringLiteral("Close:%1s").arg(hideCount);
            this->countLabel->setText(text);
        }else{
            hideTimer->stop();
            animationOut();
        }
    });
}

/**
 * @brief QMesBoxWidget::stopAnimation
 * 停止动画和定时器
 * Stop the animation and the timer
 */
void QMesBoxWidget::stopAnimation()
{
    m_aniGroupIn->stop();
    m_aniGroupOut->stop();
    hideTimer->stop();
}
/**
 * @brief QMesBoxWidget::checkTime
 * check 动画时间是否改变
 * Whether the animation time is changed
 */
bool QMesBoxWidget::checkTime()
{
    if(m_AnimationInTime != m_temporaryAnimaIn || m_AnimationOutTime != m_temporaryAnimaOut || m_AnimationDispalyTime != m_temporaryAnimaDis){
        m_AnimationInTime = m_temporaryAnimaIn;
        m_AnimationOutTime = m_temporaryAnimaOut;
        m_AnimationDispalyTime = m_temporaryAnimaDis;
        return true;
    }
    return false;
}

/**
 * @brief QMesBoxWidget::animationIn
 * 动画进入，正向加载动画组，动画结束启动定时器
 * The animation enters, the animation group is loaded forward, and the timer is started when the animation ends
 */
void QMesBoxWidget::animationIn(){
    hideCount = m_AnimationDispalyTime;
    QString text = QStringLiteral("Close:%1s").arg(hideCount);
    this->countLabel->setText(text);
    this->setWindowModality(Qt::WindowModal);
    if(m_aniGroupIn && m_aniGroupIn->state() != QPropertyAnimation::Running){
        m_aniGroupIn->setDirection(QAbstractAnimation::Forward);
        m_aniGroupIn->start();
        connect(m_aniGroupIn,&QPropertyAnimation::finished,this,[this](){
            hideTimer->start();
        });
    }
}

/**
 * @brief QMesBoxWidget::animationOut
 * 动画退出，反向加载动画组，动画结束关闭窗口
 * The animation exits, the animation group is reversely loaded, and the animation window is closed
 */
void QMesBoxWidget::animationOut(){
    this->setWindowModality(Qt::WindowModal);
    if( m_aniGroupOut && m_aniGroupOut->state() != QPropertyAnimation::Running){
        m_aniGroupOut->setDirection(QAbstractAnimation::Backward);
        m_aniGroupOut->start();
        connect(m_aniGroupOut,&QPropertyAnimation::finished,this,[this]{
            this->close();
        });
    }
}

/**
 * @brief QMesBoxWidget::show
 * 自定义show方法   Customize the show method
 */
void QMesBoxWidget::show(){

    animationIn();
    QWidget::show();
}

/**
 * @brief QMesBoxWidget::MesBox       静态调用方法     Statically invoking methods
 * @param themeType                   主题类型        Theme type
 * @param title                       标题名称        Title name
 * @param text                        消息文本        Message text
 * @param AniInTime                   动画进入时间     Animation entry time
 * @param AniOutTime                  动画退出时间     Animation exit time
 * @param KeepTime                    窗口保持时间     Window hold time
 */
void QMesBoxWidget::MesBox(Theme themeType,const QString& title,const QString& text,quint32 AniInTime,quint32 AniOutTime,quint32 KeepTime){
    if(nullptr == mP_instance){
        QMutexLocker locker(&m_mutex);
        if(nullptr == mP_instance){
            mP_instance = new QMesBoxWidget();
        }
    }else{
        mP_instance->stopAnimation();
        mP_instance->hide();
    }
    if(nullptr == mP_instance){
        return;
    }
    mP_instance->m_AnimationInTime = AniInTime;
    mP_instance->m_AnimationOutTime = AniOutTime;
    mP_instance->m_AnimationDispalyTime = KeepTime;
    mP_instance->initAnimation();
    mP_instance->styleSheet(themeType,mP_instance);
    mP_instance->titleLabel->setText(title);
    mP_instance->contentLabel->setText(text);
    mP_instance->raise();
    mP_instance->show();
}

/**
 * @brief QMesBoxWidget::setMesBox      静态主题与时间设置方法    Static theme and time setting methods
 * @param themeType                     主题类型                Theme type
 * @param AniInTime                     动画进入时间             Animation entry time
 * @param AniOutTime                    动画退出时间             Animation exit time
 * @param KeepTime                      窗口保持时间             Window hold time
 */
void QMesBoxWidget::setMesBox(Theme themeType, quint32 AniInTime, quint32 AniOutTime, quint32 KeepTime)
{
    if(nullptr == mP_instance){
        QMutexLocker locker(&m_mutex);
        if(nullptr == mP_instance){
            mP_instance = new QMesBoxWidget();
        }
    }else{
        mP_instance->stopAnimation();
        mP_instance->hide();
    }
    if(nullptr == mP_instance){
        return;
    }

    mP_instance->m_theme = themeType;
    mP_instance->m_AnimationInTime = AniInTime;
    mP_instance->m_AnimationOutTime = AniOutTime;
    mP_instance->m_AnimationDispalyTime = KeepTime;
    mP_instance->m_temporaryAnimaIn = AniInTime;
    mP_instance->m_temporaryAnimaOut = AniOutTime;
    mP_instance->m_temporaryAnimaDis = KeepTime;
    mP_instance->initAnimation();
}

/**
 * @brief QMesBoxWidget::MesBox         通用静态调用方法     Generic static invocation methods
 * @param title                         标题名称            Title name
 * @param text                          消息文本            Message text
 */
void QMesBoxWidget::MesBox(const QString &title, const QString &text)
{
    if(nullptr == mP_instance){
        QMutexLocker locker(&m_mutex);
        if(nullptr == mP_instance){
            mP_instance = new QMesBoxWidget();
        }
    }else{
        mP_instance->stopAnimation();
        mP_instance->hide();
    }
    if(nullptr == mP_instance){
        return;
    }
    if(mP_instance->checkTime()){
        mP_instance->initAnimation();
    }
    mP_instance->styleSheet(mP_instance->m_theme,mP_instance);
    mP_instance->titleLabel->setText(title);
    mP_instance->contentLabel->setText(text);
    mP_instance->raise();
    mP_instance->show();
}

