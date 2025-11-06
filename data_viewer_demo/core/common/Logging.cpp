#include "core/common/Logging.h"

namespace core::common {

// 注册实际的日志分类，供 QLoggingCategory 使用。
Q_LOGGING_CATEGORY(lcApp, "ctviewer.app")
Q_LOGGING_CATEGORY(lcIO, "ctviewer.io")
Q_LOGGING_CATEGORY(lcRender, "ctviewer.render")
Q_LOGGING_CATEGORY(lcMpr, "ctviewer.mpr")

} // namespace core::common
