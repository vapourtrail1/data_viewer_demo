#pragma once
#include <vector>
#include <array>

namespace core::data {
    class VolumeModel;   // 前向声明，避免头文件互相 include
}

namespace core::services {

    /*
     * DistanceMeasureService
     * 负责：
     *  - 绑定当前的 VolumeModel（DICOM -> vtkImageData 的那套）
     *  - 记录所有“两点距离”测量
     *  - 提供 IJK -> 世界坐标 -> mm 距离 的计算
     */
    class DistanceMeasureService
    {
    public:
        // 世界坐标下的三维点（单位：一般是 mm）
        struct Point3D
        {
            double x = 0.0;
            double y = 0.0;
            double z = 0.0;
        };

        // 一条距离测量记录
        struct DistanceItem
        {
            int id = 0;       // 简单的自增 ID，方便以后在 UI/表格里引用
            Point3D p0World;  // 起点世界坐标
            Point3D p1World;  // 终点世界坐标
            double lengthMm;  // 两点之间的直线距离（单位 mm）
        };

        DistanceMeasureService();

        /*
         * 绑定当前体数据。
         * 注意：这里只是保存指针，不负责管理 VolumeModel 的生命周期。
         */
        void bindVolume(const core::data::VolumeModel* volume);

        /*
         * 用体素索引（IJK）添加一条距离测量。
         * 传入的是体素坐标，内部会根据 VolumeModel 的 spacing / origin / directionMatrix
         * 转成世界坐标，然后计算距离。
         *
         * 返回：新建测量的 id；如果 volume 尚未绑定，返回 -1。
         */
        int addDistanceByVoxel(const std::array<int, 3>& p0Ijk,
            const std::array<int, 3>& p1Ijk);

        /*
         * 直接用世界坐标添加一条距离测量。
         * 如果你在别处已经算好了世界坐标，可以走这个接口。
         *
         * 返回：新建测量的 id。
         */
        int addDistanceByWorld(const Point3D& p0World,
            const Point3D& p1World);

        /*
         * 获取所有测量记录（只读）。
         */
        const std::vector<DistanceItem>& items() const;

        /*
         * 清空所有测量记录。
         * 一般在重新加载 DICOM 或新建工程时调用。
         */
        void clear();

    private:
        const core::data::VolumeModel* volume_ = nullptr; // 当前绑定的体数据（只借用，不接管）
        std::vector<DistanceItem> items_;
        int nextId_ = 1;                                  // 下一个可用 ID

        /*
         * 将体素坐标 (i, j, k) 转成世界坐标。
         * 假设 VolumeModel::directionMatrix() 返回的是按行展开的 3x3 矩阵：
         * [ r00 r01 r02  r10 r11 r12  r20 r21 r22 ]
         *
         * world = origin + R * (spacing ⊙ ijk)
         */
        Point3D voxelToWorld(const std::array<int, 3>& ijk) const;

        /*
         * 计算两点之间的欧式距离。
         */
        static double distance(const Point3D& a, const Point3D& b);
    };

} // namespace core::services
