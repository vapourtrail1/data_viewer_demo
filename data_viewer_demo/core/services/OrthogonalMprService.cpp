#include "core/services/OrthogonalMprService.h"
#include <array>
#include <utility>
#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QtGlobal>

#if USE_VTK
#    include <vtkCommand.h>
#    include <vtkColorTransferFunction.h>
#    include <vtkDICOMImageReader.h>
#    include <vtkImageData.h>
#    include <vtkPiecewiseFunction.h>
#    include <vtkRenderer.h>
#    include <vtkRenderWindow.h>
#    include <vtkRenderWindowInteractor.h>
#    include <vtkResliceCursor.h>
#    include <vtkResliceCursorLineRepresentation.h>
#    include <vtkResliceCursorWidget.h>
#    include <vtkResliceImageViewer.h>
#    include <vtkSmartPointer.h>
#    include <vtkSmartVolumeMapper.h>
#    include <vtkVolume.h>
#    include <vtkVolumeProperty.h>
#endif

Q_LOGGING_CATEGORY(lcOrthogonalMpr, "core.services.OrthogonalMprService");

namespace core::services {

#if USE_VTK
    namespace {

        /**
         * @brief Propagates slice-interaction events between viewers and keeps the volume view updated.
         */
        class ResliceCursorCallback final : public vtkCommand
        {
        public:
            static ResliceCursorCallback* New()
            {
                return new ResliceCursorCallback();
            }

            vtkTypeMacro(ResliceCursorCallback, vtkCommand);

            void SetViewers(const std::array<vtkResliceImageViewer*, 3>& viewers)
            {
                viewers_ = viewers;
            }

            void SetVolumeWindow(vtkRenderWindow* window)
            {
                volumeWindow_ = window;
            }

            void Execute(vtkObject* caller, unsigned long eventId, void*) override
            {
                Q_UNUSED(caller);
                Q_UNUSED(eventId);

                for (auto* viewer : viewers_) {
                    if (viewer) {
                        viewer->Render();
                    }
                }

                if (volumeWindow_) {
                    volumeWindow_->Render();
                }
            }

        private:
            std::array<vtkResliceImageViewer*, 3> viewers_{};
            vtkRenderWindow* volumeWindow_ = nullptr;
        };

    } // namespace
#endif // USE_VTK

    struct OrthogonalMprService::Impl
    {
#if USE_VTK
        vtkSmartPointer<vtkDICOMImageReader> reader;
        vtkSmartPointer<vtkResliceCursor> resliceCursor;
        std::array<vtkSmartPointer<vtkResliceImageViewer>, 3> viewers;
        std::array<vtkSmartPointer<vtkResliceCursorWidget>, 3> widgets;
        vtkSmartPointer<ResliceCursorCallback> callback;

        vtkSmartPointer<vtkRenderer> volumeRenderer;
        vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper;
        vtkSmartPointer<vtkVolume> volume;
        vtkSmartPointer<vtkVolumeProperty> volumeProperty;
        vtkSmartPointer<vtkColorTransferFunction> volumeColor;
        vtkSmartPointer<vtkPiecewiseFunction> volumeOpacity;

        vtkRenderWindow* linkedVolumeWindow = nullptr;
        bool hasData = false;
#else
        bool hasData = false;
#endif
    };

    OrthogonalMprService::OrthogonalMprService()
        : impl_(std::make_unique<Impl>())
    {
#if !USE_VTK
        qCWarning(lcOrthogonalMpr)
            << "VTK support is disabled; OrthogonalMprService will only provide stub behaviour.";
#endif
    }

    OrthogonalMprService::~OrthogonalMprService() = default;

    bool OrthogonalMprService::loadSeries(const QString& directory, QString* errorMessage)
    {
        if (!impl_) {
            // The unique_ptr should always be initialised, but guard against accidental misuse.
            impl_ = std::make_unique<Impl>();
        }

#if USE_VTK
        if (directory.isEmpty()) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("Please choose a valid DICOM directory before loading.");
            }
            impl_->hasData = false;
            return false;
        }

        const QFileInfo dirInfo(directory);
        if (!dirInfo.exists() || !dirInfo.isDir()) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("The specified DICOM directory does not exist: %1").arg(directory);
            }
            impl_->hasData = false;
            return false;
        }

        impl_->reader = vtkSmartPointer<vtkDICOMImageReader>::New();
        impl_->reader->SetDirectoryName(QDir::toNativeSeparators(dirInfo.absoluteFilePath()).toUtf8().constData());
        impl_->reader->Update();

        vtkImageData* image = impl_->reader->GetOutput();
        if (image == nullptr || image->GetNumberOfPoints() == 0) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("No readable DICOM volume was produced from the directory: %1").arg(directory);
            }
            impl_->hasData = false;
            impl_->reader = nullptr;
            return false;
        }

        impl_->hasData = true;
        return true;
#else
        Q_UNUSED(directory);
        if (errorMessage) {
            *errorMessage = QStringLiteral("VTK support is disabled in this build; loading is not available.");
        }
        impl_->hasData = false;
        return false;
#endif
    }

    bool OrthogonalMprService::initializeViewers(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* sagittalWindow,
        vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* coronalWindow,
        vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor)
    {
#if USE_VTK
        if (!impl_ || !impl_->hasData) {
            qCWarning(lcOrthogonalMpr)
                << "initializeViewers called before loadSeries produced any data.";
            return false;
        }

        if (!axialWindow || !sagittalWindow || !coronalWindow || !volumeWindow) {
            qCWarning(lcOrthogonalMpr)
                << "All render windows must be valid before initialising the MPR pipeline.";
            return false;
        }

        if (!axialInteractor || !sagittalInteractor || !coronalInteractor || !volumeInteractor) {
            qCWarning(lcOrthogonalMpr)
                << "All render window interactors must be valid before initialising the MPR pipeline.";
            return false;
        }

        vtkImageData* image = impl_->reader ? impl_->reader->GetOutput() : nullptr;
        if (!image) {
            qCWarning(lcOrthogonalMpr)
                << "The reader did not yield an image; aborting viewer initialisation.";
            return false;
        }

        // 统一的 ResliceCursor（让三个切面联动）
        impl_->resliceCursor = vtkSmartPointer<vtkResliceCursor>::New();
        impl_->resliceCursor->SetImage(image);
        impl_->resliceCursor->SetThickMode(0);

        double range[2] = { 0.0, 0.0 };
        image->GetScalarRange(range);

        const std::array<std::pair<vtkRenderWindow*, vtkRenderWindowInteractor*>, 3> viewConfig = {
            std::make_pair(axialWindow, axialInteractor),
            std::make_pair(sagittalWindow, sagittalInteractor),
            std::make_pair(coronalWindow, coronalInteractor)
        };

        for (int i = 0; i < 3; ++i) {
            // 1) 创建并配置每个切片 Viewer
            impl_->viewers[i] = vtkSmartPointer<vtkResliceImageViewer>::New();
            impl_->viewers[i]->SetInputData(image);
            impl_->viewers[i]->SetResliceCursor(impl_->resliceCursor);  // 统一的 ResliceCursor 设在 viewer 上
            impl_->viewers[i]->SetSliceOrientation(i);                  // 0=axial, 1=sagittal, 2=coronal
            impl_->viewers[i]->SetRenderWindow(viewConfig[i].first);
            impl_->viewers[i]->SetupInteractor(viewConfig[i].second);

            impl_->viewers[i]->SetColorWindow(range[1] - range[0]);
            impl_->viewers[i]->SetColorLevel(0.5 * (range[0] + range[1]));
            impl_->viewers[i]->GetRenderer()->ResetCamera();

            // 2) 直接使用 viewer 自带的 Widget；只做交互器/渲染器绑定与启用
            impl_->widgets[i] = impl_->viewers[i]->GetResliceCursorWidget();
            impl_->widgets[i]->SetInteractor(viewConfig[i].second);
            impl_->widgets[i]->SetDefaultRenderer(impl_->viewers[i]->GetRenderer());
            impl_->widgets[i]->SetEnabled(1);

            // （可选）如果需要调外观，可从 widget 取 representation 再 SafeDownCast 做美化
            // auto* rep = vtkResliceCursorLineRepresentation::SafeDownCast(impl_->widgets[i]->GetRepresentation());
            // if (rep) {
            //     rep->GetResliceCursorActor()->GetCenterlineProperty(0)->SetLineWidth(1.5);
            //     rep->GetResliceCursorActor()->GetCenterlineProperty(1)->SetLineWidth(1.5);
            //     rep->GetResliceCursorActor()->GetCenterlineProperty(2)->SetLineWidth(1.5);
            // }
        }


        // 事件回调：三视图联动，并刷新体渲染窗口
        impl_->callback = vtkSmartPointer<ResliceCursorCallback>::New();
        impl_->callback->SetViewers({
            impl_->viewers[0].GetPointer(),
            impl_->viewers[1].GetPointer(),
            impl_->viewers[2].GetPointer()
            });
        impl_->callback->SetVolumeWindow(volumeWindow);

        for (auto& widget : impl_->widgets) {
            widget->AddObserver(vtkCommand::InteractionEvent, impl_->callback);
            widget->AddObserver(vtkResliceCursorWidget::ResliceAxesChangedEvent, impl_->callback);
        }

        // 简单的体渲染管线（与窗宽窗位联动刷新即可）
        impl_->linkedVolumeWindow = volumeWindow;
        impl_->volumeRenderer = vtkSmartPointer<vtkRenderer>::New();
        impl_->volumeRenderer->SetBackground(0.1, 0.1, 0.1);
        volumeWindow->AddRenderer(impl_->volumeRenderer);
        volumeInteractor->SetRenderWindow(volumeWindow);

        impl_->volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
        impl_->volumeMapper->SetInputData(image);

        impl_->volumeColor = vtkSmartPointer<vtkColorTransferFunction>::New();
        impl_->volumeColor->AddRGBPoint(range[0], 0.0, 0.0, 0.0);
        impl_->volumeColor->AddRGBPoint(range[1], 1.0, 1.0, 1.0);

        impl_->volumeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
        impl_->volumeOpacity->AddPoint(range[0], 0.0);
        impl_->volumeOpacity->AddPoint(range[1], 1.0);

        impl_->volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
        impl_->volumeProperty->SetColor(impl_->volumeColor);
        impl_->volumeProperty->SetScalarOpacity(impl_->volumeOpacity);
        impl_->volumeProperty->SetInterpolationTypeToLinear();
        impl_->volumeProperty->ShadeOff();

        impl_->volume = vtkSmartPointer<vtkVolume>::New();
        impl_->volume->SetMapper(impl_->volumeMapper);
        impl_->volume->SetProperty(impl_->volumeProperty);

        impl_->volumeRenderer->AddVolume(impl_->volume);
        impl_->volumeRenderer->ResetCamera();

        // 初始渲染
        axialWindow->Render();
        sagittalWindow->Render();
        coronalWindow->Render();
        volumeWindow->Render();

        return true;
#else
        Q_UNUSED(axialWindow);
        Q_UNUSED(axialInteractor);
        Q_UNUSED(sagittalWindow);
        Q_UNUSED(sagittalInteractor);
        Q_UNUSED(coronalWindow);
        Q_UNUSED(coronalInteractor);
        Q_UNUSED(volumeWindow);
        Q_UNUSED(volumeInteractor);

        qCWarning(lcOrthogonalMpr)
            << "initializeViewers is unavailable because VTK support is disabled.";
        return false;
#endif
    }
    bool OrthogonalMprService::hasData() const
    {
        return impl_ ? impl_->hasData : false;
    }

}