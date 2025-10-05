#ifndef InitParticles_H_
#define InitParticles_H_

#include <AMReX_Config.H>
#include <cstdlib>
#include <ctime>

/*---------------------------生成随机分布的颗粒-----------------------------------------------------------*/
void generateSpheres(int n, amrex::Real R, amrex::Real NX, amrex::Real NY, amrex::Real NZ, amrex::Vector<amrex::RealVect>& sphere_centers) {
    // 初始化随机数生成器
    std::srand(std::time(0));

    amrex::Real min_dist = 8 * R; // 球与球及球与盒子壁面的最小距离

    // 生成球心的循环
    for (int i = 0; i < n; ++i) {
        bool valid = false;
        amrex::RealVect new_center;

        // 保证新生成的球心符合要求
        while (!valid) {
            // 在有效范围内生成随机球心坐标 (x, y, z)
            amrex::Real x = min_dist + static_cast<amrex::Real>(std::rand()) / RAND_MAX * (NX - 2 * min_dist);
            amrex::Real y = min_dist + static_cast<amrex::Real>(std::rand()) / RAND_MAX * (NY - 2 * min_dist);
            amrex::Real z = min_dist + static_cast<amrex::Real>(std::rand()) / RAND_MAX * (NZ - 2 * min_dist);

            new_center = amrex::RealVect{x, y, z};

            // 检查新球心是否与之前的球保持足够的距离
            valid = true;
            for (const auto& center : sphere_centers) {
                amrex::Real distance = std::sqrt(std::pow(center[0] - x, 2) +
                                                 std::pow(center[1] - y, 2) +
                                                 std::pow(center[2] - z, 2));
                if (distance < 2 * R + min_dist) {
                    valid = false; // 距离太近，重新生成
                    break;
                }
            }
        }

        // 将合法的球心添加到向量中
        sphere_centers.push_back(new_center);
    }
}

/*---------------------------生成500个颗粒-----------------------------------------------------------*/
void generateSpheres(amrex::Real D, amrex::Real NX, amrex::Real NY, amrex::Real NZ, amrex::Vector<amrex::RealVect>& sphere_centers) {
    amrex::Real layer_spacing = 1.15 * D; // 层间距为 1.36D
    amrex::Real start_z = NZ - 0.65 * D;  // 最顶层球心位置

    // 每层放置 25x25 个球
    int n_layers = 5;
    int n_x = 10;
    int n_y = 10;
    amrex::Real x_spacing = (NX - 2 * 0.65 * D) / (n_x - 1); // x 方向球心间距
    amrex::Real y_spacing = (NY - 2 * 0.65 * D) / (n_y - 1); // y 方向球心间距

    for (int layer = 0; layer < n_layers; ++layer) {
        amrex::Real z = start_z - layer * layer_spacing; // 当前层的 z 坐标

        for (int i = 0; i < n_x; ++i) {
            for (int j = 0; j < n_y; ++j) {
                amrex::Real x = 0.65 * D + i * x_spacing; // 当前球的 x 坐标
                amrex::Real y = 0.65 * D + j * y_spacing; // 当前球的 y 坐标

                // 添加当前球心坐标到 sphere_centers
                sphere_centers.push_back(amrex::RealVect{x, y, z});
            }
        }
    }
}

/*---------------------------生成1232个颗粒-----------------------------------------------------------*/
// void generateSpheres(amrex::Real D, amrex::Real NX, amrex::Real NY, amrex::Real NZ, amrex::Vector<amrex::RealVect>& sphere_centers)
// {
//     amrex::Real layer_spacing   = 1.125 * D; // 层间距为 1.36D
//     amrex::Real start_z = NZ - 0.875 * D; // 最顶层球心位置

//     // 每层放置 25x25 个球
//     int n_layers = 28;
//     int n_x = 44;

//     for (int layer = 0; layer < n_layers; ++layer)
//     {
//         amrex::Real z = start_z - layer * layer_spacing; // 当前层的 z 坐标

//         for (int i = 0; i < n_x; ++i)
//         {
//             if(layer % 2 == 0)
//             {
//                 amrex::Real x = 0.875 * D + i * 1.125 * D; // 当前球的 x 坐标
//                 amrex::Real y = 0.75 * D;

//                 sphere_centers.push_back(amrex::RealVect{x, y, z});
//             }
//             else
//             {
//                 amrex::Real x = 0.75 * D + i * 1.125 * D;  // 当前球的 x 坐标
//                 amrex::Real y = 0.75 * D;

//                 sphere_centers.push_back(amrex::RealVect{x, y, z});
//             }
//         }
//     }
// }

#endif