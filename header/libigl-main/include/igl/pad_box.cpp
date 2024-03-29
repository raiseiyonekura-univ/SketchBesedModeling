#include "pad_box.h"

template <typename Scalar, int DIM>
IGL_INLINE void igl::pad_box(
  const Scalar pad,
  Eigen::AlignedBox<Scalar,DIM> & box)
{
  box.min().array() -= pad;
  box.max().array() += pad;
}

#ifdef IGL_STATIC_LIBRARY
// Explicit template instantiation
// generated by autoexplicit.sh
template void igl::pad_box<double, 2>(double, Eigen::AlignedBox<double, 2>&);
template void igl::pad_box<double, 3>(double, Eigen::AlignedBox<double, 3>&);
#endif
