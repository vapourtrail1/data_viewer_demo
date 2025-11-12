#pragma once
#include <array>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

class vtkImageData;

namespace core::data {

    /*
     VolumeModel 保存体数据的 vtkImageData 句柄与元信息。
     */
    class VolumeModel//定义一个类
    {
    public:
        VolumeModel();

        vtkImageData* image() const;
        void setImage(vtkImageData* image);

        std::array<double, 3> spacing3() const;
        std::array<double, 3> origin3() const;
        std::array<int, 6> extent6() const;
        std::array<double, 2> scalarRange2() const;
        std::array<double, 9> directionMatrix() const;

        void setSpacing(const std::array<double, 3>& spacing);
        void setOrigin(const std::array<double, 3>& origin);
        void setExtent(const std::array<int, 6>& extent);
        void setScalarRange(const std::array<double, 2>& range);
        void setDirection(const std::array<double, 9>& direction);

    private:
		vtkSmartPointer<vtkImageData> m_image;//智能指针管理vtkImageData对象的生命周期
        std::array<double, 3> m_spacing{};
        std::array<double, 3> m_origin{};
        std::array<int, 6> m_extent{};
        std::array<double, 2> m_scalarRange{};
        std::array<double, 9> m_direction{};
    };
} // namespace core::data