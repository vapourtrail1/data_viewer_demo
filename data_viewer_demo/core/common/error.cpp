#include "core/common/error.h"

namespace core::common {

    QString errorCodeToString(ErrorCode code)
    {
        // 将错误码映射到简洁的字符串，方便日志打印。
        switch (code) {
        case ErrorCode::Ok:
            return QStringLiteral("Ok");
        case ErrorCode::IoFailed:
            return QStringLiteral("IoFailed");
        case ErrorCode::Not3D:
            return QStringLiteral("Not3D");
        case ErrorCode::PixelUnsupported:
            return QStringLiteral("PixelUnsupported");
        case ErrorCode::RawParamMissing:
            return QStringLiteral("RawParamMissing");
        case ErrorCode::InvalidArgument:
            return QStringLiteral("InvalidArgument");
        case ErrorCode::OutOfRange:
            return QStringLiteral("OutOfRange");
        case ErrorCode::Unknown:
        default:
            return QStringLiteral("Unknown false");
        }
    }
} // namespace core::common