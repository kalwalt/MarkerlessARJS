class CameraCalibration {
  constructor(_fx, _fy, _cx, _cy) {
    //m_intrinsic = cv::Matx33f::zeros();
    this.m_intrinsic = new cv.Matx33f();
    this.m_distortion = new cv.Mat();

    fx() = _fx;
    fy() = _fy;
    cx() = _cx;
    cy() = _cy;

      this.m_distortion.create(5,1);
    for (let i=0; i<5; i++)
        tbis.m_distortion(i) = 0;
  };

  fx(){
    return this.m_intrinsic(1,1);
  };

  fy(){
    return this.m_intrinsic(0,0);
  };

  cx(){
    return this.m_intrinsic(0,2);
  };

  cy(){
    return this.m_intrinsic(1,2);
  };
};
