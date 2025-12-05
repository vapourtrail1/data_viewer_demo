#pragma once
#include <array>
#include <vector>

namespace core::data {
    class VolumeModel;  // 前置声明：体数据模型
}

namespace core::services {

    /*
     *  提供三点角度（以 p1 为顶点）测量的后端服务，支持基于体素与世界坐标的计算。 
     */
    class AngleMeasureService
    {
    public:
        // 三维点（世界坐标，单位：mm）
        struct Point3D
        {
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
        };

        // 记录一次角度测量
        struct AngleItem
        {
            int id = 0;           // 简单的递增 ID
            Point3D p0World;      // 起点
            Point3D p1World;      // 顶点（角度所依赖的点）
            Point3D p2World;      // 终点
            double angleDegree;   // 夹角大小，单位：度
        };

        AngleMeasureService();

        // 绑定当前 VolumeModel，仅保存指针以便使用 spacing/origin/direction 信息
        void bindVolume(const core::data::VolumeModel* volume);

        // 通过 IJK 坐标添加一条角度记录，未绑定体数据时返回 -1
        int addAngleByVoxel(const std::array<int, 3>& p0Ijk,
            const std::array<int, 3>& p1Ijk,
            const std::array<int, 3>& p2Ijk);

        // 直接使用世界坐标添加角度记录
        int addAngleByWorld(const Point3D& p0World,
            const Point3D& p1World,
            const Point3D& p2World);

        // 只读访问所有记录
        const std::vector<AngleItem>& items() const;

        // 清空所有记录并重置 ID
        void clear();

    private:
        const core::data::VolumeModel* volume_ = nullptr; // 当前绑定的体数据
        std::vector<AngleItem> items_;                    // 记录集合
        int nextId_ = 1;                                  // 下一个可用 ID

        // 将 IJK 坐标转换为世界坐标
        Point3D voxelToWorld(const std::array<int, 3>& ijk) const;

        // 计算三点夹角（顶点为 p1），返回角度（度）
        static double angleDegree(const Point3D& p0,
            const Point3D& p1,
            const Point3D& p2);
    };

} // namespace core::services