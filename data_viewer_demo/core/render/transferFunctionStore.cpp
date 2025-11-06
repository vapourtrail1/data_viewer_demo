#include "core/render/transferFunctionStroe.h"
#include <vtkColorTransferFunction.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>

namespace core::render {

    namespace {
        const core::common::WL kDefaultWL{ 400.0, 40.0 };
        const QHash<QString, core::common::WL> kPresetWLs = {
            { QStringLiteral("SoftTissue"), { 400.0, 40.0 } },
            { QStringLiteral("Bone"), { 2000.0, 500.0 } },
            { QStringLiteral("Lung"), { 1500.0, -600.0 } },
        };
    }

    TransferFunctionStore::TransferFunctionStore() = default;

    core::common::WL TransferFunctionStore::presetWL(const QString& name) const
    {
        return kPresetWLs.value(name, fallback());
    }

    void TransferFunctionStore::fillVolumePreset(const QString& name, vtkVolumeProperty* prop) const
    {
        if (!prop) {
            return;
        }

        vtkNew<vtkColorTransferFunction> color;
        vtkNew<vtkPiecewiseFunction> opacity;

        const QString preset = name.isEmpty() ? QStringLiteral("SoftTissue") : name;

        if (preset == QStringLiteral("Bone")) {
            color->AddRGBPoint(-1000.0, 0.0, 0.0, 0.0);
            color->AddRGBPoint(0.0, 0.9, 0.9, 0.9);
            color->AddRGBPoint(2000.0, 1.0, 1.0, 1.0);

            opacity->AddPoint(-1000.0, 0.0);
            opacity->AddPoint(300.0, 0.1);
            opacity->AddPoint(1500.0, 0.9);
        }
        else if (preset == QStringLiteral("Lung")) {
            color->AddRGBPoint(-1000.0, 0.3, 0.7, 0.9);
            color->AddRGBPoint(-600.0, 0.8, 0.9, 0.9);
            color->AddRGBPoint(400.0, 1.0, 1.0, 1.0);

            opacity->AddPoint(-1000.0, 0.0);
            opacity->AddPoint(-700.0, 0.0);
            opacity->AddPoint(-500.0, 0.2);
            opacity->AddPoint(400.0, 0.6);
        }
        else {
            color->AddRGBPoint(-200.0, 0.0, 0.0, 0.0);
            color->AddRGBPoint(100.0, 0.8, 0.3, 0.3);
            color->AddRGBPoint(300.0, 0.9, 0.9, 0.9);

            opacity->AddPoint(-200.0, 0.0);
            opacity->AddPoint(40.0, 0.05);
            opacity->AddPoint(300.0, 0.6);
        }

        prop->SetColor(color);
        prop->SetScalarOpacity(opacity);
        prop->SetInterpolationTypeToLinear();
        prop->ShadeOn();
    }

    core::common::WL TransferFunctionStore::fallback() const
    {
        return kDefaultWL;
    }

} // namespace core::render