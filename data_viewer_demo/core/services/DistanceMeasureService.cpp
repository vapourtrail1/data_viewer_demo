#include "core/services/DistanceMeasureService.h"
#include <cmath>  // std::sqrt
#include "core/data/volumeModel.h"

namespace core::services {

    DistanceMeasureService::DistanceMeasureService() = default;

    void DistanceMeasureService::bindVolume(const core::data::VolumeModel* volume)
    {
        // 只保存指针，不接管所有权
        volume_ = volume;

        // 一般绑定新 volume 的时候，清空旧的测量结果
        items_.clear();
        nextId_ = 1;
    }

    int DistanceMeasureService::addDistanceByVoxel(const std::array<int, 3>& p0Ijk,
        const std::array<int, 3>& p1Ijk)
    {
        if (!volume_) {
            // 还没有绑定体数据，无法根据 spacing/origin/direction 计算世界坐标
            return -1;
        }

        DistanceItem item;
        item.id = nextId_++;
        item.p0World = voxelToWorld(p0Ijk);
        item.p1World = voxelToWorld(p1Ijk);
        item.lengthMm = distance(item.p0World, item.p1World);

        items_.push_back(item);
        return item.id;
    }

    int DistanceMeasureService::addDistanceByWorld(const Point3D& p0World,
        const Point3D& p1World)
    {
        DistanceItem item;
        item.id = nextId_++;
        item.p0World = p0World;
        item.p1World = p1World;
        item.lengthMm = distance(p0World, p1World);

        items_.push_back(item);
        return item.id;
    }

    const std::vector<DistanceMeasureService::DistanceItem>&
        DistanceMeasureService::items() const
    {
        return items_;
    }

    void DistanceMeasureService::clear()
    {
        items_.clear();
        nextId_ = 1;
    }

    DistanceMeasureService::Point3D
        DistanceMeasureService::voxelToWorld(const std::array<int, 3>& ijk) const
    {
        Point3D out{};

        if (!volume_) {
            // 理论上上层已经检查过，这里只是防御
            out.x = static_cast<double>(ijk[0]);
            out.y = static_cast<double>(ijk[1]);
            out.z = static_cast<double>(ijk[2]);
            return out;
        }

        // 从 VolumeModel 取出几何信息
        auto spacing = volume_->spacing3();        // [sx, sy, sz]
        auto origin = volume_->origin3();         // [ox, oy, oz]
        auto dir = volume_->directionMatrix(); // 3x3，按行展开

        // 先把体素坐标乘 spacing，得到物理坐标（还没乘方向矩阵）
        const double sx = ijk[0] * spacing[0];
        const double sy = ijk[1] * spacing[1];
        const double sz = ijk[2] * spacing[2];

        // world = origin + R * (sx, sy, sz)
        // R = [ r00 r01 r02
        //       r10 r11 r12
        //       r20 r21 r22 ]
        out.x = origin[0]
            + dir[0] * sx + dir[1] * sy + dir[2] * sz;
        out.y = origin[1]
            + dir[3] * sx + dir[4] * sy + dir[5] * sz;
        out.z = origin[2]
            + dir[6] * sx + dir[7] * sy + dir[8] * sz;

        return out;
    }

    double DistanceMeasureService::distance(const Point3D& a, const Point3D& b)
    {
        const double dx = a.x - b.x;
        const double dy = a.y - b.y;
        const double dz = a.z - b.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

} // namespace core::services
