#include "core/mpr/mprState.h"
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
        m_cursor->SetCenter(center[0], center[1], center[2]);
    }

    int MprState::axialIndex() const
    {
        if (!m_cursor || !m_image) return 0;
        double c[3]; m_cursor->GetCenter(c);
        double origin[3]; m_image->GetOrigin(origin);
        double spacing[3]; m_image->GetSpacing(spacing);
        return static_cast<int>(std::round((c[2] - origin[2]) / spacing[2]));
    }

    int MprState::coronalIndex() const
    {
        if (!m_cursor || !m_image) return 0;
        double c[3]; m_cursor->GetCenter(c);
        double origin[3]; m_image->GetOrigin(origin);
        double spacing[3]; m_image->GetSpacing(spacing);
        return static_cast<int>(std::round((c[1] - origin[1]) / spacing[1]));
    }

    int MprState::sagittalIndex() const
    {
        if (!m_cursor || !m_image) return 0;
        double c[3]; m_cursor->GetCenter(c);
        double origin[3]; m_image->GetOrigin(origin);
        double spacing[3]; m_image->GetSpacing(spacing);
        return static_cast<int>(std::round((c[0] - origin[0]) / spacing[0]));
    }


    void MprState::setIndices(int axial, int coronal, int sagittal)
    {
        if (!m_cursor || !m_image) return;

        double origin[3];  m_image->GetOrigin(origin);
        double spacing[3]; m_image->GetSpacing(spacing);
        double c[3];       m_cursor->GetCenter(c);

        c[2] = origin[2] + axial * spacing[2]; // axial->Z
        c[1] = origin[1] + coronal * spacing[1]; // coronal->Y
        c[0] = origin[0] + sagittal * spacing[0]; // sagittal->X

        m_cursor->SetCenter(c[0], c[1], c[2]);
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

        m_cursor->SetCenter(c[0], c[1], c[2]);
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