#pragma once

#include <array>

class vtkImageData;
class vtkResliceCursor;

namespace core::mpr {

/**
 * @brief MprState 管理共享的 vtkResliceCursor 状态。
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
    vtkResliceCursor* m_cursor = nullptr;
    vtkImageData* m_image = nullptr;
};

} // namespace core::mpr
