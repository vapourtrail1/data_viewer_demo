#pragma once
#pragma once

#include <QString>

#include "core/common/VtkMacros.h" // 统一入口：根据环境自动判定是否启用 VTK。

class vtkRenderWindow;
class vtkRenderWindowInteractor;

namespace core::services {

    /**
     * @brief 后端服务：负责加载 DICOM 数据并为前端提供三向正交视图和原始体渲染。
     */
    class OrthogonalMprService
    {
    public:
        OrthogonalMprService();
        ~OrthogonalMprService();

        /**
         * @brief 读取指定目录下的 DICOM 序列。
         * @param directory DICOM 序列所在目录。
         * @param errorMessage 可选的错误信息返回字符串。
         * @return 读取是否成功。
         */
        bool loadSeries(const QString& directory, QString* errorMessage = nullptr);

#if USE_VTK
        /**
         * @brief 将内部的 VTK 管线绑定到四个渲染窗口上。
         * @param axialWindow 轴状面渲染窗口。
         * @param axialInteractor 轴状面交互器。
         * @param sagittalWindow 矢状面渲染窗口。
         * @param sagittalInteractor 矢状面交互器。
         * @param coronalWindow 冠状面渲染窗口。
         * @param coronalInteractor 冠状面交互器。
         * @param volumeWindow 原始体数据渲染窗口。
         * @param volumeInteractor 原始体数据交互器。
         * @return 管线初始化是否成功。
         */
        bool initializeViewers(vtkRenderWindow* axialWindow,
            vtkRenderWindowInteractor* axialInteractor,
            vtkRenderWindow* sagittalWindow,
            vtkRenderWindowInteractor* sagittalInteractor,
            vtkRenderWindow* coronalWindow,
            vtkRenderWindowInteractor* coronalInteractor,
            vtkRenderWindow* volumeWindow,
            vtkRenderWindowInteractor* volumeInteractor);
#else
        bool initializeViewers(vtkRenderWindow* axialWindow,
            vtkRenderWindowInteractor* axialInteractor,
            vtkRenderWindow* sagittalWindow,
            vtkRenderWindowInteractor* sagittalInteractor,
            vtkRenderWindow* coronalWindow,
            vtkRenderWindowInteractor* coronalInteractor,
            vtkRenderWindow* volumeWindow,
            vtkRenderWindowInteractor* volumeInteractor)
        {
            Q_UNUSED(axialWindow);
            Q_UNUSED(axialInteractor);
            Q_UNUSED(sagittalWindow);
            Q_UNUSED(sagittalInteractor);
            Q_UNUSED(coronalWindow);
            Q_UNUSED(coronalInteractor);
            Q_UNUSED(volumeWindow);
            Q_UNUSED(volumeInteractor);
            return false;
        }
#endif

        /**
         * @brief 判断是否已经成功加载了 DICOM 数据。
         */
        bool hasData() const;

    private:
#if USE_VTK
        struct Impl;
        Impl* impl_;
#else
        void* impl_ = nullptr;
#endif
    };

} // namespace core::services