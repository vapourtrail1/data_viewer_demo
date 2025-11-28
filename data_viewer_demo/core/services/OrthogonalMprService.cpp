#include "core/services/OrthogonalMprService.h"
#include <QLoggingCategory>

#if USE_VTK
#  include <vtkAutoInit.h>
#  include <vtkCommand.h>
#  include <vtkImageData.h>
#  include <vtkRenderWindow.h>
#  include <vtkRenderWindowInteractor.h>
#  include <vtkResliceImageViewer.h>
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
#endif

#include "core/mpr/mprAssembly.h"
#include "core/mpr/mprState.h"
#include "core/mpr/mprInteractionRouter.h"
#include "core/render/renderService.h"
#include "core/services/VolumeService.h"
#include "core/data/volumeModel.h"

Q_LOGGING_CATEGORY(lcMprService, "core.mpr.OrthogonalMprService")

/*
   将数据和渲染管线  和  视图窗口 绑定在一起
*/

namespace core::services {

    struct OrthogonalMprService::Impl {
		std::unique_ptr<core::mpr::MprState>           state; //这个参数保存了当前的图像数据和游标状态
		std::unique_ptr<core::mpr::MprAssembly>        assembly;//这个参数负责管理 MPR 视图的 VTK 管线和渲染窗口
		std::unique_ptr<core::mpr::MprInteractionRouter> router;//这个参数负责处理用户交互事件，并将其映射到 MPR 状态的更新
		std::unique_ptr<core::render::RenderService>   render;//这个参数负责应用渲染相关的设置，比如窗口宽度/水平和预设
		std::unique_ptr<VolumeService>                 volume;//这个参数负责加载和管理体数据，比如 DICOM 系列
		bool hasData = false;                                 //标记当前是否有绑定的体数据
		vtkImageData* image = nullptr;                        //当前绑定的体数据
    };

    OrthogonalMprService::OrthogonalMprService()
        : impl_(std::make_unique<Impl>())
    {
        impl_->state = std::make_unique<core::mpr::MprState>();
        impl_->assembly = std::make_unique<core::mpr::MprAssembly>();
        impl_->router = std::make_unique<core::mpr::MprInteractionRouter>();
        impl_->render = std::make_unique<core::render::RenderService>();
        impl_->volume = std::make_unique<VolumeService>();//构造函数new一下
    }

    OrthogonalMprService::~OrthogonalMprService() = default;

	//这个函数绑定四个 QVTKOpenGLNativeWidget 到 MPR 组件中
    void OrthogonalMprService::attachWidgets(QVTKOpenGLNativeWidget* axial,
        QVTKOpenGLNativeWidget* coronal,
        QVTKOpenGLNativeWidget* sagittal,
        QVTKOpenGLNativeWidget* volume3D)
    {
        impl_->assembly->attach(axial, coronal, sagittal, volume3D);
    }

    bool OrthogonalMprService::loadSeries(const QString& directory, QString* error)
    {
#if !USE_VTK
        Q_UNUSED(directory);
        if (error) {
            *error = QStringLiteral("当前未启用 VTK，无法加载 DICOM 目录。");
        }
        return false;
#else
        if (!impl_->volume) {
            if (error) {
                *error = QStringLiteral("内部 VolumeService 未初始化。");
            }
            return false;
        }

        //  用 VolumeService 打开 DICOM 目录
		auto result = impl_->volume->openDicomDir(directory);
        if (!result.ok() || !result.value) {
            if (error) {
                if (!result.message.isEmpty()) {
					*error = result.message;
                }
                else {
                    *error = QStringLiteral("加载 DICOM 目录失败。");
                }
            }
            impl_->hasData = false;
            return false;
        }

        //  拿到 VolumeModel 里的 vtkImageData
        const core::data::VolumeModel* volumeModel = result.value;
        vtkImageData* image = volumeModel ? volumeModel->image() : nullptr;
        if (!image) {
            if (error) {
                *error = QStringLiteral("加载成功，但 VolumeModel 中没有图像数据。");
            }
            impl_->hasData = false;
            return false;
        }
        //  绑定到当前 MPR 状态
		impl_->state->bindImage(image);//把参数 image 传给 MprState 对象
        impl_->state->resetToCenter();
        impl_->hasData = true;
        if (error) {
            error->clear();  // 告诉上层 没有错误
        }
        return true;
#endif
    }

	//这个函数绑定四个 VTK 窗口和交互器 到 MPR 组件中
	//service是传入的参数，service里保存的是已经加载好的vtkImageData和MPR管线对象
    bool OrthogonalMprService::initializeViewers(
        vtkRenderWindow* axialWindow, vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* sagittalWindow, vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* coronalWindow, vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* volumeWindow, vtkRenderWindowInteractor* volumeInteractor)
    {
#if !USE_VTK
        Q_UNUSED(axialWindow); Q_UNUSED(axialInteractor);
        Q_UNUSED(sagittalWindow); Q_UNUSED(sagittalInteractor);
        Q_UNUSED(coronalWindow); Q_UNUSED(coronalInteractor);
        Q_UNUSED(volumeWindow); Q_UNUSED(volumeInteractor);
        return false;
#else
        if (!impl_->hasData) {
            return false;
        }
        
        // 先断开旧的交互路由
        impl_->router->unwire();

        // 绑定原始 VTK window/interactor
        impl_->assembly->attachRaw(
            axialWindow, axialInteractor,
            coronalWindow, coronalInteractor,
            sagittalWindow, sagittalInteractor,
            volumeWindow, volumeInteractor);

        impl_->assembly->setState(impl_->state.get());
        impl_->assembly->buildPipelines();
        impl_->assembly->build3DPlanes();

        //样式
        if (impl_->render) {
            impl_->render->syncWLTo2D(
            impl_->assembly->axialViewer(),
            impl_->assembly->coronalViewer(),
            impl_->assembly->sagittalViewer());
			impl_->render->applyPreset(QStringLiteral("SoftTissue"),
            impl_->assembly->axialViewer(),
            impl_->assembly->coronalViewer(),
            impl_->assembly->sagittalViewer(),
        	impl_->assembly->volumeProperty());
			impl_->assembly->refreshAll();
        }

        // 构建3D三平面
        if (impl_->router)
        {
           impl_->router->wire();
        }
        return true;
#endif
    }

    void OrthogonalMprService::detach()
    {
        impl_->router->unwire();
        impl_->assembly->detach();
        impl_->hasData = false;
        impl_->image = nullptr;
    }

    //绑定一份体数据

	bool OrthogonalMprService::bindImage(vtkImageData* img)
    {
        impl_->image = img;
        impl_->hasData = (img != nullptr);
        if (!img) {
            impl_->state->bindImage(nullptr);
            return false;
        }
        impl_->state->bindImage(img);
        impl_->state->resetToCenter();
        return true;
    }


    bool OrthogonalMprService::hasData() const
    {
        return impl_->hasData;
    }

	// 重置游标到体数据中心
    void OrthogonalMprService::resetCursorToCenter()
    {
        if (!impl_->hasData) return;
        impl_->state->resetToCenter();
        impl_->assembly->refreshAll();
    }
    
	//这个函数设置三向切片的索引位置
    void OrthogonalMprService::setSliceIndex(int axial, int coronal, int sagittal)
    {
#if USE_VTK
        if (!impl_->hasData) return;
        impl_->state->setIndices(axial, coronal, sagittal);
        impl_->assembly->on2DSliceChanged(axial, coronal, sagittal);
        impl_->assembly->refreshAll();
#else
        Q_UNUSED(axial); Q_UNUSED(coronal); Q_UNUSED(sagittal);
#endif
    }

	//这个函数设置窗宽窗位
    void OrthogonalMprService::setWindowLevel(double window, double level, bool allViews)
    {
#if USE_VTK
        if (!impl_->hasData) {
            return;
        }

        Q_UNUSED(allViews);

        impl_->render->setWL(window, level);
        impl_->render->syncWLTo2D(
        impl_->assembly->axialViewer(),
        impl_->assembly->coronalViewer(),
        impl_->assembly->sagittalViewer());
        impl_->assembly->refreshAll();
#else
        Q_UNUSED(window); Q_UNUSED(level); Q_UNUSED(allViews);
#endif
    }
    
	//应用预设
    void OrthogonalMprService::applyPreset(const QString& name)
    {
#if USE_VTK
        if (!impl_->hasData) return;
        impl_->render->applyPreset(name,
        impl_->assembly->axialViewer(),
        impl_->assembly->coronalViewer(),
        impl_->assembly->sagittalViewer(),
        impl_->assembly->volumeProperty());
        impl_->assembly->refreshAll();
#else
        Q_UNUSED(name);
#endif
    }
} // namespace core::services
