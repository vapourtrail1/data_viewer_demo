#include "core/io/volumeIOServiceVtk.h"
#include <algorithm>
#include <QFile>
#include <QFileInfo>
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkImageReader.h>
#include <vtkMatrix3x3.h>
#include <vtkNIFTIImageReader.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include "core/common/error.h"
#include "core/common/logging.h"


namespace core::io {

    VolumeIOServiceVtk::VolumeIOServiceVtk() = default;

    VolumeIOServiceVtk::~VolumeIOServiceVtk() = default;

    core::common::Result<core::data::VolumeModel> VolumeIOServiceVtk::loadDicomDir(const QString& dir)
    {
        // 使用 VTK 的 DICOM 读取器直接解析目录。
        core::common::Result<core::data::VolumeModel> result;

        vtkNew<vtkDICOMImageReader> reader;
        reader->SetDirectoryName(QFile::encodeName(dir));

        try {
			reader->Update();//尝试执行这段代码 ，如果失败 ，抛出异常
        }
        catch (...) {
            result.code = core::common::ErrorCode::IoFailed;
            result.message = QStringLiteral("读取 DICOM 目录失败: %1").arg(dir);
            return result;
        }

        result = buildModelFromImage(reader->GetOutput(), dir);
        return result;
    }

    core::common::Result<core::data::VolumeModel> VolumeIOServiceVtk::loadNifti(const QString& file)
    {
        // NIfTI 直接使用 vtkNIFTIImageReader。
        core::common::Result<core::data::VolumeModel> result;

        vtkNew<vtkNIFTIImageReader> reader;
        reader->SetFileName(QFile::encodeName(file));

        try {
            reader->Update();
        }
        catch (...) {
            result.code = core::common::ErrorCode::IoFailed;
            result.message = QStringLiteral("读取 NIfTI 文件失败: %1").arg(file);
            return result;
        }

        result = buildModelFromImage(reader->GetOutput(), file);
        return result;
    }

    core::common::Result<core::data::VolumeModel> VolumeIOServiceVtk::loadRaw(const RawParam& param, const QString& file)
    {
        // RAW 读取需要调用者提供尺寸和像素信息。
        core::common::Result<core::data::VolumeModel> result;

        if (param.dimX <= 0 || param.dimY <= 0 || param.dimZ <= 0) {
            result.code = core::common::ErrorCode::RawParamMissing;
            result.message = QStringLiteral("RAW 参数不完整");
            return result;
        }

        vtkNew<vtkImageReader> reader;
        reader->SetFileDimensionality(3);
        reader->SetFileName(QFile::encodeName(file));
        reader->SetDataExtent(0, param.dimX - 1, 0, param.dimY - 1, 0, param.dimZ - 1);
        reader->SetDataSpacing(1.0, 1.0, 1.0);
        reader->SetDataOrigin(0.0, 0.0, 0.0);

        switch (param.bytesPerPixel) {
        case 1:
            if (param.isSigned) {
                reader->SetDataScalarTypeToChar();
            }
            else {
                reader->SetDataScalarTypeToUnsignedChar();
            }
            break;
        case 2:
            if (param.isSigned) {
                reader->SetDataScalarTypeToShort();
            }
            else {
                reader->SetDataScalarTypeToUnsignedShort();
            }
            break;
        case 4:
            reader->SetDataScalarTypeToFloat();
            break;
        default:
            result.code = core::common::ErrorCode::PixelUnsupported;
            result.message = QStringLiteral("暂不支持的 RAW 像素字节数: %1").arg(param.bytesPerPixel);
            return result;
        }

        if (param.littleEndian) {
            reader->SetDataByteOrderToLittleEndian();
        }
        else {
            reader->SetDataByteOrderToBigEndian();
        }

        try {
            reader->Update();
        }
        catch (...) {
            result.code = core::common::ErrorCode::IoFailed;
            result.message = QStringLiteral("读取 RAW 文件失败: %1").arg(file);
            return result;
        }

        result = buildModelFromImage(reader->GetOutput(), file);
        return result;
    }

    core::common::Result<core::data::VolumeModel> VolumeIOServiceVtk::buildModelFromImage(vtkImageData* image, const QString& source)
    {
        core::common::Result<core::data::VolumeModel> result;
        if (!image) {
            result.code = core::common::ErrorCode::IoFailed;
            result.message = QStringLiteral("无法从 %1 读取到有效的体数据").arg(source);
            return result;
        }

        // Register 以增加引用计数，避免原始 reader 析构后数据被释放。
        image->Register(nullptr);

        core::data::VolumeModel model;
        model.setImage(image);

        double spacing[3];
        image->GetSpacing(spacing);
        model.setSpacing({ spacing[0], spacing[1], spacing[2] });

        double origin[3];
        image->GetOrigin(origin);
        model.setOrigin({ origin[0], origin[1], origin[2] });

        int extent[6];
        image->GetExtent(extent);
        model.setExtent({ extent[0], extent[1], extent[2], extent[3], extent[4], extent[5] });

        double range[2];
        image->GetScalarRange(range);
        model.setScalarRange({ range[0], range[1] });

        vtkMatrix3x3* matrix = image->GetDirectionMatrix();
        double direction[9];
        std::copy_n(matrix->GetData(), 9, direction);
        model.setDirection({ direction[0], direction[1], direction[2], direction[3], direction[4], direction[5], direction[6], direction[7], direction[8] });

        result.code = core::common::ErrorCode::Ok;
        result.value = model;
        result.message = QStringLiteral("成功加载体数据: %1").arg(source);
        return result;
    }

} // namespace core::io