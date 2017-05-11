//
//  Distance.cpp
//  AudioSearchCLI
//
//  Created by Michael Donovan on 3/27/17.
//

#include "Distance.h"

vector<double> Distance::distance (const AudioObject &a, const AudioObject &b, int num_processors, bool local_alignment,
                                   double dtw_constraint)
{
    ThreadPool thread_pool (num_processors);

    vector<future<pair<int, double>>> results;

    for (int j = 0; j < AudioFeatures::num_features; ++j)
    {
        auto a_timeseries = a.dtwDataType ()[j];
        auto b_timeseries = b.dtwDataType ()[j];
        int constraint = a_timeseries.size () * dtw_constraint;

        if (local_alignment)
        {
            results.emplace_back (
                thread_pool.enqueue ([](vector<double> &s, vector<double> &t, int w,
                                        int j) { return pair<int, double> (j, Distance::smith_waterman (s, t)); },
                                     a_timeseries, b_timeseries, constraint, j));
        }
        else
        {
            results.emplace_back (
                thread_pool.enqueue ([](vector<double> &s, vector<double> &t, int w,
                                        int j) { return pair<int, double> (j, Distance::DTW (s, t, w)); },
                                     a_timeseries, b_timeseries, constraint, j));
        }
    }

    // distances along each feature axis
    vector<double> distances (AudioFeatures::num_features);
    // get results when ready
    for (auto &&result : results)
    {
        result.wait ();
        auto r = result.get ();
        distances[r.first] = log (r.second);

    }
    return distances;
}

double Distance::DTW (const vector<double> &s, const vector<double> &t, int w)
{
    double cost;
    double dbl_max = numeric_limits<double>::max ();
    int n = s.size ();
    int m = t.size ();

    MatrixXd dtw (n + 1, m + 1);

    w = max (w, abs (n - m));

    for (int i = 0; i <= n; ++i) dtw (i, 0) = dbl_max;
    for (int i = 0; i <= m; ++i) dtw (0, i) = dbl_max;
    dtw (0, 0) = 0;

    for (int i = 1; i <= n; ++i)
    {
        for (int j = max (1, i - w); j <= min (m, i + w); ++j)
        {
            cost = abs (s[i - 1] - t[j - 1]);
            dtw (i, j) = cost + min  (min (dtw (i - 1, j), dtw (i, j - 1)), dtw (i - 1, j - 1));
        }
    }

    return abs (dtw (n, m));
}

double Distance::DTW (const vector<double> &s, const vector<double> &t)
{
    double cost;
    double dbl_max = numeric_limits<double>::max ();
    size_t n = s.size ();
    size_t m = t.size ();

    MatrixXd dtw (n + 1, m + 1);

    for (int i = 1; i <= n; ++i) dtw (i, 0) = dbl_max;
    for (int i = 1; i <= m; ++i) dtw (0, i) = dbl_max;
    dtw (0, 0) = 0;

    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= m; ++j)
        {
            cost = abs (s[i - 1] - t[j - 1]);
            dtw (i, j) = cost + min (min (dtw (i - 1, j), dtw (i, j - 1)), dtw (i - 1, j - 1));
        }
    }
    return abs (dtw (n, m));
}

double Distance::weightedPNorm (const vector<double> &dists, const vector<double> &weights, int p)
{
    if (weights.size () != dists.size ()) throw invalid_argument ("length of weights and dists must be equal");

    double sum = 0;
    for (int i = 0; i < dists.size (); ++i)
    {
        sum += weights[i] * pow (dists[i], p);
    }

    return pow (sum, 1 / (double) p);
}

template <typename T>
bool within_tolerance(T a, T b, double tolerance) {
    double lower = a * (1 - tolerance);
    double upper = a * (1 + tolerance);

    return b >= lower && b <= upper;
}

double Distance::smith_waterman (const vector<double> &s, const vector<double> &t, double tolerance, vector<double> penalties)
{
    MatrixXd scoring (s.size (), t.size ());
    for (int i = 0; i < s.size (); ++i) scoring (i, 0) = 0;
    for (int i = 0; i < t.size (); ++i) scoring (0, i) = 0;

    double best = 0;

    for (int i = 1; i < s.size (); ++i)
    {
        for (int j = 1; j < t.size (); ++j)
        {
            double match_mismatch = within_tolerance (s[i], t[j], tolerance) ? penalties[0] : penalties[1];
            scoring (i, j) = std::max (std::max (scoring (i, j - 1) + penalties[2], scoring (i - 1, j) + penalties[1]),
                                       std::max (scoring (i - 1, j - 1) + match_mismatch, 0.));

            if (scoring (i, j) >= best) best = scoring (i, j);
        }
    }
    return best;
}


