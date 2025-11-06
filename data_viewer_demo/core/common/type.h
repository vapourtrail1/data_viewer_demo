#pragma once
#include <array>
#include <utility>

namespace core::common {

	/**
	WL 表示窗宽/窗位值对。
	 */
	using WL = std::pair<double, double>;

	/**
	 默认窗宽/窗位，作为启动时的兜底值。
	 */
	constexpr WL DefaultWL{ 400.0, 40.0 };
    
	/**
     默认像素间距，当输入数据缺失 spacing 信息时兜底使用。
	 */
	constexpr std::array<double, 3> DefaultSpacing{ 1.0, 1.0, 1.0 };

} // namespace core::common