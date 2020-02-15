OPENVINO_FLAGS=-I$(INTEL_CVSDK_DIR)/deployment_tools/inference_engine/include \
	    -I$(INTEL_CVSDK_DIR)/deployment_tools/inference_engine/samples/common/ \
	    -I$(INTEL_CVSDK_DIR)/deployment_tools/inference_engine/src/extension/ \
	    -I$(INTEL_CVSDK_DIR)/opencv/include \
	    -L$(INTEL_CVSDK_DIR)/deployment_tools/inference_engine/lib/intel64 \
	    -L$(INTEL_CVSDK_DIR)/opencv/lib \
            -L/opt/intel/openvino/deployment_tools/inference_engine/samples/build/intel64/Release/lib/ \
	    -lgflags -linference_engine -ldl -lpthread -lcpu_extension_avx2 \
            -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_video -lgflags \


all: human_pose_estimation_demo
clean:
	rm -f *.o human_pose_estimation_demo


human_pose_estimation_demo: main.cpp human_pose.cpp human_pose_estimator.cpp render_human_pose.cpp peak.cpp scale_human_pose.cpp
	ln -sf /opt/intel/openvino_2019.3.334/deployment_tools/inference_engine/lib/intel64/libcpu_extension_avx2.so libcpu_extension.so
	g++ -fPIE -O3 -c -o human_pose.o --std=c++11 human_pose.cpp -fopenmp -I. -DUSE_OPENCV ${OPENVINO_FLAGS} -I./
	g++ -fPIE -O3 -c -o human_pose_estimator.o --std=c++11 human_pose_estimator.cpp -fopenmp -I. -DUSE_OPENCV ${OPENVINO_FLAGS} -I./
	g++ -fPIE -O3 -c -o render_human_pose.o --std=c++11 render_human_pose.cpp -fopenmp -I. -DUSE_OPENCV ${OPENVINO_FLAGS} -I./
	g++ -fPIE -O3 -c -o scale_human_pose.o --std=c++11 scale_human_pose.cpp -fopenmp -I. -DUSE_OPENCV ${OPENVINO_FLAGS} -I./
	g++ -fPIE -O3 -c -o peak.o --std=c++11 peak.cpp -fopenmp -I. -DUSE_OPENCV ${OPENVINO_FLAGS} -I./
	g++ -fPIE -O3 -o human_pose_estimation_demo --std=c++11 main.cpp -fopenmp -I. -DUSE_OPENCV ${OPENVINO_FLAGS} -I./ human_pose.o \
    -I./ human_pose_estimator.o -I./ render_human_pose.o -I./ peak.o -I./ scale_human_pose.o

