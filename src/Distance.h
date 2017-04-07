//
//  DTW.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 1/31/17.
//

#ifndef DTW_h
#define DTW_h

#include <cassert>
#include <limits>
#include "Common.h"
#include "AudioFeatures.h"
#include "libs/ThreadPool.h"
#include "AudioObject.h"

using namespace std;
using namespace Eigen;

namespace Distance
{
/**
 *  Calculate the distance between two AudioObjects.
 *
 *  @param a              The first AudioObject to be compared
 *  @param b              The second AudioObject to be compared
 *  @param dtw_constraint The DTW locality constraint. Default=1
 *  @param num_processors Number of processor cores to use. Default-1
 *
 *  @return A vector containing distances along each feature axis
 */
vector<double> distance (const AudioObject &a, const AudioObject &b, double dtw_constraint = 1, int num_processors = 1);

/**
 *  Calculate dynamic time warping distance on two time series with a locality constraint
 *
 *  @param s The first time series
 *  @param t The second time series
 *  @param w The locality constraint
 *
 *  @return The DTW distance between the two time series
 */
double DTW (const vector<double> &s, const vector<double> &t, int w);

/**
 *  Calculate dynamic time warping distance on two time series
 *
 *  @param s The first time series
 *  @param t The second time series
 *
 *  @return The DTW distance between the two time series
 */
double DTW (const vector<double> &s, const vector<double> &t);

double weightedPNorm (const vector<double> &dists, const vector<double> &weights, int p = 2);
}

#endif /* DTW_h */
