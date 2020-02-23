#pragma once

#include <vector>

#include <opencv2/core/core.hpp>

#include "human_pose.hpp"

namespace human_pose_estimation {
    std::vector<HumanPose> scaleHumanPose(const std::vector<HumanPose>& ref_poses, const std::vector<HumanPose>& input_poses);
    int get_angle_limb(HumanPose pose, std::pair<int, int> limbKeypointsId1, std::pair<int, int> limbKeypointsId2);
}  // namespace human_pose_estimation
