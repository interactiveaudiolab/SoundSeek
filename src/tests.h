//
//  tests.h
//  AudioSearchCLI
//
//  Created by Michael on 5/4/17.
//

#ifndef tests_h
#define tests_h

#include "DeepSimilarity.h"
#include "Distance.h"

namespace Test {
template <typename T1, typename T2>
void assert_eq(T1 a, T2 b, std::string msg = "")
{
    if (a != b)
        cerr << "Test failed: " << msg << "\n->\t" << a << " != " << b << endl << endl;
}

void test_dtw()
{
    vector<double> a = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    vector<double> b = { 5, 7, 2, 4, 5 };
    double expected = 20.;
    assert_eq(Distance::DTW(a, b), expected);
    assert_eq(Distance::DTW(a, b, 1), expected);
    assert_eq(Distance::DTW(a, b, .5), expected);
    assert_eq(Distance::DTW(a, b, .25), expected);
    assert_eq(Distance::DTW(a, b, .1), expected);
}

void big_dtw()
{
    vector<double> a = { 1, 3, 5, 6, 2, 5, 3, 7, 8, 2, 4, 6, 7, 2,
                         7, 8, 4, 9, 2, 1, 8, 3, 5, 1, 5, 0, 4, 1 };
    vector<double> b = { 2, 1, 7, 7, 8, 1, 4, 5, 7, 2, 7, 8, 9, 5,
                         5, 2, 4, 6, 2, 1, 8, 3, 5, 1, 5, 1, 4, 2 };
    double expected = 28;
    assert_eq(Distance::DTW(a, b), expected);
    assert_eq(Distance::DTW(a, b, 1), expected);
    assert_eq(Distance::DTW(a, b, .5), expected);
    assert_eq(Distance::DTW(a, b, .25), expected);
    assert_eq(Distance::DTW(a, b, .1), expected);
}

void test_euclidean()
{
    vector<double> dists = { 2., 4., 4. };
    vector<double> weights = { 1., 1., 1. };
    assert_eq(Distance::weightedPNorm(dists, weights), 6);

    weights = { 1., 0., 0. };
    assert_eq(Distance::weightedPNorm(dists, weights), 2);
}

void test_manhattan()
{
    vector<double> dists = { 2., 4., 4. };
    vector<double> weights = { 1., 1., 1. };
    assert_eq(Distance::weightedPNorm(dists, weights, 1), 10);

    weights = { 1., 0., 0. };
    assert_eq(Distance::weightedPNorm(dists, weights, 1), 2);
}

void self_dtw()
{
    vector<double> a = { 1, 2, 3, 4, 5 };
    assert_eq(Distance::DTW(a, a), 0., "DTW: self v. self = 0");
}

void self_local()
{
    vector<double> a = { 1, 2, 3, 4, 5 };
    assert_eq(Distance::smithWaterman(a, a), 0., "local alignment: self v. self = 0");
}

void test_siamese()
{
    //    double result = Distance::deepSimilarity(
    auto net = Distance::CaffeNetSimilarity(PROTOTXT, CAFFEMODEL);

    double result =
      1. -
      net.similarity(path("/Users/michael/InteractiveAudioLab/SoundSeek/Testing/query.wav"),
                     path("/Users/michael/InteractiveAudioLab/SoundSeek/Testing/test1.wav"));
    DBG("Siamese: " << result);
}

void run_tests()
{
    test_dtw();
    test_euclidean();
    self_dtw();
    self_local();
    test_siamese();
}
}

#endif /* tests_h */
