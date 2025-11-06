#pragma once
namespace core::io {

    /**
    描述 RAW 体数据所需的额外参数。
    */
    struct RawParam {
        int dimX = 0;
        int dimY = 0;
        int dimZ = 0;
        int bytesPerPixel = 1;
        bool littleEndian = true;
        bool isSigned = false;
    };

} // namespace core::io