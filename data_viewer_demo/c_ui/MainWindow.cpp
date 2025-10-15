﻿#include "c_ui/MainWindow.h"
#include "c_ui/workbenches/DocumentPage.h"   
#include <QApplication>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QStyle>
#include <QStatusBar>
#include <QMouseEvent>
#include <QEvent>
#include <QStringList>

#if USE_VTK
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#endif

// 构造函数
CTViewer::CTViewer(QWidget* parent)
    : QMainWindow(parent)
{
    // ---- 无边框窗口 + 深色主题 ----
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowTitle(QStringLiteral("CTViewer_demo"));
    setStyleSheet(QStringLiteral(
        "QMainWindow{background-color:#121212;}"
        "QMenuBar, QStatusBar{background-color:#1a1a1a; color:#e0e0e0;}"));

    // ---- 搭建结构 ----
    buildTitleBar();
    buildCentral();     // DocumentPage
    wireSignals();      // 连接信号
    setDefaults();      // 默认状态

    statusBar()->showMessage(QStringLiteral("就绪"));
}

CTViewer::~CTViewer() = default;



// 自定义标题栏

void CTViewer::buildTitleBar()
{
    //拼接新的标签栏
	auto* topBarContainer = new QWidget(this);
	auto* topBarLayout = new QVBoxLayout(topBarContainer);
    topBarLayout->setContentsMargins(0, 0, 0, 0);//这句话的作用是去掉边框
	topBarLayout->setSpacing(0);//这句话的作用是去掉边框

    topBarContainer->setAttribute(Qt::WA_StyledBackground, true);
    topBarContainer->setStyleSheet(QStringLiteral("QWidget{background-color:#202020;}"));

    titleBar_ = new QWidget(topBarContainer);
    titleBar_->setAttribute(Qt::WA_StyledBackground, true);
    titleBar_->setObjectName(QStringLiteral("customTitleBar"));
    titleBar_->setFixedHeight(38);
    titleBar_->setStyleSheet(QStringLiteral(
        "QWidget#customTitleBar{background-color:#202020;}"
        "QToolButton{background:transparent; border:none; color:#f5f5f5; padding:6px;}"
        "QToolButton:hover{background-color:rgba(255,255,255,0.12);}" 
        "QLabel#titleLabel{color:#f5f5f5; font-size:14px; font-weight:600;}"));

    auto* barLayout = new QHBoxLayout(titleBar_);
    // 将内边距设为 0，保证标题栏背景可以延伸到窗口最右侧无留白。
    barLayout->setContentsMargins(0, 0, 0, 0);
    barLayout->setSpacing(0);

    // ---- 左侧撤回/前进按钮 ----
    titleLeftArea_ = new QWidget(titleBar_);
    auto* leftLayout = new QHBoxLayout(titleLeftArea_);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(6);

    btnTitleUndo_ = new QToolButton(titleLeftArea_);
    btnTitleUndo_->setToolTip(QStringLiteral("撤回"));
    btnTitleUndo_->setCursor(Qt::PointingHandCursor);
    btnTitleUndo_->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    btnTitleUndo_->setAutoRaise(true);
    leftLayout->addWidget(btnTitleUndo_);

    btnTitleUndo02_ = new QToolButton(titleLeftArea_);
    btnTitleUndo02_->setToolTip(QStringLiteral("前进"));
    btnTitleUndo02_->setCursor(Qt::PointingHandCursor);
    btnTitleUndo02_->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    btnTitleUndo02_->setAutoRaise(true);
    leftLayout->addWidget(btnTitleUndo02_);

    titleLeftArea_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    titleLeftArea_->installEventFilter(this);
    barLayout->addWidget(titleLeftArea_, 0);

    // ---- 中间标题 ----
    titleCenterArea_ = new QWidget(titleBar_);
    auto* centerLayout = new QHBoxLayout(titleCenterArea_);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    titleLabel_ = new QLabel(windowTitle(), titleCenterArea_);
    titleLabel_->setObjectName(QStringLiteral("titleLabel"));
    titleLabel_->setAlignment(Qt::AlignCenter);
    titleLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    centerLayout->addWidget(titleLabel_);
    titleCenterArea_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    titleCenterArea_->installEventFilter(this);
    titleLabel_->installEventFilter(this);
    barLayout->addWidget(titleCenterArea_, 1);

    // ---- 右侧控制按钮 ----
    auto* rightContainer = new QWidget(titleBar_);
    auto* rightLayout = new QHBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    auto makeBtn = [&](QPointer<QToolButton>& btn, QStyle::StandardPixmap icon, const QString& tip) {
        btn = new QToolButton(rightContainer);
        btn->setToolTip(tip);
        btn->setIcon(style()->standardIcon(icon));
        btn->setStyleSheet(
            "QToolButton { color:white; border-radius:4px; }"
            "QToolButton:hover { background-color:#6EAD3E; }");
        rightLayout->addWidget(btn);
        };
    makeBtn(btnMinimize_, QStyle::SP_TitleBarMinButton, QStringLiteral("最小化"));
    makeBtn(btnMaximize_, QStyle::SP_TitleBarMaxButton, QStringLiteral("最大化"));
    makeBtn(btnClose_, QStyle::SP_TitleBarCloseButton, QStringLiteral("关闭"));
    barLayout->addWidget(rightContainer, 0);

    // ---- 安装拖拽事件 ----
    titleBar_->installEventFilter(this);
	topBarLayout->addWidget(titleBar_);

    //标题栏
    ribbontabBar_ = new QTabBar(topBarContainer);
    ribbontabBar_->setObjectName(QStringLiteral("mainRibbonTabBar"));
    ribbontabBar_->setDrawBase(false);
    ribbontabBar_->setExpanding(false);
    ribbontabBar_->setMovable(false);
    ribbontabBar_->setAttribute(Qt::WA_StyledBackground, true); // 让标签栏自绘背景色。
    ribbontabBar_->setStyleSheet(QStringLiteral(
        "QTabBar#mainRibbonTabBar{background-color:#202020; color:#f5f5f5;}"
        "QTabBar#mainRibbonTabBar::tab{padding:8px 16px; margin:0px; border:none; background-color:#202020;}"
        "QTabBar#mainRibbonTabBar::tab:selected{background-color:#333333;}"
        "QTabBar#mainRibbonTabBar::tab:hover{background-color:#2a2a2a;}"));
    //填充标签名称 别的标签页暂时占位置
    const QStringList tabNames = {
           QStringLiteral("文件"),
           QStringLiteral("开始"),
           QStringLiteral("编辑"),
           QStringLiteral("体积"),
           QStringLiteral("选择"),
           QStringLiteral("对齐"),
           QStringLiteral("几何"),
           QStringLiteral("测量"),
           QStringLiteral("CAD/表面测量"),
           QStringLiteral("分析"),
           QStringLiteral("报告"),
           QStringLiteral("动画"),
           QStringLiteral("窗口"),
           QStringLiteral("量具"),
           QStringLiteral("可视化"),
    };
    for (auto& name:tabNames)
    {
        ribbontabBar_->addTab(name);
    }
	ribbontabBar_->setCurrentIndex(0);//把下标设置成0 默认选中第一个标签页
	topBarLayout->addWidget(ribbontabBar_);

	//把整个标题栏放到主窗口
	setMenuWidget(topBarContainer);


    //  连接标题栏按钮 
    connect(btnMinimize_, &QToolButton::clicked, this, &CTViewer::showMinimized);
    connect(btnMaximize_, &QToolButton::clicked, this, [this]() {
        if (isMaximized()) showNormal();
        else showMaximized();
        updateMaximizeButtonIcon();
        });
    connect(btnClose_, &QToolButton::clicked, this, &CTViewer::close);

    //标签栏交互
    connect(ribbontabBar_, &QTabBar::currentChanged, this, [this](int index) {
        //仅先交互DocumentPage
        if (!stack_) {
            return;
        }
        if (index == 0 && pageDocument_) {
			stack_->setCurrentWidget(pageDocument_);//setcurrentwidget是切换当前显示的页面
            statusBar()->showMessage(QStringLiteral("已切换到“文件”功能区"), 1500);
			
        }
        else if (index >= 0) {
            statusBar()->showMessage(QStringLiteral("“%1”功能暂未实现").arg(ribbontabBar_->tabText(index)), 1500);
        }
        
		});

    
    updateMaximizeButtonIcon();
}

void CTViewer::updateMaximizeButtonIcon()
{
    if (!btnMaximize_) return;
    if (isMaximized()) {
        btnMaximize_->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
        btnMaximize_->setToolTip(QStringLiteral("还原"));
    }
    else {
        btnMaximize_->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
        btnMaximize_->setToolTip(QStringLiteral("最大化"));
    }
}


// 事件过滤器（实现标题栏拖动）
bool CTViewer::eventFilter(QObject* watched, QEvent* event)
{
    if (!event) return false;
    bool titleArea = (watched == titleBar_.data()
        || watched == titleLeftArea_.data()
        || watched == titleCenterArea_.data()
        || watched == titleLabel_.data());
    if (titleArea) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            auto* e = static_cast<QMouseEvent*>(event);
            if (e->button() == Qt::LeftButton) {
                draggingWindow_ = true;
                dragOffset_ = e->globalPos() - frameGeometry().topLeft();
                return true;
            }
            break;
        }
        case QEvent::MouseMove: {
            if (draggingWindow_) {
                auto* e = static_cast<QMouseEvent*>(event);
                move(e->globalPos() - dragOffset_);
                return true;
            }
            break;
        }
        case QEvent::MouseButtonRelease:
            draggingWindow_ = false;
            break;
        case QEvent::MouseButtonDblClick:
            draggingWindow_ = false;
            if (isMaximized()) showNormal();
            else showMaximized();
            updateMaximizeButtonIcon();
            return true;
        default: break;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}


// 中央内容区域（只加载 DocumentPage）

void CTViewer::buildCentral()
{
    auto central = new QWidget(this);
    auto v = new QVBoxLayout(central);
    v->setContentsMargins(0, 0, 0, 0);

    stack_ = new QStackedWidget(central);
    v->addWidget(stack_);
    setCentralWidget(central);

    //  加载 DocumentPage 两个页面
    pageDocument_ = new DocumentPage(stack_);
    stack_->addWidget(pageDocument_);
    stack_->setCurrentWidget(pageDocument_);

    // ---- 连接 DocumentPage 发出的信号 ----
    connect(pageDocument_, &DocumentPage::moduleClicked, this, [this](const QString& msg) {
        statusBar()->showMessage(msg, 1500);
        });
    connect(pageDocument_, &DocumentPage::requestSwitchTo, this, [this](const QString& key) {
        statusBar()->showMessage(QStringLiteral("请求切换到页面：%1").arg(key), 1500);
        // 未来：可在此根据 key 切换其他工作台
        });
    connect(pageDocument_, &DocumentPage::recentOpenRequested, this, [this](const QString& name) {
        statusBar()->showMessage(QStringLiteral("正在打开 %1 ...").arg(name), 1500);
        });
}

// 空函数（为兼容老结构保留）
void CTViewer::wireSignals() {}
void CTViewer::setDefaults() {}
