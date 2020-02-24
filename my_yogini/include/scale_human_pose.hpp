#pragma once

#include <vector>

#include <opencv2/core/core.hpp>

#include "human_pose.hpp"

namespace human_pose_estimation {
    std::vector<int> extract_angles(HumanPose pose, std::vector<int> restricted_angles);
    std::vector<HumanPose> scaleHumanPose(const std::vector<HumanPose>& ref_poses, const std::vector<HumanPose>& input_poses);
    int get_angle_limb(HumanPose pose, std::pair<int, int> limbKeypointsId1, std::pair<int, int> limbKeypointsId2);
    struct limb_angle {
    limb_angle(const std::pair<int, int>& limbKeypointsId1 = std::pair<int, int>(), 
               const std::pair<int, int>& limbKeypointsId2 = std::pair<int, int>(), 
               const int& angleID= 0){};

    std::pair<int, int> limbKeypointsId1;
    std::pair<int, int> limbKeypointsId2;
    int angleID;
    };
}  // namespace human_pose_estimation
