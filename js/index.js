let utils = new Utils('errorMessage');
//CameraCalibration calibration(526.58037684199849f, 524.65577209994706f, 318.41744018680112f, 202.96659047014398f);
//let cameraCalibration = new CameraCalibration('526.58037684199849f', '524.65577209994706f', '318.41744018680112f', '202.96659047014398f');
function init(){
let video = document.getElementById('videoInput');
let cameraCalibration;
cv['onRuntimeInitialized']=()=>{
  cameraCalibration = new CameraCalibration('526.58037684199849f', '524.65577209994706f', '318.41744018680112f', '202.96659047014398f');
};
// Try to read the pattern:
//cv::Mat patternImage = cv::imread(argv[1]);
let patternImage = new cv.Mat(video.height, video.width, cv.CV_8UC4);
patternImage = cv.imread('pinball');
if (patternImage.empty())
  {
      console.log("Input image cannot be read");
      //return 2;
  }else if(patternImage){
    console.log("Image ok")
  }
  //cv::VideoCapture cap = cv::VideoCapture(0);
  let cap = new cv.VideoCapture(video);
  processVideo(patternImage, cameraCalibration, cap);

  function processVideo(patternImage, calibration, capture){

    // Grab first frame to get the frame dimensions
    //cv::Mat currentFrame;
    let currentFrame = new cv.Mat();
    capture >> currentFrame;

    // Check the capture succeeded:
    if (currentFrame.empty())
    {
        //std::cout << "Cannot open video capture device" << std::endl;
        console.log("Cannot open video capture device");
    }
    //cv::Size frameSize(currentFrame.cols, currentFrame.rows);
    let frameSize = cv.Size(currentFrame.cols, currentFrame.rows);

    //ARPipeline pipeline(patternImage, calibration);
    //ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

    let shouldQuit = false;
    do
    {
        capture >> currentFrame;
        if (currentFrame.empty())
        {
            shouldQuit = true;
            continue;
        }

        //shouldQuit = processFrame(currentFrame, pipeline, drawingCtx);
    } while (!shouldQuit);
  };
};

utils.loadOpenCv(() => {
init();
});
