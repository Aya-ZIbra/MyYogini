#include <utility>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

#include "human_pose_estimator.hpp"
#include "scale_human_pose.hpp"

namespace human_pose_estimation {
int get_angle_limb(HumanPose pose, std::pair<int, int> limbKeypointsId1, std::pair<int, int> limbKeypointsId2){
    // Input example: pose, {8, 9},   {9, 10}
    std::pair<cv::Point2f, cv::Point2f> limbKeypoints1(pose.keypoints[limbKeypointsId1.first],pose.keypoints[limbKeypointsId1.second]);
    std::pair<cv::Point2f, cv::Point2f> limbKeypoints2(pose.keypoints[limbKeypointsId2.first],pose.keypoints[limbKeypointsId2.second]);
    cv::Point limb1 = limbKeypoints1.second - limbKeypoints1.first;
    cv::Point limb2 = limbKeypoints2.second - limbKeypoints2.first;
    float dot = limb1.x * limb2.x + limb1.y * limb2.y; // dot product between [x1, y1] and [x2, y2]
    float det = limb1.x * limb2.y - limb1.y * limb2.x; // x1*y2 - y1*x2      # determinant
    int angle = static_cast<int>(std::atan2(det, dot) * 180 / CV_PI); //atan2(det, dot)  # atan2(y, x) or atan2(sin, cos)
    return angle;
}
std::vector<HumanPose> scaleHumanPose(const std::vector<HumanPose>& ref_poses, const std::vector<HumanPose>& input_poses) {
    
    const std::vector<cv::Scalar> colors = {
        cv::Scalar(255, 0, 0), cv::Scalar(255, 85, 0), cv::Scalar(255, 170, 0),
        cv::Scalar(255, 255, 0), cv::Scalar(170, 255, 0), cv::Scalar(85, 255, 0),
        cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 85), cv::Scalar(0, 255, 170),
        cv::Scalar(0, 255, 255), cv::Scalar(0, 170, 255), cv::Scalar(0, 85, 255),
        cv::Scalar(0, 0, 255), cv::Scalar(85, 0, 255), cv::Scalar(170, 0, 255),
        cv::Scalar(255, 0, 255), cv::Scalar(255, 0, 170), cv::Scalar(255, 0, 85)
    };
    const std::vector<std::pair<int, int> > limbKeypointsIds = {
        {1, 2},  {1, 5},   {2, 3},
        {3, 4},  {5, 6},   {6, 7},
        {1, 8},  {8, 9},   {9, 10},
        {1, 11}, {11, 12}, {12, 13},
        {1, 0},  {0, 14},  {14, 16},
        {0, 15}, {15, 17}
    };
   

    const int stickWidth = 4;
    const cv::Point2f absentKeypoint(-1.0f, -1.0f);
    const int keypointsNumber = HumanPoseEstimator::keypointsNumber;
    
    
    HumanPose ref_pose = ref_poses[0];
    std::cout << "Check1" << std::endl;
    std::vector<HumanPose> scaled_poses;
    
    for (const auto& pose : input_poses){
        HumanPose scaled_pose(std::vector<cv::Point2f>(keypointsNumber, cv::Point2f(-1.0f, -1.0f)), pose.score);
        scaled_pose.keypoints[1] =  pose.keypoints[1] ;
        for (const auto& limbKeypointsId : limbKeypointsIds) {
            std::pair<cv::Point2f, cv::Point2f> limbKeypoints(pose.keypoints[limbKeypointsId.first],
                    pose.keypoints[limbKeypointsId.second]);
            if (limbKeypoints.first == absentKeypoint
                    || limbKeypoints.second == absentKeypoint) {
                continue;
            }
            cv::Point difference_input = limbKeypoints.second - limbKeypoints.first;
            double L = std::sqrt(difference_input.x * difference_input.x + difference_input.y * difference_input.y);
            
            cv::Point2f difference_ref= ref_pose.keypoints[limbKeypointsId.second]- ref_pose.keypoints[limbKeypointsId.first];
            double L_ref = std::sqrt(difference_ref.x * difference_ref.x +difference_ref.y * difference_ref.y);
            
            cv::Point2f delta (L * difference_ref.x/ L_ref, L * difference_ref.y/ L_ref);
            scaled_pose.keypoints[limbKeypointsId.second] = scaled_pose.keypoints[limbKeypointsId.first] + delta;
        }
        std::cout << "Check2" << std::endl;
        scaled_poses.push_back(scaled_pose);
    }
    
        
    return scaled_poses;
    
}
}  // namespace human_pose_estimation