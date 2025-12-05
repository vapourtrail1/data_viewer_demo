#include "core/services/AngleMeasureService.h"
#include <cmath>
#include "core/data/volumeModel.h"

namespace core::services {

    AngleMeasureService::AngleMeasureService() = default;

    void AngleMeasureService::bindVolume(const core::data::VolumeModel* volume)
    {
        // 仅保存指针，便于读取 spacing/origin/direction 信息进行 IJK->世界坐标转换
        volume_ = volume;

        // 切换数据源时，清空旧的测量记录并重置计数器
        items_.clear();
        nextId_ = 1;
    }

    int AngleMeasureService::addAngleByVoxel(const std::array<int, 3>& p0Ijk,
        const std::array<int, 3>& p1Ijk,
        const std::array<int, 3>& p2Ijk)
    {
        if (!volume_) {
            // 没有绑定体数据时无法正确换算世界坐标
            return -1;
        }

        AngleItem item;
        item.id = nextId_++;
        item.p0World = voxelToWorld(p0Ijk);
        item.p1World = voxelToWorld(p1Ijk);
        item.p2World = voxelToWorld(p2Ijk);
        item.angleDegree = angleDegree(item.p0World, item.p1World, item.p2World);

        items_.push_back(item);
        return item.id;
    }

    int AngleMeasureService::addAngleByWorld(const Point3D& p0World,
        const Point3D& p1World,
        const Point3D& p2World)
    {
        AngleItem item;
        item.id = nextId_++;
        item.p0World = p0World;
        item.p1World = p1World;
        item.p2World = p2World;
        item.angleDegree = angleDegree(p0World, p1World, p2World);

        items_.push_back(item);
        return item.id;
    }

    const std::vector<AngleMeasureService::AngleItem>& AngleMeasureService::items() const
    {
        return items_;
    }

    void AngleMeasureService::clear()
    {
        // 清理缓存并重置 ID 计数
        items_.clear();
        nextId_ = 1;
    }

    AngleMeasureService::Point3D AngleMeasureService::voxelToWorld(const std::array<int, 3>& ijk) const
    {
        Point3D out{}; // 使用聚合初始化保证成员清零

        if (!volume_) {
            // 未绑定体数据时，直接将 IJK 视为世界坐标返回
            out.x = static_cast<double>(ijk[0]);
            out.y = static_cast<double>(ijk[1]);
            out.z = static_cast<double>(ijk[2]);
            return out;
        }

        // VolumeModel 提供 spacing/origin/direction 信息
        auto spacing = volume_->spacing3();        // [sx, sy, sz]
        auto origin = volume_->origin3();          // [ox, oy, oz]
        auto dir = volume_->directionMatrix();     // 3x3 展开数组

        // 先将 IJK 缩放到物理尺寸
        const double sx = ijk[0] * spacing[0];
        const double sy = ijk[1] * spacing[1];
        const double sz = ijk[2] * spacing[2];

        // world = origin + R * (sx, sy, sz)
        out.x = origin[0] + dir[0] * sx + dir[1] * sy + dir[2] * sz;
        out.y = origin[1] + dir[3] * sx + dir[4] * sy + dir[5] * sz;
        out.z = origin[2] + dir[6] * sx + dir[7] * sy + dir[8] * sz;

        return out;
    }

    double AngleMeasureService::angleDegree(const Point3D& p0,
        const Point3D& p1,
        const Point3D& p2)
    {
        // 计算向量（p0-p1）与（p2-p1）的夹角
        const double v1x = p0.x - p1.x;
        const double v1y = p0.y - p1.y;
        const double v1z = p0.z - p1.z;

        const double v2x = p2.x - p1.x;
        const double v2y = p2.y - p1.y;
        const double v2z = p2.z - p1.z;

        const double dot = v1x * v2x + v1y * v2y + v1z * v2z;
        const double len1 = std::sqrt(v1x * v1x + v1y * v1y + v1z * v1z);
        const double len2 = std::sqrt(v2x * v2x + v2y * v2y + v2z * v2z);

        if (len1 <= 0.0 || len2 <= 0.0) {
            // 任意向量长度为 0 时，无法计算有效角度
            return 0.0;
        }

        double cosTheta = dot / (len1 * len2);
        // 数值稳定性处理，避免精度误差导致反余弦域外
        if (cosTheta > 1.0) cosTheta = 1.0;
        if (cosTheta < -1.0) cosTheta = -1.0;

        const double radians = std::acos(cosTheta);
        constexpr double radToDegree = 180.0 / 3.14159265358979323846;
        return radians * radToDegree;
    }

} // namespace core::services

