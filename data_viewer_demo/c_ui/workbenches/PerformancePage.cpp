#include "c_ui/workbenches/PerformancePage.h"
#include "core/services/OrthogonalMprService.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

#if USE_VTK
#include <QVTKOpenGLNativeWidget.h>
#endif

PerformancePage::PerformancePage(QWidget* parent)
    : QWidget(parent)
    , mprService_(std::make_unique<core::services::OrthogonalMprService>())
{
    // ---- UI 构建及信号连接 ----
    buildUi();
    wireSignals();
}

PerformancePage::~PerformancePage() = default;

void PerformancePage::buildUi()
{
    setObjectName(QStringLiteral("performancePage"));
    setStyleSheet(QStringLiteral(
        "QWidget#performancePage{background-color:#1c1c1c;}"
        "QLineEdit{background:#2c2c2c; color:#f0f0f0; border-radius:4px; padding:6px;}"
        "QPushButton{background:#3a3a3a; color:#f0f0f0; border-radius:4px; padding:6px 12px;}"
        "QPushButton:hover{background:#4a4a4a;}"
        "QLabel{color:#f0f0f0;}"));

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(12, 12, 12, 12);
    rootLayout->setSpacing(8);

    // ---- 顶部操作栏：目录输入 + 按钮 ----
    auto* toolbar = new QFrame(this);
    toolbar->setObjectName(QStringLiteral("performanceToolbar"));
    toolbar->setStyleSheet(QStringLiteral(
        "QFrame#performanceToolbar{background-color:#242424; border-radius:6px;}"));

    auto* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(12, 10, 12, 10);
    toolbarLayout->setSpacing(10);

    auto* directoryLabel = new QLabel(QStringLiteral("DICOM目录:"), toolbar);
    toolbarLayout->addWidget(directoryLabel);

    inputDirectory_ = new QLineEdit(toolbar);
    inputDirectory_->setPlaceholderText(QStringLiteral("选择或输入 DICOM 序列所在目录"));
    toolbarLayout->addWidget(inputDirectory_, 1);

    btnBrowse_ = new QPushButton(QStringLiteral("浏览..."), toolbar);
    toolbarLayout->addWidget(btnBrowse_);

    btnLoad_ = new QPushButton(QStringLiteral("加载"), toolbar);
    toolbarLayout->addWidget(btnLoad_);

    statusLabel_ = new QLabel(QStringLiteral("尚未加载数据"), toolbar);
    statusLabel_->setStyleSheet(QStringLiteral("color:#d0d0d0;"));
    toolbarLayout->addWidget(statusLabel_);

    rootLayout->addWidget(toolbar);

    // ---- 四视图区域：2x2 网格 ----
    auto* viewsFrame = new QFrame(this);
    viewsFrame->setObjectName(QStringLiteral("performanceViews"));
    viewsFrame->setStyleSheet(QStringLiteral(
        "QFrame#performanceViews{background-color:#202020; border-radius:8px;}"));

    auto* grid = new QGridLayout(viewsFrame);
    grid->setContentsMargins(10, 10, 10, 10);
    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(8);

#if USE_VTK
    // 使用 QVTKOpenGLNativeWidget 作为真实的渲染容器。
    viewAxial_ = new QVTKOpenGLNativeWidget(viewsFrame);
    viewSagittal_ = new QVTKOpenGLNativeWidget(viewsFrame);
    viewCoronal_ = new QVTKOpenGLNativeWidget(viewsFrame);
    viewVolume_ = new QVTKOpenGLNativeWidget(viewsFrame);
#else
    // 若未启用 VTK，使用占位控件提醒用户。
    auto makePlaceholder = [viewsFrame](const QString& text) {
        auto* holder = new QWidget(viewsFrame);
        holder->setStyleSheet(QStringLiteral("background:#161616; border:1px dashed #444;"));
        auto* layout = new QVBoxLayout(holder);
        layout->setContentsMargins(4, 4, 4, 4);
        auto* label = new QLabel(text, holder);
        label->setAlignment(Qt::AlignCenter);
        layout->addStretch();
        layout->addWidget(label);
        layout->addStretch();
        return holder;
        };
    viewAxial_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示轴状面"));
    viewSagittal_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示矢状面"));
    viewCoronal_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示冠状面"));
    viewVolume_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示体渲染"));
#endif

    grid->addWidget(viewAxial_, 0, 0);
    grid->addWidget(viewSagittal_, 0, 1);
    grid->addWidget(viewCoronal_, 1, 0);
    grid->addWidget(viewVolume_, 1, 1);

    rootLayout->addWidget(viewsFrame, 1);
}

void PerformancePage::wireSignals()
{
    // 浏览按钮：弹出目录选择对话框。
    connect(btnBrowse_, &QPushButton::clicked, this, [this]() {
        const QString directory = QFileDialog::getExistingDirectory(this, QStringLiteral("选择 DICOM 目录"));
        if (!directory.isEmpty()) {
            inputDirectory_->setText(directory);
        }
        });

    // 加载按钮：根据输入框内容尝试载入。
    connect(btnLoad_, &QPushButton::clicked, this, [this]() {
        loadDicomDirectory(inputDirectory_->text().trimmed());
        });

    // 回车触发加载，方便键盘操作。
    connect(inputDirectory_, &QLineEdit::returnPressed, this, [this]() {
        loadDicomDirectory(inputDirectory_->text().trimmed());
        });
}

void PerformancePage::updateStatusLabel(const QString& text, bool isError)
{
    if (!statusLabel_) {
        return;
    }

    statusLabel_->setText(text);
    statusLabel_->setStyleSheet(isError ? QStringLiteral("color:#ff6464;") : QStringLiteral("color:#8ae66a;"));
}

void PerformancePage::loadDicomDirectory(const QString& directory)
{
    if (directory.isEmpty()) {
        updateStatusLabel(QStringLiteral("目录为空，请先选择 DICOM 数据。"), true);
        return;
    }

    if (!mprService_) {
        updateStatusLabel(QStringLiteral("后端服务尚未初始化。"), true);
        return;
    }

    QString error;
    if (!mprService_->loadSeries(directory, &error)) {
        updateStatusLabel(error.isEmpty() ? QStringLiteral("加载失败，请检查目录。") : error, true);
        return;
    }

#if USE_VTK
    initializeVtkViews();
#else
    updateStatusLabel(QStringLiteral("当前构建未启用 VTK，无法渲染视图。"), true);
#endif
}

#if USE_VTK
void PerformancePage::initializeVtkViews()
{
    if (!viewAxial_ || !viewSagittal_ || !viewCoronal_ || !viewVolume_) {
        updateStatusLabel(QStringLiteral("视图控件未正确创建。"), true);
        return;
    }

    if (!mprService_) {
        updateStatusLabel(QStringLiteral("后端服务为空，无法初始化。"), true);
        return;
    }

    const bool success = mprService_->initializeViewers(
        viewAxial_->renderWindow(), viewAxial_->interactor(),
        viewSagittal_->renderWindow(), viewSagittal_->interactor(),
        viewCoronal_->renderWindow(), viewCoronal_->interactor(),
        viewVolume_->renderWindow(), viewVolume_->interactor());

    if (success) {
        updateStatusLabel(QStringLiteral("DICOM 数据加载成功。"), false);
    }
    else {
        updateStatusLabel(QStringLiteral("VTK 视图初始化失败，请检查日志。"), true);
    }
}
#endif