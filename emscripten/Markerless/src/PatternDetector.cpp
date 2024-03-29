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
#include "PatternDetector.hpp"

// Standard includes:
#include <cmath>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <cassert>

PatternDetector::PatternDetector(cv::Ptr<cv::FeatureDetector> detector,
    cv::Ptr<cv::DescriptorExtractor> extractor,
    cv::Ptr<cv::BFMatcher> matcher,
    bool ratioTest)
    : m_detector(detector)
    , m_extractor(extractor)
    , m_matcher(matcher)
    , enableRatioTest(ratioTest)
    , enableHomographyRefinement(true)
    , homographyReprojectionThreshold(3)
{
}

void PatternDetector::train(const Pattern& pattern)
{
    // Store the pattern object
    m_pattern = pattern;

    // API of cv::DescriptorMatcher is somewhat tricky
    // First we clear old train data:
    m_matcher->clear();

    // Then we add vector of descriptors (each descriptors matrix describe one image).
    // This allows us to perform search across multiple images:
    std::vector<cv::Mat> descriptors(1);
    descriptors[0] = pattern.descriptors.clone();
    m_matcher->add(descriptors);

    // After adding train data perform actual train:
    m_matcher->train();
}

void PatternDetector::buildPatternFromImage(const cv::Mat& image, Pattern& pattern) const
{
    int numImages = 4;
    float step = sqrtf(2.0f);

    // Store original image in pattern structure
    pattern.size = cv::Size(image.cols, image.rows);
    pattern.frame = image.clone();
    getGray(image, pattern.grayImg);

    // Build 2d and 3d contours (3d contour lie in XY plane since it's planar)
    pattern.points2d.resize(4);
    pattern.points3d.resize(4);

    // Image dimensions
    const float w = image.cols;
    const float h = image.rows;

    // Normalized dimensions:
    const float maxSize = std::max(w,h);
    const float unitW = w / maxSize;
    const float unitH = h / maxSize;

    pattern.points2d[0] = cv::Point2f(0,0);
    pattern.points2d[1] = cv::Point2f(w,0);
    pattern.points2d[2] = cv::Point2f(w,h);
    pattern.points2d[3] = cv::Point2f(0,h);

    pattern.points3d[0] = cv::Point3f(-unitW, -unitH, 0);
    pattern.points3d[1] = cv::Point3f( unitW, -unitH, 0);
    pattern.points3d[2] = cv::Point3f( unitW,  unitH, 0);
    pattern.points3d[3] = cv::Point3f(-unitW,  unitH, 0);

    extractFeatures(pattern.grayImg, pattern.keypoints, pattern.descriptors);
}

bool PatternDetector::findPattern(const cv::Mat& image, PatternTrackingInfo& info)
{
    // Convert input image to gray
    getGray(image, m_grayImg);

    // Extract feature points from input gray image
    extractFeatures(m_grayImg, m_queryKeypoints, m_queryDescriptors);

    // Get matches with current pattern
    getMatches(m_queryDescriptors, m_matches);
    
    // uncomment next line to debug
    std::cout << "Num Matches: " << m_matches.size() << std::endl;

    // Find homography transformation and detect good matches
    bool homographyFound = refineMatchesWithHomography(
        m_queryKeypoints,
        m_pattern.keypoints,
        homographyReprojectionThreshold,
        m_matches,
        m_roughHomography);

    // uncomment next line to debug 
    std::cout << "Homography found: " << (bool)homographyFound << std::endl;

    if (homographyFound)
    {
        // If homography refinement enabled improve found transformation
        if (enableHomographyRefinement)
        {
            // Warp image using found homography
            cv::warpPerspective(m_grayImg, m_warpedImg, m_roughHomography, m_pattern.size, cv::WARP_INVERSE_MAP | cv::INTER_CUBIC);

            // Get refined matches:
            std::vector<cv::KeyPoint> warpedKeypoints;
            std::vector<cv::DMatch> refinedMatches;

            // Detect features on warped image
            extractFeatures(m_warpedImg, warpedKeypoints, m_queryDescriptors);

            // Match with pattern
            getMatches(m_queryDescriptors, refinedMatches);

            std::cout << "Num Matches(refined): " << refinedMatches.size() << std::endl;

            // Estimate new refinement homography
            homographyFound = refineMatchesWithHomography(
                warpedKeypoints,
                m_pattern.keypoints,
                homographyReprojectionThreshold,
                refinedMatches,
                m_refinedHomography);
            std::cout << "Homography found(refined): " << (bool)homographyFound << std::endl;

            // Get a result homography as result of matrix product of refined and rough homographies:
            info.homography = m_roughHomography * m_refinedHomography;

            // Transform contour with precise homography
            cv::perspectiveTransform(m_pattern.points2d, info.points2d, info.homography);

        }
        else
        {
            info.homography = m_roughHomography;

            // Transform contour with rough homography
            cv::perspectiveTransform(m_pattern.points2d, info.points2d, m_roughHomography);
        }
    }

    return homographyFound;
}

cv::Mat PatternDetector::getHomography() const {
    return m_refinedHomography;
}

void PatternDetector::getGray(const cv::Mat& image, cv::Mat& gray)
{
    if (image.channels()  == 3)
        //cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY); 
        cv::cvtColor(image, gray, cv::COLOR_RGB2GRAY);
    else if (image.channels() == 4)
        //cv::cvtColor(image, gray, cv::COLOR_BGRA2GRAY);
        cv::cvtColor(image, gray, cv::COLOR_RGBA2GRAY);
    else if (image.channels() == 1)
        gray = image;
}

bool PatternDetector::extractFeatures(const cv::Mat& image, std::vector<cv::KeyPoint>& keypoints, cv::Mat& descriptors) const
{
    assert(!image.empty());
    assert(image.channels() == 1);

    m_detector->detect(image, keypoints);
    // uncomment next line to debug
    // std::cout << "Keypoints num: " <<  keypoints.size() << std::endl;
    if (keypoints.empty())
        return false;

    m_extractor->compute(image, keypoints, descriptors);
    if (keypoints.empty())
        return false;

    return true;
}

void PatternDetector::getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches)
{
    matches.clear();

    if (enableRatioTest)
    {
        // To avoid NaN's when best match has zero distance we will use inversed ratio.
        const float minRatio = 1.f / 1.5f;

        // KNN match will return 2 nearest matches for each query descriptor
        m_matcher->knnMatch(queryDescriptors, m_knnMatches, 2);

        for (size_t i=0; i<m_knnMatches.size(); i++)
        {
            const cv::DMatch& bestMatch   = m_knnMatches[i][0];
            const cv::DMatch& betterMatch = m_knnMatches[i][1];

            float distanceRatio = bestMatch.distance / betterMatch.distance;

            // Pass only matches where distance ratio between
            // nearest matches is greater than 1.5 (distinct criteria)
            if (distanceRatio < minRatio)
            {
                matches.push_back(bestMatch);
            }
        }
    }
    else
    {
        // Perform regular match
        m_matcher->match(queryDescriptors, matches);
    }
}

bool PatternDetector::refineMatchesWithHomography
    (
    const std::vector<cv::KeyPoint>& queryKeypoints,
    const std::vector<cv::KeyPoint>& trainKeypoints,
    float reprojectionThreshold,
    std::vector<cv::DMatch>& matches,
    cv::Mat& homography
    )
{
    const int minNumberMatchesAllowed = 8;

    // uncomment next line to debug
    // std::cout << "Number of matches inside refineatchesWithHomography: " << matches.size() << std::endl;

    if (matches.size() < minNumberMatchesAllowed)
        return false;

    // Prepare data for cv::findHomography
    std::vector<cv::Point2f> srcPoints(matches.size());
    std::vector<cv::Point2f> dstPoints(matches.size());

    for (size_t i = 0; i < matches.size(); i++)
    {
        srcPoints[i] = trainKeypoints[matches[i].trainIdx].pt;
        dstPoints[i] = queryKeypoints[matches[i].queryIdx].pt;
    }

    // Find homography matrix and get inliers mask
    // uncomment next line to debug
    // std::cout << "SrcPoints size: " << srcPoints.size() << std::endl;
    std::vector<unsigned char> inliersMask(srcPoints.size());
    homography = cv::findHomography(srcPoints,
                                    dstPoints,
                                    cv::RANSAC,
                                    reprojectionThreshold,
                                    inliersMask);

    std::vector<cv::DMatch> inliers;
    for (size_t i=0; i<inliersMask.size(); i++)
    {
        if (inliersMask[i])
            inliers.push_back(matches[i]);
    }
    // uncomment next line to debug
    // std::cout << "Num inliers: " << inliers.size() << std::endl;
    matches.swap(inliers);
    return matches.size() > minNumberMatchesAllowed;
}
