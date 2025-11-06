#include "core/data/volumeModel.h"
#include <vtkImageData.h>

namespace core::data {

    VolumeModel::VolumeModel() = default;

    vtkImageData* VolumeModel::image() const
    {
        return m_image;
    }

    void VolumeModel::setImage(vtkImageData* image)
    {
        // vtkSmartPointer 自动管理引用计数，不复制数据。
        m_image = image;
    }

    std::array<double, 3> VolumeModel::spacing3() const
    {
        return m_spacing;
    }

    std::array<double, 3> VolumeModel::origin3() const
    {
        return m_origin;
    }

    std::array<int, 6> VolumeModel::extent6() const
    {
        return m_extent;
    }

    std::array<double, 2> VolumeModel::scalarRange2() const
    {
        return m_scalarRange;
    }

    std::array<double, 9> VolumeModel::directionMatrix() const
    {
        return m_direction;
    }

    void VolumeModel::setSpacing(const std::array<double, 3>& spacing)
    {
        m_spacing = spacing;
    }

    void VolumeModel::setOrigin(const std::array<double, 3>& origin)
    {
        m_origin = origin;
    }

    void VolumeModel::setExtent(const std::array<int, 6>& extent)
    {
        m_extent = extent;
    }

    void VolumeModel::setScalarRange(const std::array<double, 2>& range)
    {
        m_scalarRange = range;
    }

    void VolumeModel::setDirection(const std::array<double, 9>& direction)
    {
        m_direction = direction;
    }

} // namespace core::data