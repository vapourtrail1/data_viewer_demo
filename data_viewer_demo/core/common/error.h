#pragma once


#include <QString>

namespace core::common {

    /**
     错误码枚举，覆盖常见 IO/渲染错误。
     */
    enum class ErrorCode {
        Ok = 0,
        IoFailed,
        Not3D,
        PixelUnsupported,
        RawParamMissing,
        InvalidArgument,
        OutOfRange,
        Unknown,
    };

    /**
    泛型结构，包含返回值、错误码与额外信息。
    */
    template <typename T>
    struct Result {
        ErrorCode code = ErrorCode::Unknown;
        T value{};
        QString message;

        bool ok() const { return code == ErrorCode::Ok; }
    };

    /**
     将错误码转换为可读字符串。
     */
    QString errorCodeToString(ErrorCode code);

} // namespace core::common