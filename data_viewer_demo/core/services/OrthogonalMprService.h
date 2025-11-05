#pragma once

#include <memory>
#include <QString>
#include "core/common/VtkMacros.h"  

class vtkRenderWindow;
class vtkRenderWindowInteractor;

namespace core::services {
    /*
     * 负责加载 DICOM 系列和连接 VTK 查看器的逻辑的服务接口。
     * 该实现有意隐藏了头文件中的所有 VTK 头文件，以便项目
     * 即使编译时未启用 VTK，也只需包含这一个头文件即可。当 VTK 不可用时，
     * 该服务会报告缺少支持并失败。
     */
    class OrthogonalMprService
    {
    public:
        OrthogonalMprService();
        ~OrthogonalMprService();
        bool loadSeries(const QString& directory, QString* errorMessage = nullptr);
        bool initializeViewers(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,//横断面
        vtkRenderWindow* sagittalWindow,
		vtkRenderWindowInteractor* sagittalInteractor,//矢状面
        vtkRenderWindow* coronalWindow,
		vtkRenderWindowInteractor* coronalInteractor,//冠状面
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor);
        bool hasData() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
} 