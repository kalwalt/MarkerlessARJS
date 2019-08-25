/*
---------------------------------------------------------------------
--- Author         : Ahmet Özlü
--- Mail           : ahmetozlu93@gmail.com
--- Date           : 1st August 2017
--- Version        : 1.0
--- OpenCV Version : 2.4.10
--- Demo Video     : https://www.youtube.com/watch?v=nPfR5ACrqu0
---------------------------------------------------------------------
*/

// File includes:
#include <windows.h>
#include "CameraCalibration.hpp"

CameraCalibration::CameraCalibration()
{
}

CameraCalibration::CameraCalibration(float _fx, float _fy, float _cx, float _cy)
{
    m_intrinsic = cv::Matx33f::zeros();

    fx() = _fx;
    fy() = _fy;
    cx() = _cx;
    cy() = _cy;

    m_distortion.create(5,1);
    for (int i=0; i<5; i++)
        m_distortion(i) = 0;
}

CameraCalibration::CameraCalibration(float _fx, float _fy, float _cx, float _cy, float distorsionCoeff[5])
{
    m_intrinsic = cv::Matx33f::zeros();

    fx() = _fx;
    fy() = _fy;
    cx() = _cx;
    cy() = _cy;

    m_distortion.create(5,1);
    for (int i=0; i<5; i++)
        m_distortion(i) = distorsionCoeff[i];
}

const cv::Matx33f& CameraCalibration::getIntrinsic() const
{
    return m_intrinsic;
}

const cv::Mat_<float>&  CameraCalibration::getDistorsion() const
{
    return m_distortion;
}

float& CameraCalibration::fx()
{
    return m_intrinsic(1,1);
}

float& CameraCalibration::fy()
{
    return m_intrinsic(0,0);
}

float& CameraCalibration::cx()
{
    return m_intrinsic(0,2);
}

float& CameraCalibration::cy()
{
    return m_intrinsic(1,2);
}

float CameraCalibration::fx() const
{
    return m_intrinsic(1,1);
}

float CameraCalibration::fy() const
{
    return m_intrinsic(0,0);
}

float CameraCalibration::cx() const
{
    return m_intrinsic(0,2);
}

float CameraCalibration::cy() const
{
    return m_intrinsic(1,2);
}