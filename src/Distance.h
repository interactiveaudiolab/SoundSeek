//
//  DTW.h
//  AudioSearchCLI
//
//  Created by Michael Donovan on 1/31/17.
//

#ifndef DTW_h
#define DTW_h

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
 *  @param a               The first AudioObject to be compared
 *  @param b               The second AudioObject to be compared
 *  @param num_processors  Number of processor cores to use. Default-1
 *  @param local_alignment If true, use local alignment (smith_waterman). Otherwise use global alignment (DTW)
 *  @param dtw_constraint  The DTW locality constraint. Default=1
 *
 *  @return A vector containing distances along each feature axis
 */
vector<double> distance (const AudioObject &a, const AudioObject &b, int num_processors = 1,
                         bool local_alignment = true, double dtw_constraint = 1);

/**
 *  Calculate dynamic time warping distance on two sequences with a locality constraint
 *
 *  @param s The first sequence
 *  @param t The second sequence
 *  @param w The locality constraint
 *
 *  @return The DTW distance between the two sequence
 */
double DTW (const vector<double> &s, const vector<double> &t, int w);

/**
 *  Calculate dynamic time warping distance on two sequences
 *
 *  @param s The first sequence
 *  @param t The second sequence
 *
 *  @return The DTW distance between the two sequence
 */
double DTW (const vector<double> &s, const vector<double> &t);


/**
 *  Calculate the score of the best subsequence alignment between two sequences
 *
 *  @param s The first sequence
 *  @param t The second Sequence
 *  @param penalties The costs/scores associated with a match, a mismatch, and a gap (in that order)
 *
 *  @return A scoring of the best subsequence alignment
 */
double smith_waterman (const vector<double> &s, const vector<double> &t, double tolerance = 0.05, vector<double> costs = {2, -1, -2});

double weightedPNorm (const vector<double> &dists, const vector<double> &weights, int p = 2);
}

#endif /* DTW_h */
