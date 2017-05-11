//
//  tests.h
//  AudioSearchCLI
//
//  Created by Michael on 5/4/17.
//

#ifndef tests_h
#define tests_h

#include "Distance.h"

namespace Test {
    template<typename T1, typename T2>
    void assert_eq (T1 a, T2 b) {
        if (a != b) cerr << "Test failed: " << a << " != " << b << endl;
    }

    void test_dtw () {
        vector<double> a = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        vector<double> b = {5, 7, 2, 4, 5};
        double expected = 20.;
        assert_eq (Distance::DTW (a, b), expected);
        assert_eq (Distance::DTW (a, b, 1), expected);
        assert_eq (Distance::DTW (a, b, .5), expected);
        assert_eq (Distance::DTW (a, b, .25), expected);
        assert_eq (Distance::DTW (a, b, .1), expected);
    }

    void test_euclidean () {
        vector<double> dists = {2., 4., 4.};
        vector<double> weights = {1., 1., 1.};
        assert_eq (Distance::weightedPNorm (dists, weights), 6);

        weights = {1., 0., 0.};
        assert_eq (Distance::weightedPNorm (dists, weights), 2);
    }

    void test_manhattan () {
        vector<double> dists = {2., 4., 4.};
        vector<double> weights = {1., 1., 1.};
        assert_eq (Distance::weightedPNorm (dists, weights, 1), 10);

        weights = {1., 0., 0.};
        assert_eq (Distance::weightedPNorm (dists, weights, 1), 2);
    }

    void test_local () {
        vector<double> a = {14., 13.5, 12.};
        vector<double> b = {14.1, 8.5, 11.};
        auto result = Distance::smith_waterman (a, b);
        
    }

    void run_tests () {
        test_dtw ();
        test_euclidean ();
        test_local ();
    }
}

#endif /* tests_h */
