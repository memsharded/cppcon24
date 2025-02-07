#include <tensorflow/lite/model.h>
#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/optional_debug_tools.h>
#include <tensorflow/lite/string_util.h>

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include <memory>

const int num_keypoints = 17;
const float confidence_threshold = 0.2;

// The Output is a float32 tensor of shape [1, 1, 17, 3].

// The first two channels of the last dimension represents the yx coordinates (normalized
// to image frame, i.e. range in [0.0, 1.0]) of the 17 keypoints (in the order of: [nose,
// left eye, right eye, left ear, right ear, left shoulder, right shoulder, left elbow,
// right elbow, left wrist, right wrist, left hip, right hip, left knee, right knee, left
// ankle, right ankle]).

// The third channel of the last dimension represents the prediction confidence scores of
// each keypoint, also in the range [0.0, 1.0].

const std::vector<std::pair<int, int>> connections = {
    {0, 1}, {0, 2}, {1, 3}, {2, 4}, {5, 6}, {5, 7}, 
    {7, 9}, {6, 8}, {8, 10}, {5, 11}, {6, 12}, {11, 12}, 
    {11, 13}, {13, 15}, {12, 14}, {14, 16}
};

void draw_keypoints(cv::Mat &resized_image, float *output)
{
    int square_dim = resized_image.rows;

    int offset = 75;
    // draw skeleton
    for (const auto &connection : connections) {
        int index1 = connection.first;
        int index2 = connection.second;
        float y1 = output[index1 * 3];
        float x1 = output[index1 * 3 + 1];
        float conf1 = output[index1 * 3 + 2];
        float y2 = output[index2 * 3];
        float x2 = output[index2 * 3 + 1];
        float conf2 = output[index2 * 3 + 2];

        if (conf1 > confidence_threshold && conf2 > confidence_threshold) {
            int img_x1 = offset+static_cast<int>(x1 * square_dim);
            int img_y1 = static_cast<int>(y1 * square_dim);
            int img_x2 = offset+static_cast<int>(x2 * square_dim);
            int img_y2 = static_cast<int>(y2 * square_dim);
            cv::line(resized_image, cv::Point(img_x1, img_y1), cv::Point(img_x2, img_y2), cv::Scalar(0, 255, 0), 3);
        }
    }
}

float scale = 100;

class Ball{
    public:
        float x=0.0f;
        float y=0.0f;
        float vx = 3.0f;
        float vy= 0.0f;
        float rad =0.4;
        float acc = 40;
        int r = 0;
        int g = 0;
        int b = 255;
        Ball(){
            if(rand() % 2)
                x = 4;
            r = rand() % 255;
            g = rand() % 255;
            b = rand() % 255;
        }
        void move(float t){
            x += vx * t;
            y += vy * t + acc * t *t;
            vy += acc * t;
        }
        void limits(){
            if (y > 5){
                y = 5; 
                vy = -vy;
            }
            if (y < 0){
                y = 0; 
                vy = -vy + 0.5;
            }
            if (x > 6.5){
                x = 6.5;
                vx = -vx;
            }
            if (x < 0){
                x = 0;
                vx = -vx;
            }
        }
        void draw(cv::Mat& frame){
            cv::circle(frame, cv::Point(x * scale, y * scale), rad * scale, cv::Scalar(r, g, b), -1);
        }
};
int main(int argc, char *argv[]) {

    // model from https://tfhub.dev/google/lite-model/movenet/singlepose/lightning/tflite/float16/4
    // A convolutional neural network model that runs on RGB images and predicts human
    // joint locations of a single person. The model is designed to be run in the browser
    // using Tensorflow.js or on devices using TF Lite in real-time, targeting
    // movement/fitness activities. This variant: MoveNet.SinglePose.Lightning is a lower
    // capacity model (compared to MoveNet.SinglePose.Thunder) that can run >50FPS on most
    // modern laptops while achieving good performance.
    std::string model_file = "assets/lite-model_movenet_singlepose_lightning_tflite_float16_4.tflite";
    // Video by Olia Danilevich from https://www.pexels.com/
    //std::string video_file = "assets/dancing.mp4";
     //Open the default video camera
    cv::VideoCapture video(0);

    // if not success, exit program
    if (video.isOpened() == false)  {
        std::cout << "Cannot open the video camera\n";
        std::cin.get(); //wait for any key press
        return -1;
    } 
    
    std::string image_file = "";
    bool show_windows = true;

    std::map<std::string, std::string> arguments;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);

        if (arg.find("--") == 0) {
            size_t equal_sign_pos = arg.find("=");
            std::string key = arg.substr(0, equal_sign_pos);
            std::string value = equal_sign_pos != std::string::npos ? arg.substr(equal_sign_pos + 1) : "";

            arguments[key] = value;
        }
    }

    if (arguments.count("--model")) {
        model_file = arguments["--model"];
    }

    /*if (arguments.count("--video")) {
        video_file = arguments["--video"];
    }*/

    if (arguments.count("--image")) {
        image_file = arguments["--image"];
    }

    if (arguments.count("--no-windows")) {
        show_windows = false;
    }

    auto model = tflite::FlatBufferModel::BuildFromFile(model_file.c_str());

    if (!model) {
        throw std::runtime_error("Failed to load TFLite model");
    }

    tflite::ops::builtin::BuiltinOpResolver op_resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::InterpreterBuilder(*model, op_resolver)(&interpreter);

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        throw std::runtime_error("Failed to allocate tensors");
    }

    tflite::PrintInterpreterState(interpreter.get());

    auto input = interpreter->inputs()[0];
    auto input_batch_size = interpreter->tensor(input)->dims->data[0];
    auto input_height = interpreter->tensor(input)->dims->data[1];
    auto input_width = interpreter->tensor(input)->dims->data[2];
    auto input_channels = interpreter->tensor(input)->dims->data[3];

    std::cout << "The input tensor has the following dimensions: ["<< input_batch_size << "," 
                                                                   << input_height << "," 
                                                                   << input_width << ","
                                                                   << input_channels << "]" << std::endl;

    auto output = interpreter->outputs()[0];

    auto dim0 = interpreter->tensor(output)->dims->data[0];
    auto dim1 = interpreter->tensor(output)->dims->data[1];
    auto dim2 = interpreter->tensor(output)->dims->data[2];
    auto dim3 = interpreter->tensor(output)->dims->data[3];
    std::cout << "The output tensor has the following dimensions: ["<< dim0 << "," 
                                                                    << dim1 << "," 
                                                                    << dim2 << ","
                                                                    << dim3 << "]" << std::endl;


    //cv::VideoCapture video(video_file);

    cv::Mat frame;

    if (image_file.empty()) {
        if (!video.isOpened()) {
            std::cout << "Can't open the video: " << std::endl;
            return -1;
        }
    }
    else {
        frame = cv::imread(image_file);
    }

    std::vector<Ball> balls;
    balls.push_back(Ball());
    Ball b;
    b.x = 3;
    balls.push_back(b);
    float hand_rad = 0.15;
    int points = 0;

    while (true) {

        if (image_file.empty()) {
            video >> frame;

            if (frame.empty()) {
                video.set(cv::CAP_PROP_POS_FRAMES, 0);
                continue;
            }
        }
        
        int image_width = frame.size().width;
        int image_height = frame.size().height;

        int square_dim = std::min(image_width, image_height);
        int delta_height = (image_height - square_dim) / 2;
        int delta_width = (image_width - square_dim) / 2;

        cv::Mat resized_image;

        // crop + resize the input image
        cv::resize(frame(cv::Rect(delta_width, delta_height, square_dim, square_dim)), resized_image, cv::Size(input_width, input_height));

        memcpy(interpreter->typed_input_tensor<unsigned char>(0), resized_image.data, resized_image.total() * resized_image.elemSize());

        // inference
        std::chrono::steady_clock::time_point start, end;
        start = std::chrono::steady_clock::now();
        if (interpreter->Invoke() != kTfLiteOk) {
            std::cerr << "Inference failed" << std::endl;
            return -1;
        }
        end = std::chrono::steady_clock::now();
        auto processing_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        std::cout << "processing time: " << processing_time << " ms" << std::endl;

        float *results = interpreter->typed_output_tensor<float>(0);

        // move ball
        float t = processing_time * 2/ 1000.0;
        for(auto& b: balls)
            b.move(t);
        int offset = 75;
        for (int index1=0; index1<17; index1++){
            float y1 = results[index1 * 3];
            float x1 = results[index1 * 3 + 1];
            float conf1 = results[index1 * 3 + 2];
            if (conf1 > confidence_threshold) {
                int img_x1 = offset+static_cast<int>(x1 * square_dim);
                int img_y1 = static_cast<int>(y1 * square_dim);
                if (index1 == 0 || index1 == 9 || index1 == 10){
                    cv::circle(frame, cv::Point(img_x1, img_y1), hand_rad * scale, cv::Scalar(255, 255, 0), -1);
                    float real_x = img_x1/scale;
                    float real_y = img_y1/scale;
                    std::vector<Ball> new_balls;
                    for(auto& b: balls){
                        if ((real_x - b.x) * (real_x - b.x) + (real_y - b.y) * (real_y - b.y) < 
                        (b.rad + hand_rad) * (b.rad + hand_rad)) {
                            if (index1 == 0)
                                points = 0;
                            else {
                                if (b.rad > 0.2){
                                    points += 10;
                                    Ball newb;
                                    newb.r = b.r; newb.g = b.g; newb.b= b.b;
                                    newb.y = b.y;
                                    newb.x = b.x - b.rad/2;
                                    newb.vx = -3;
                                    newb.vy = 5;
                                    newb.rad = b.rad / 2;
                                    new_balls.push_back(newb);
                                    Ball newb2;
                                    newb2.r = b.r; newb2.g = b.g; newb2.b= b.b;
                                    newb2.y = b.y;
                                    newb2.x = b.x + b.rad/2;
                                    newb2.vx = 3;
                                    newb2.vy = 5;
                                    newb2.rad = b.rad / 2;
                                    new_balls.push_back(newb2);
                                }
                            }
                        }
                        else
                            new_balls.push_back(b);
                    }
                    balls = new_balls;
                    if (balls.size() < 2){
                        balls.push_back(Ball());
                    } 
                }
            }
        }
        for(auto& b: balls)
            b.limits();
        
        cv::putText(frame, std::string("POINTS:") + std::to_string(points), cv::Point(20, 45), cv::FONT_HERSHEY_SIMPLEX, 2,
                    cv::Scalar(0,0, 255), 3, cv::LINE_AA);
        draw_keypoints(frame, results);
        for(auto& b: balls)
            b.draw(frame);

        if (show_windows) {
            imshow("Output", frame);
        }
        else {
            // just run one frame when not showing output
            break;
        }

        // render at 30 fps
        int waitTime = processing_time<33 ? 33-processing_time : 1;
        int k = cv::waitKey(33-processing_time);
        if (k >= 0) {
            break;
        }
    }

    if (image_file.empty()) {
        video.release();
    }

    if (show_windows) {
            cv::destroyAllWindows();
    }

    return 0;
}
