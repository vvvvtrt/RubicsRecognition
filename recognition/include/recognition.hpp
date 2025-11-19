#ifndef RECOGNITION_HPP
#define RECOGNITION_HPP

#include <classifier/classifier.hpp>
#include <detector/detector.hpp>
#include <utils/circut.hpp>
#include <preprocessor/preprocessor.hpp>

namespace recognition {
    using ContourDetector = ::ContourDetector;
    using Circuit  = ::Circuit;
    using KMeansClassifier = ::KMeansClassifier;
    using ContourDetector = ::ContourDetector;
    using SquareSearcher9 = ::SquareSearcher9;
    using SquareSearcher = ::SquareSearcher;
    using DilateProcessor = ::DilateProcessor;
}

#endif // RECOGNITION_HPP