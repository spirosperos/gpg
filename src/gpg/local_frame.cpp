#include <gpg/local_frame.h>
#include <boost/lexical_cast.hpp>


void LocalFrame::print()
{
  std::cout << "sample: " << sample_.transpose() << std::endl;
  std::cout << "curvature_axis: " << curvature_axis_.transpose() << std::endl;
  std::cout << "normal: " << normal_.transpose() << std::endl;
  std::cout << "binormal: " << binormal_.transpose() << std::endl;
  std::cout << "-----------\n";
}


void LocalFrame::findAverageNormalAxis(const Eigen::MatrixXd &normals)
{
  // 1. Calculate "curvature axis" (corresponds to minor principal curvature axis).
  Eigen::Matrix3d M = normals * normals.transpose();
  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver(M); // M is adjoint (M is equal to its transpose)
  Eigen::Vector3d eigen_values = eigen_solver.eigenvalues().real();
  Eigen::Matrix3d eigen_vectors = eigen_solver.eigenvectors().real();
  int min_index;
  eigen_values.minCoeff(&min_index);
  curvature_axis_ = eigen_vectors.col(min_index);

  // 2. Calculate surface normal.
  int max_index;
  eigen_values.maxCoeff(&max_index);
  normal_ = eigen_vectors.col(max_index);

  // 3. Ensure that the new normal is pointing in the same direction as the existing normals.
  Eigen::Vector3d avg_normal = normals.rowwise().sum();
  avg_normal /= avg_normal.norm();
  if (avg_normal.transpose() * normal_ < 0)
  {
    normal_ *= -1.0;
  }

  // 4. Create binormal (corresponds to major principal curvature axis).
  binormal_ = curvature_axis_.cross(normal_);
}


void LocalFrame::plotAxes(void* viewer_void, int id) const
{
  boost::shared_ptr<pcl::visualization::PCLVisualizer> viewer
    = *static_cast<boost::shared_ptr<pcl::visualization::PCLVisualizer> *>(viewer_void);

  pcl::PointXYZ p, q, r;
  p.x = sample_(0);
  p.y = sample_(1);
  p.z = sample_(2);
  q.x = p.x + 0.02 * curvature_axis_(0);
  q.y = p.y + 0.02 * curvature_axis_(1);
  q.z = p.z + 0.02 * curvature_axis_(2);
  r.x = p.x + 0.02 * normal_(0);
  r.y = p.y + 0.02 * normal_(1);
  r.z = p.z + 0.02 * normal_(2);
  viewer->addLine<pcl::PointXYZ>(p, q, 0, 0, 255, "curvature_axis_" + boost::lexical_cast<std::string>(id));
  viewer->addLine<pcl::PointXYZ>(p, r, 255, 0, 0, "normal_axis_" + boost::lexical_cast<std::string>(id));
}
