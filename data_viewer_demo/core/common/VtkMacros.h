#pragma once

/**
 * 若编译环境可用则自动启用 USE_VTK 宏。
 *
 * 若用户未在构建系统中手动定义 USE_VTK，此处会尝试通过头文件探测
 * QVTKOpenGLNativeWidget 以及 vtkSmartPointer 的可用性来决定是否开启。
 * 这样可以避免在具备 VTK 的环境中误判为未启用，从而影响三向联动视图。
 */
#if !defined(USE_VTK)
#  if defined(__has_include)
#    if __has_include(<QVTKOpenGLNativeWidget.h>) && __has_include(<vtkSmartPointer.h>)
#      define USE_VTK 1
#    else
#      define USE_VTK 0
#    endif
#  else
#    define USE_VTK 0
#  endif
#endif