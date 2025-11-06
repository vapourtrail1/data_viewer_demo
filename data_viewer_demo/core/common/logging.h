#pragma once
#include <QLoggingCategory>

namespace core::common {
	/**
    定义全局使用的日志分类，便于模块化过滤。
	*/
	Q_DECLARE_LOGGING_CATEGORY(lcApp)
	Q_DECLARE_LOGGING_CATEGORY(lcIO)
	Q_DECLARE_LOGGING_CATEGORY(lcRender)
	Q_DECLARE_LOGGING_CATEGORY(lcMpr)
} // namespace core::common