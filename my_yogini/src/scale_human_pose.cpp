#include <utility>
#include <vector>

#include <opencv2/imgproc/imgproc.hpp>

#include "human_pose_estimator.hpp"
#include "scale_human_pose.hpp"

namespace human_pose_estimation {

const int AnglesNumber = 13;
//std::vector<limb_angle> angles_array(AnglesNumber, limb_angle({0,0}, {0,0}, 0));
//limb_angle angle9({9,10}, {9,8}, 9);
//angles_array[9] = limb_angle({9,10}, {9,8}, 9);
//angles_array[11] = angle({11,12}, {11,10}, 11);
//angles_array[12] =angle({12,13}, {12,11}, 12);
    
const std::vector<std::pair<int, int> > AngleSides = {{0,0}, {1,1}, {3,1},{4,2},
                                                      {4,4}, {6,1}, {7,5},
                                                      {7,7}, {9,1}, {10,8},
                                                      {10,10}, {12,1}, {13,11}};
const std::vector<std::string > Joints = {"neck", "head", "right shoulder","right elbow",
                                                      "right hand", "left shoulder", "left elbow",
                                                      "left hand", "right hip", "right knee",
                                                      "foot", "left hip", "left knee"};
const cv::Point2f absentKeypoint(-1.0f, -1.0f);
void draw_arrows(cv::Mat& image, const std::vector<HumanPose>& scaled_poses, const std::vector<HumanPose>& input_poses){
    const std::vector<cv::Scalar> colors = {
        cv::Scalar(255, 0, 0), cv::Scalar(255, 85, 0), cv::Scalar(255, 170, 0),
        cv::Scalar(255, 255, 0), cv::Scalar(170, 255, 0), cv::Scalar(85, 255, 0),
        cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 85), cv::Scalar(0, 255, 170),
        cv::Scalar(0, 255, 255), cv::Scalar(0, 170, 255), cv::Scalar(0, 85, 255),
        cv::Scalar(0, 0, 255), cv::Scalar(85, 0, 255), cv::Scalar(170, 0, 255),
        cv::Scalar(255, 0, 255), cv::Scalar(255, 0, 170), cv::Scalar(255, 0, 85)
    };
    for (size_t keypointIdx = 0; keypointIdx < 14; keypointIdx++) {
        //scaled_poses[0].keypoints.size()
        if (scaled_poses[0].keypoints[keypointIdx] != absentKeypoint) {
            cv::arrowedLine(image, input_poses[0].keypoints[keypointIdx], scaled_poses[0].keypoints[keypointIdx], colors[keypointIdx],4, 4, 0, 0.2);
        }
    }
}
std::vector<int> extract_angles(HumanPose pose, std::vector<int> restricted_angles){
    // pose, {9,  11, 12}
    std::vector<int> output(AnglesNumber, -1);
    for (auto& angle_i : restricted_angles){
            //std::cout << angle_i << std::endl;
            output[angle_i] = get_angle_limb(pose,{angle_i,AngleSides[angle_i].first}, {angle_i,AngleSides[angle_i].second});
            //std::cout << output[angle_i]<< std::endl;
    } 
    return output;
}
int get_angle(cv::Point diff1, cv::Point diff2){
    float dot = diff1.x * diff2.x + diff1.y * diff2.y; // dot product between [x1, y1] and [x2, y2]
    float det = diff1.x * diff2.y - diff1.y * diff2.x; // x1*y2 - y1*x2      # determinant 
    int rot_angle = static_cast<int>(std::atan2(det, dot) );//* 180 / CV_PI); //atan2(det, dot)  # atan2(y, x) or atan2(sin, cos)
    return rot_angle;   
}
cv::Point2f rotate2d(const cv::Point2f& inPoint, const double& angRad)
{
    cv::Point2f outPoint;
    //CW rotation
    outPoint.x = std::cos(angRad)*inPoint.x - std::sin(angRad)*inPoint.y;
    outPoint.y = std::sin(angRad)*inPoint.x + std::cos(angRad)*inPoint.y;
    return outPoint;
}

HumanPose tieToFloor(HumanPose scaled_pose, HumanPose pose, std::vector<int> ref_angles){
   double Y_ground= ( pose.keypoints[10].y + pose.keypoints[13].y)/2;
    // leg 1: 11 12 13
    if (ref_angles[11] == -1){
        std::cout << "Inside Tie 11: Check1" << std::endl;
        // rotate 13 around 11
        cv::Point2f center = scaled_pose.keypoints[11];
        //std::cout << scaled_pose.keypoints[8].x <<", "<< scaled_pose.keypoints[8].y<< std::endl;
        double y10 = scaled_pose.keypoints[10].y ; // make 10 and 13 equal Y
        cv::Point difference = scaled_pose.keypoints[13] - center; // 13 old and 11 (x,y)
        double L2 = difference.x * difference.x + difference.y * difference.y;
        double delta_y10 = center.y - y10; // 10 new and 8 
        double delta_x10 = std::sqrt(L2 - delta_y10*delta_y10 ); // 13 new and 11 
        
         std::cout << "y dlta"<< y10-scaled_pose.keypoints[13].y << std::endl;
        if (scaled_pose.keypoints[12].x >scaled_pose.keypoints[9].x){
            cv::Point2f pt10 (center.x + delta_x10, y10);
            scaled_pose.keypoints[13] = pt10; // 13 moved
        }
        else {
            cv::Point2f pt10 (center.x - delta_x10, y10);
             scaled_pose.keypoints[13] = pt10; // 13 moved
        }
       
        
        std::cout << "Inside Tie: Check2" << std::endl;
        // get rotation angle about 11
        cv::Point differenceNew = scaled_pose.keypoints[13] - center;  // 13 new and 11
        int rot_angle = get_angle(difference, differenceNew);
        std::cout << "Inside Tie: Check3" << std::endl;
        // rotate the knee (12)
        
        scaled_pose.keypoints[12] = rotate2d(scaled_pose.keypoints[12] - center, rot_angle) + center;
        
        std::cout << "Inside Tie : Check4" << std::endl;
        
    }
    // leg 2: 8 9 10
    if (ref_angles[8] == -1){
        std::cout << "Inside Tie 8: Check1" << std::endl;
        // rotate 10 around 8
        cv::Point2f center = scaled_pose.keypoints[8];
        //std::cout << scaled_pose.keypoints[8].x <<", "<< scaled_pose.keypoints[8].y<< std::endl;
        double y10 = scaled_pose.keypoints[13].y ; // make 10 and 13 equal Y
        cv::Point difference = scaled_pose.keypoints[10] - center; // 10 old and 8 (x,y)
        double L2 = difference.x * difference.x + difference.y * difference.y;
        double delta_y10 = center.y - y10; // 10 new and 8 
        double delta_x10 = std::sqrt(L2 - delta_y10*delta_y10 ); // 10 new and 8 
 
        if (scaled_pose.keypoints[9].x >scaled_pose.keypoints[12].x){
            cv::Point2f pt10 (center.x + delta_x10, y10);
            scaled_pose.keypoints[10] = pt10; // 13 moved
        }
        else {
            cv::Point2f pt10 (center.x - delta_x10, y10);
             scaled_pose.keypoints[10] = pt10; // 10 moved
        }
        
        std::cout << "Inside Tie: Check2" << std::endl;
        // get rotation angle about 8
        cv::Point differenceNew = scaled_pose.keypoints[10] - center;  // 10 new and 8
        int rot_angle = get_angle(difference, differenceNew);
        std::cout << "Inside Tie: Check3" << std::endl;
        // rotate the knee (9)
        
        scaled_pose.keypoints[9] = rotate2d(scaled_pose.keypoints[9] - center, rot_angle) + center;
        
        std::cout << "Inside Tie: Check4" << std::endl;
        
    }
    if (scaled_pose.keypoints[10].y == scaled_pose.keypoints[13].y){
        double shift = Y_ground - scaled_pose.keypoints[10].y ;
        std::cout << "shift"<<shift << std::endl;
        for( int a = 0; a < 18; a = a + 1 ) {
            if (scaled_pose.keypoints[a].y != -1){
                //std::cout << a<< "\t"<< scaled_pose.keypoints[a].y << std::endl;
                scaled_pose.keypoints[a].y = shift + scaled_pose.keypoints[a].y;
                //std::cout << a<< "\t"<< scaled_pose.keypoints[a].y << std::endl;
            }
        }
        
    }
    return scaled_pose;
}
int get_angle_limb(HumanPose pose, std::pair<int, int> limbKeypointsId1, std::pair<int, int> limbKeypointsId2){
    // Input example: pose, {8, 9},   {9, 10}
    std::pair<cv::Point2f, cv::Point2f> limbKeypoints1(pose.keypoints[limbKeypointsId1.first],pose.keypoints[limbKeypointsId1.second]);
    std::pair<cv::Point2f, cv::Point2f> limbKeypoints2(pose.keypoints[limbKeypointsId2.first],pose.keypoints[limbKeypointsId2.second]);
    if (limbKeypoints1.second == cv::Point2f(-1.0f, -1.0f)) {return -1;}
    if (limbKeypoints1.first == cv::Point2f(-1.0f, -1.0f)) {return -1;}
    if (limbKeypoints2.second == cv::Point2f(-1.0f, -1.0f)) {return -1;}
    if (limbKeypoints2.first == cv::Point2f(-1.0f, -1.0f)) {return -1;}
    cv::Point limb1 = limbKeypoints1.second - limbKeypoints1.first;
    cv::Point limb2 = limbKeypoints2.second - limbKeypoints2.first;
    float dot = limb1.x * limb2.x + limb1.y * limb2.y; // dot product between [x1, y1] and [x2, y2]
    float det = limb1.x * limb2.y - limb1.y * limb2.x; // x1*y2 - y1*x2      # determinant
    int angle = static_cast<int>(std::atan2(det, dot) * 180 / CV_PI); //atan2(det, dot)  # atan2(y, x) or atan2(sin, cos)
    return angle;
}

std::stringstream compare_angles(const std::vector<HumanPose>& ref_poses, const std::vector<HumanPose>& input_poses, std::vector<int> restricted_angles){
    std::vector<int> user_angles = extract_angles(input_poses[0],restricted_angles); // for warrior 1 flipped
    std::vector<int> ref_angles = extract_angles(ref_poses[0], restricted_angles); // for warrior 1 flipped
    std::stringstream ang_report;
    for (int a = 0; a < AnglesNumber+1; a = a + 1){
        if(user_angles[a] != -1 && ref_angles[a] != -1){
            int ang_error= (user_angles[a]-ref_angles[a]);
            if (ang_error >5){
                ang_report << "The angle at (" << int(a) << ") "<< Joints[a] << " should be "<< ref_angles[a]<< ". \n";
            }
        }
    }
    std::cout << ang_report.str();
    return ang_report;
    
}
std::vector<HumanPose> scaleHumanPose(const std::vector<HumanPose>& ref_poses, const std::vector<HumanPose>& input_poses, std::vector<int> restricted_angles) {
    
    
    const std::vector<std::pair<int, int> > limbKeypointsIds = {
        {1, 2},  {1, 5},   {2, 3},
        {3, 4},  {5, 6},   {6, 7},
        {1, 8},  {8, 9},   {9, 10},
        {1, 11}, {11, 12}, {12, 13},
        {1, 0},  {0, 14},  {14, 16},
        {0, 15}, {15, 17}
    };
   

    //const int stickWidth = 4;
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
        std::vector<int> ref_angles = extract_angles(ref_poses[0], restricted_angles); // for warrior 1 flipped
        std::cout << "Check2" << std::endl;
        HumanPose scaled_pose_tied = tieToFloor(scaled_pose, pose, ref_angles);
        std::cout << "Check3" << std::endl;
        scaled_poses.push_back(scaled_pose_tied);
    }
    
        
    return scaled_poses;
    
}

}  // namespace human_pose_estimation
