#include "DocumentPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDialog>

#include "core/services/OrthogonalMprService.h"


DocumentPage::DocumentPage(QWidget* parent)
    : QWidget(parent)
    , mprService_(std::make_unique<core::services::OrthogonalMprService>())
{
    buildUi();
    wireLeftDockSignals();
    wireRightContentSignals();
}

/*
 *  构建整页：左右分栏（左：导航，右：主内容）
 */
void DocumentPage::buildUi()
{
    setObjectName(QStringLiteral("pageDocument"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageDocument{background-color:#404040;}"
        "QLabel{color:#f2f2f2;}"));

    auto hl = new QHBoxLayout(this);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    // 左侧Dock区
    buildLeftDock();
    hl->addWidget(listNav_, 0); // 固定宽度在 buildLeftDock 中设置

    // 右侧主内容
    auto right = buildRightContent(this);
    hl->addWidget(right, 1);
}

/*
 *  构建左侧导航列表
 */
void DocumentPage::buildLeftDock()
{
    listNav_ = new QListWidget(this);
    listNav_->setMinimumWidth(180);
    listNav_->setStyleSheet(R"(
        QListWidget { background:#181818; color:#ddd; border:none; }
        QListWidget::item { height:30px; padding-left:8px; }
        QListWidget::item:selected { background:#444; color:#fff; }
    )");

    // 工具函数:分割线
    auto addSeparator = [this]() {
        auto sep = new QListWidgetItem();
        sep->setFlags(Qt::NoItemFlags);
        sep->setSizeHint(QSize(180, 10));
        listNav_->addItem(sep);

        auto line = new QFrame(listNav_);
        line->setFrameShape(QFrame::HLine);
        line->setStyleSheet("background:#444; margin:4px 12px;");
        listNav_->setItemWidget(sep, line);
        };
    auto addItem = [this](const QString& text) {
        auto item = new QListWidgetItem(text);
        item->setSizeHint(QSize(180, 30));
        listNav_->addItem(item);
        };

    //Dock的内容
    addItem(QStringLiteral("欢迎使用"));
    addSeparator();

    addItem(QStringLiteral("新建"));
    addItem(QStringLiteral("打开"));
    addItem(QStringLiteral("保存"));
    addItem(QStringLiteral("另存为"));
    addItem(QStringLiteral("打包"));
    addItem(QStringLiteral("导出为"));
    addSeparator();

    addItem(QStringLiteral("快速导入"));
    addItem(QStringLiteral("CT重建"));
    addItem(QStringLiteral("导入"));
    addItem(QStringLiteral("导出"));
    addSeparator();

    addItem(QStringLiteral("合并对象"));
    addItem(QStringLiteral("保存对象"));
    addItem(QStringLiteral("保存图像"));
    addItem(QStringLiteral("保存影像/图像堆栈"));
	addItem(QStringLiteral("保存报告"));
	addItem(QStringLiteral("批处理"));
	addItem(QStringLiteral("首选项"));
    addSeparator();

    addItem(QStringLiteral("退出"));
}

/*
 *  构建右侧主内容
 */
QWidget* DocumentPage::buildRightContent(QWidget* parent)
{
    auto right = new QWidget(parent);
    auto vl = new QVBoxLayout(right);
    vl->setContentsMargins(18, 18, 18, 18);
    vl->setSpacing(16);

    // 顶部操作行
   /* auto actionRow = new QFrame(right);
    actionRow->setObjectName(QStringLiteral("welcomeActionRow"));
    actionRow->setStyleSheet(QStringLiteral(
        "QFrame#welcomeActionRow{background:#322F30; border-radius:10px;}"
        "QFrame#welcomeActionRow QPushButton{background:#2C2C2C; border-radius:8px; border:1px solid #333;"
        " color:#f5f5f5; padding:8px 18px;}"
        "QFrame#welcomeActionRow QPushButton:hover{border-color:#4d6fff;}"));
    auto actionLayout = new QHBoxLayout(actionRow);
    actionLayout->setContentsMargins(20, 12, 20, 12);
    actionLayout->setSpacing(12);
    btnUndo_ = new QPushButton(QStringLiteral("撤回"), actionRow);
    btnUndo_->setCursor(Qt::PointingHandCursor);
    btnKeep_ = new QPushButton(QStringLiteral("保持更改"), actionRow);
    btnKeep_->setCursor(Qt::PointingHandCursor);
    actionLayout->addWidget(btnUndo_);
    actionLayout->addWidget(btnKeep_);
    actionLayout->addStretch();
    vl->addWidget(actionRow);*/

    // 顶部横幅
    auto banner = new QFrame(right);
    banner->setObjectName(QStringLiteral("heroBanner"));
    banner->setStyleSheet(QStringLiteral(
        "QFrame#heroBanner{background:#322F30; border-radius:10px;}"
        "QFrame#heroBanner QLabel{color:#f9f9f9;}"));
    auto bannerLayout = new QVBoxLayout(banner);
    bannerLayout->setContentsMargins(20, 16, 20, 16);
    bannerLayout->setSpacing(8);
    auto title = new QLabel(QStringLiteral("欢迎使用 data_viewer_demo"), banner);
    title->setStyleSheet(QStringLiteral("font-size:24px; font-weight:700;"));
    bannerLayout->addWidget(title);
    auto subtitle = new QLabel(QStringLiteral("继续最近项目，或通过下方模块快速开始您的工作流程。"), banner);
    subtitle->setStyleSheet(QStringLiteral("font-size:14px; color:#bbbbbb;"));
    subtitle->setWordWrap(true);
    bannerLayout->addWidget(subtitle);
    vl->addWidget(banner);

    // 操作提示
    auto tipsFrame = new QFrame(right);
    tipsFrame->setObjectName(QStringLiteral("tipsFrame"));
    tipsFrame->setStyleSheet(QStringLiteral(
        "QFrame#tipsFrame{background:#322F30; border-radius:10px;}"
        "QFrame#tipsFrame QLabel{color:#d8d8d8;}"));
    auto tipsLayout = new QVBoxLayout(tipsFrame);
    tipsLayout->setContentsMargins(20, 18, 20, 18);
    tipsLayout->setSpacing(10);
    auto tipsTitle = new QLabel(QStringLiteral("操作提示"), tipsFrame);
    tipsTitle->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    tipsLayout->addWidget(tipsTitle);
    auto tips = new QLabel(QStringLiteral(
        "1.可导入 DICOM、TIFF、RAW 等常见工业 CT 数据。\n"
        "2.若需培训资料，可访问帮助中心以获取最新教程。"), tipsFrame);
    tips->setWordWrap(true);
    tips->setStyleSheet(QStringLiteral("font-size:13px; line-height:20px;"));
    tipsLayout->addWidget(tips);
    tipsLayout->addStretch();
    vl->addWidget(tipsFrame);

    // 模块入口按钮
    auto moduleFrame = new QFrame(right);
    moduleFrame->setObjectName(QStringLiteral("moduleFrame"));
    moduleFrame->setStyleSheet(QStringLiteral(
        "QFrame#moduleFrame{background:#322F30; border-radius:10px;}"
        "QFrame#moduleFrame QPushButton{background:#2C2C2C; border-radius:8px; border:1px solid #333;"
        " color:#f5f5f5; font-size:16px; padding:18px 12px;}"
        "QFrame#moduleFrame QPushButton:hover{background:#2C2C2C; border-color:#4d6fff;}"
        "QFrame#moduleFrame QLabel{color:#f5f5f5;}"));
    auto moduleLayout = new QVBoxLayout(moduleFrame);
    moduleLayout->setContentsMargins(20, 18, 20, 18);
    moduleLayout->setSpacing(12);
    auto moduleTitle = new QLabel(QStringLiteral("选择最适合您工作流程的“开始”选项卡"), moduleFrame);
    moduleTitle->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    moduleLayout->addWidget(moduleTitle);

    auto grid = new QGridLayout();
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(16);
    btnVisCheck_ = new QPushButton(QStringLiteral("视觉检查"), moduleFrame);
    btnPorosity_ = new QPushButton(QStringLiteral("孔隙度"), moduleFrame);
    btnMetrology_ = new QPushButton(QStringLiteral("计量"), moduleFrame);
    btnMaterial_ = new QPushButton(QStringLiteral("材料"), moduleFrame);
    for (auto* b : { btnVisCheck_.data(), btnPorosity_.data(), btnMetrology_.data(), btnMaterial_.data() })
        b->setMinimumSize(160, 70);
    grid->addWidget(btnVisCheck_, 0, 0);
    grid->addWidget(btnPorosity_, 0, 1);
    grid->addWidget(btnMetrology_, 0, 2);
    grid->addWidget(btnMaterial_, 0, 3);
    moduleLayout->addLayout(grid);
    vl->addWidget(moduleFrame);

    // DICOM 打开入口：点击后弹出模态框，不再把工具条固定在欢迎页上。
    auto dicomEntryFrame = new QFrame(right);
    dicomEntryFrame->setObjectName(QStringLiteral("dicomEntryFrame"));
    dicomEntryFrame->setStyleSheet(QStringLiteral(
        "QFrame#dicomEntryFrame{background:#2e2e2e; border-radius:8px;}"
        "QFrame#dicomEntryFrame QPushButton{background:#4d6fff; color:#fff; border:none; border-radius:6px; padding:10px 16px;}"
        "QFrame#dicomEntryFrame QPushButton:hover{background:#758dff;}"
        "QFrame#dicomEntryFrame QLabel{color:#f5f5f5;}"));
    auto dicomEntryLayout = new QHBoxLayout(dicomEntryFrame);
    dicomEntryLayout->setContentsMargins(16, 12, 16, 12);
    dicomEntryLayout->setSpacing(10);
    auto dicomLabel = new QLabel(QStringLiteral("需要加载 DICOM 数据？点击下方按钮后在弹出的模态框中选择文件夹。"), dicomEntryFrame);
    dicomLabel->setWordWrap(true);
    dicomEntryLayout->addWidget(dicomLabel, 1);
    btnDicomEntry_ = new QPushButton(QStringLiteral("打开 DICOM..."), dicomEntryFrame);
    btnDicomEntry_->setCursor(Qt::PointingHandCursor);
    dicomEntryLayout->addWidget(btnDicomEntry_, 0, Qt::AlignRight);
    vl->addWidget(dicomEntryFrame);

    // 最近项目
    auto recentFrame = new QFrame(right);
    recentFrame->setObjectName(QStringLiteral("recentFrame"));
    recentFrame->setStyleSheet(QStringLiteral(
        "QFrame#recentFrame{background:#322F30; border-radius:10px;}"
        "QFrame#recentFrame QLabel{color:#f5f5f5;}"
        "QFrame#recentFrame QHeaderView::section{background:#2c2c2c; color:#f0f0f0; border:0;}"
        "QFrame#recentFrame QTableWidget{background:transparent; border:0; color:#f5f5f5;}"
        "QFrame#recentFrame QTableWidget::item:selected{background-color:#3d65f5;}"));
    auto recentLayout = new QVBoxLayout(recentFrame);
    recentLayout->setContentsMargins(20, 18, 20, 18);
    recentLayout->setSpacing(12);

    auto recentTitle = new QLabel(QStringLiteral("最近项目"), recentFrame);
    recentTitle->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    recentLayout->addWidget(recentTitle);

    tableRecent_ = new QTableWidget(0, 3, recentFrame);
    tableRecent_->setHorizontalHeaderLabels({ QStringLiteral("名称"), QStringLiteral("位置"), QStringLiteral("上次打开") });
    tableRecent_->horizontalHeader()->setStretchLastSection(true);
    tableRecent_->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    tableRecent_->verticalHeader()->setVisible(false);
    tableRecent_->setShowGrid(false);
    tableRecent_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableRecent_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableRecent_->setAlternatingRowColors(true);
    tableRecent_->setStyleSheet(QStringLiteral(
        "QTableWidget{alternate-background-color:#2C2C2C;}"
        "QTableWidget QTableCornerButton::section{background:#2c2c2c;}"));

    struct RecentItem { QString name; QString path; QString time; };
    const QList<RecentItem> recents = {
        { QStringLiteral("发动机缸体.vgl"), QStringLiteral("D:/Projects/CT/EngineBlock"), QStringLiteral("今天 09:24") },
        { QStringLiteral("齿轮箱.vgl"),     QStringLiteral("D:/Projects/CT/GearBox"),     QStringLiteral("昨天 17:42") },
        { QStringLiteral("叶片扫描.vgi"),   QStringLiteral("E:/Scan/Blade"),              QStringLiteral("2024-05-12") },
        { QStringLiteral("材料试样.raw"),   QStringLiteral("E:/Lab/Materials"),           QStringLiteral("2024-04-28") }
    };
    for (const auto& it : recents) {
        int row = tableRecent_->rowCount();
        tableRecent_->insertRow(row);
        tableRecent_->setItem(row, 0, new QTableWidgetItem(it.name));
        tableRecent_->setItem(row, 1, new QTableWidgetItem(it.path));
        tableRecent_->setItem(row, 2, new QTableWidgetItem(it.time));
    }
    tableRecent_->setMinimumHeight(220);
    recentLayout->addWidget(tableRecent_);
    vl->addWidget(recentFrame);

    vl->addStretch();
    return right;
}

/*
 * 连接左侧导航点击逻辑（这里只做演示：发出切页或退出等信号）
 */
void DocumentPage::wireLeftDockSignals()
{
    if (!listNav_) return;

    connect(listNav_, &QListWidget::itemClicked, this,
        [this](QListWidgetItem* it) {
            const QString t = it ? it->text() : QString();

            if (t == QStringLiteral("打开")) {
                // 点击“打开”时弹出 DICOM 选择模态框，行为与性能页保持一致
                emit moduleClicked(QStringLiteral("选择：%1").arg(t));
                showOpenDicomDialog();
            }
            else if (t == QStringLiteral("CT重建")
                || t == QStringLiteral("快速导入")
                || t == QStringLiteral("导入")
                || t == QStringLiteral("导出")) {
                // 这类入口跳去“重建/数据”页面
                emit requestSwitchTo(QStringLiteral("reconstruct")); // 或 "volume"
                emit moduleClicked(QStringLiteral("正在进入：%1").arg(t));
            }
            else if (t == QStringLiteral("退出")) {
                // 这里只能发个意图；真正 close() 由 MainWindow 来做
                emit moduleClicked(QStringLiteral("准备退出"));
            }
            else {
                // 其他项：仅做提示
                emit moduleClicked(QStringLiteral("选择：%1").arg(t));
            }
        });
}

/**
 *  连接右侧按钮/表格逻辑（这里只发信号）
 */
void DocumentPage::wireRightContentSignals()
{
    auto goReconstruct = [this]() { emit requestSwitchTo(QStringLiteral("reconstruct")); };

    connect(btnVisCheck_, &QPushButton::clicked, this, [this, goReconstruct] {
        emit moduleClicked(QStringLiteral("进入视觉检查模块"));
        goReconstruct();
        });
    connect(btnPorosity_, &QPushButton::clicked, this, [this, goReconstruct] {
        emit moduleClicked(QStringLiteral("进入孔隙度分析模块"));
        goReconstruct();
        });
    connect(btnMetrology_, &QPushButton::clicked, this, [this, goReconstruct] {
        emit moduleClicked(QStringLiteral("进入计量模块"));
        goReconstruct();
        });
    connect(btnMaterial_, &QPushButton::clicked, this, [this, goReconstruct] {
        emit moduleClicked(QStringLiteral("进入材料分析模块"));
        goReconstruct();
        });

    // 欢迎页上的入口按钮：打开 DICOM 模态框。
    connect(btnDicomEntry_, &QPushButton::clicked, this, [this] {
        showOpenDicomDialog();
        });

    connect(tableRecent_, &QTableWidget::itemDoubleClicked, this,
        [this, goReconstruct](auto* item) {
            const QString txt = item ? item->text() : QStringLiteral("项目");
            emit recentOpenRequested(txt);
            goReconstruct();
        });

    // 撤回/保持：这里只提示一下
    connect(btnUndo_, &QPushButton::clicked, this, [this] {
        emit moduleClicked(QStringLiteral("已执行撤回操作"));
        });
    connect(btnKeep_, &QPushButton::clicked, this, [this] {
        emit moduleClicked(QStringLiteral("保持当前更改"));
        });

}

// 弹出 DICOM 选择模态框，重用性能页的打开逻辑。
void DocumentPage::showOpenDicomDialog()
{
    // 仅在首次调用时构建 UI，保持简单的复用。
    if (!dicomDialog_) {
        dicomDialog_ = new QDialog(this);
        dicomDialog_->setModal(true);
        dicomDialog_->setWindowTitle(QStringLiteral("打开 DICOM 数据"));

        auto* dialogLayout = new QVBoxLayout(dicomDialog_);
        dialogLayout->setContentsMargins(14, 14, 14, 14);
        dialogLayout->setSpacing(12);

        // 顶部提示文案，提醒用户需要选择目录。
        auto* introLabel = new QLabel(QStringLiteral("请选择包含 DICOM 序列的文件夹，然后点击“加载”。"), dicomDialog_);
        introLabel->setWordWrap(true);
        dialogLayout->addWidget(introLabel);

        // 输入与按钮区域。
        auto* inputRow = new QHBoxLayout();
        inputRow->setSpacing(8);
        auto* dirLabel = new QLabel(QStringLiteral("目录:"), dicomDialog_);
        inputRow->addWidget(dirLabel);

        inputDicomDirectory_ = new QLineEdit(dicomDialog_);
        inputDicomDirectory_->setPlaceholderText(QStringLiteral("选择或输入 DICOM 序列所在目录"));
        inputRow->addWidget(inputDicomDirectory_, 1);

        btnDicomBrowse_ = new QPushButton(QStringLiteral("浏览..."), dicomDialog_);
        inputRow->addWidget(btnDicomBrowse_);

        dialogLayout->addLayout(inputRow);

        // 状态与动作行。
        auto* actionRow = new QHBoxLayout();
        actionRow->setSpacing(8);
        dicomStatusLabel_ = new QLabel(QStringLiteral("尚未加载数据"), dicomDialog_);
        dicomStatusLabel_->setStyleSheet(QStringLiteral("color:#d0d0d0;"));
        actionRow->addWidget(dicomStatusLabel_, 1);

        btnDicomLoad_ = new QPushButton(QStringLiteral("加载"), dicomDialog_);
        btnDicomLoad_->setDefault(true);
        actionRow->addWidget(btnDicomLoad_);
        dialogLayout->addLayout(actionRow);

        // 按钮行为：保持与性能页一致，增加注释说明。
        connect(btnDicomBrowse_, &QPushButton::clicked, this, [this]() {
            const QString directory = QFileDialog::getExistingDirectory(this, QStringLiteral("选择 DICOM 序列所在目录"));
            if (!directory.isEmpty()) {
                inputDicomDirectory_->setText(directory);
            }
            });

        connect(btnDicomLoad_, &QPushButton::clicked, this, [this]() {
            loadDicomDirectory(inputDicomDirectory_->text().trimmed());
            });

        connect(inputDicomDirectory_, &QLineEdit::returnPressed, this, [this]() {
            loadDicomDirectory(inputDicomDirectory_->text().trimmed());
            });
    }

    // 每次展示前重置状态文案，避免旧状态误导用户。
    updateDicomStatusLabel(QStringLiteral("尚未加载数据"), false);
    dicomDialog_->show();
    dicomDialog_->raise();
    dicomDialog_->activateWindow();
}

// 统一更新 DICOM 状态提示，带上错误标记
void DocumentPage::updateDicomStatusLabel(const QString& text, bool isError)
{
    if (!dicomStatusLabel_) {
        return;
    }

    dicomStatusLabel_->setText(text);

    // 根据状态选择颜色：错误为红色，成功为绿色，其余保持中性灰色。
    if (isError) {
        dicomStatusLabel_->setStyleSheet(QStringLiteral("color:#ff6464;"));
    }
    else if (text.contains(QStringLiteral("成功"))) {
        dicomStatusLabel_->setStyleSheet(QStringLiteral("color:#8ae66a;"));
    }
    else {
        dicomStatusLabel_->setStyleSheet(QStringLiteral("color:#d0d0d0;"));
    }
}

// 实际加载 DICOM 目录并反馈状态
void DocumentPage::loadDicomDirectory(const QString& directory)
{
    if (directory.isEmpty()) {
        updateDicomStatusLabel(QStringLiteral("目录为空，请先选择 DICOM 数据。"), true);
        return;
    }

    if (!mprService_) {
        updateDicomStatusLabel(QStringLiteral("加载服务未初始化"), true);
        return;
    }

    QString error;
    if (!mprService_->loadSeries(directory, &error)) {
        updateDicomStatusLabel(error.isEmpty() ? QStringLiteral("加载失败，请检查目录。") : error, true);
        return;
    }

    updateDicomStatusLabel(QStringLiteral("DICOM 数据加载成功"), false);

    // 将成功加载的服务通知给外部，让三视图和 3D 视图立即刷新。
    emit dicomLoaded(mprService_.get());

    // 关闭模态框，避免挡住后续的可视化视图。
    if (dicomDialog_) {
        dicomDialog_->accept();
    }
}
