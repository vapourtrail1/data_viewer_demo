#include "core/mpr/MprState.h"

#include <vtkImageData.h>
#include <vtkResliceCursor.h>

namespace core::mpr {

MprState::MprState()
{
    // vtkResliceCursor 负责记录三个正交平面的切片信息。
    m_cursor = vtkResliceCursor::New();
}

MprState::~MprState()
{
    if (m_cursor) {
        m_cursor->Delete();
        m_cursor = nullptr;
    }
}

void MprState::bindImage(vtkImageData* img)
{
    m_image = img;
    if (m_cursor) {
        m_cursor->SetImage(img);
    }
}

void MprState::resetToCenter()
{
    if (!m_cursor || !m_image) {
        return;
    }

    // 根据体数据的中心点重置 reslice 光标。
    double bounds[6];
    m_image->GetBounds(bounds);
    const double center[3] = {
        0.5 * (bounds[0] + bounds[1]),
        0.5 * (bounds[2] + bounds[3]),
        0.5 * (bounds[4] + bounds[5]),
    };
    m_cursor->SetCenter(center);
}

int MprState::axialIndex() const
{
    return m_cursor ? m_cursor->GetSliceIndex(2) : 0;
}

int MprState::coronalIndex() const
{
    return m_cursor ? m_cursor->GetSliceIndex(1) : 0;
}

int MprState::sagittalIndex() const
{
    return m_cursor ? m_cursor->GetSliceIndex(0) : 0;
}

void MprState::setIndices(int axial, int coronal, int sagittal)
{
    if (!m_cursor) {
        return;
    }

    m_cursor->SetSliceIndex(2, axial);
    m_cursor->SetSliceIndex(1, coronal);
    m_cursor->SetSliceIndex(0, sagittal);
}

void MprState::center(double out[3]) const
{
    if (!m_cursor || !out) {
        return;
    }

    m_cursor->GetCenter(out);
}

void MprState::setCenter(const double c[3])
{
    if (!m_cursor || !c) {
        return;
    }

    m_cursor->SetCenter(c);
}

vtkResliceCursor* MprState::cursor() const
{
    return m_cursor;
}

vtkImageData* MprState::image() const
{
    return m_image;
}

} // namespace core::mpr
