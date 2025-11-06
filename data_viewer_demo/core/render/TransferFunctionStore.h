#pragma once

#include <QHash>
#include <QString>

#include "core/common/Types.h"

class vtkVolumeProperty;

namespace core::render {

/**
 * @brief TransferFunctionStore 保存体渲染的颜色/不透明度预设。
 */
class TransferFunctionStore
{
public:
    TransferFunctionStore();

    core::common::WL presetWL(const QString& name) const;
    void fillVolumePreset(const QString& name, vtkVolumeProperty* prop) const;

private:
    core::common::WL fallback() const;
};

} // namespace core::render
