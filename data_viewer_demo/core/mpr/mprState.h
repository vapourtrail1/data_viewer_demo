#pragma once
#include <array>
class vtkImageData;
class vtkResliceCursor;

namespace core::mpr {

    /**
     MprState 管理共享的 vtkResliceCursor 状态。
     */
    class MprState
    {
    public:
        MprState();
        ~MprState();

        void bindImage(vtkImageData* img);
        void resetToCenter();

        int axialIndex() const;
        int coronalIndex() const;
        int sagittalIndex() const;

        void setIndices(int axial, int coronal, int sagittal);

        void center(double out[3]) const;
        void setCenter(const double c[3]);

        vtkResliceCursor* cursor() const;
        vtkImageData* image() const;

    private:
		vtkResliceCursor* m_cursor = nullptr;// vtkResliceCursor的意思是“重切片光标”，用于在三维图像数据中定义和操作切片平面。它允许用户通过交互方式调整切片的位置和方向，从而查看图像数据的不同截面。
        vtkImageData* m_image = nullptr;

    };

} // namespace core::mpr