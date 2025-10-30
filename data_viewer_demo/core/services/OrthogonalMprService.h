#pragma once

#include <memory>

#include <QString>

#include "core/common/VtkMacros.h"  // Centralised macro that reveals whether VTK support is available.

class vtkRenderWindow;
class vtkRenderWindowInteractor;

namespace core::services {
    /**
     * @brief Service facade that owns the logic for loading a DICOM series and wiring VTK viewers.
     *
     * The implementation intentionally hides all VTK headers from the header file so that projects
     * that compile without VTK still only need to include this single header.  When VTK is not
     * available the service reports the lack of support and fails gracefully.
     */
    class OrthogonalMprService
    {
    public:
        OrthogonalMprService();
        ~OrthogonalMprService();
        bool loadSeries(const QString& directory, QString* errorMessage = nullptr);
        bool initializeViewers(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* sagittalWindow,
        vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* coronalWindow,
        vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor);
        bool hasData() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // na