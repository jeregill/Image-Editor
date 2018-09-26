#include "stats.h"

using namespace std;



// initialize the private vectors so that, for each color,  entry 
// (x,y) is the cumulative sum of the the color values from (0,0)
// to (x,y). Similarly, the sumSq vectors are the cumulative
// sum of squares from (0,0) to (x,y).
stats::stats(PNG & im) {
	// first, sum down the rows
	for (unsigned int k = 0; k < im.height(); k++) {
		vector<long> rowRed;
		vector<long> rowGreen;
		vector<long> rowBlue;
		long redRow = 0;
		long greenRow = 0;
		long blueRow = 0;
		vector<long> rowRedSq;
		vector<long> rowGreenSq;
		vector<long> rowBlueSq;
		long redRowSq = 0;
		long greenRowSq = 0;
		long blueRowSq = 0;
		for (unsigned int l = 0; l < im.width(); l++) {
			RGBAPixel* curr = im.getPixel(l, k);
			redRow   += (long)curr->r;
			greenRow += (long)curr->g;
			blueRow  += (long)curr->b;
			rowRed.push_back(redRow);
			rowGreen.push_back(greenRow);
			rowBlue.push_back(blueRow);
			redRowSq += (long)(curr->r * curr->r);
			greenRowSq += (long)(curr->g * curr->g);
			blueRowSq += (long)(curr->b * curr->b);
			rowRedSq.push_back(redRowSq);
			rowGreenSq.push_back(greenRowSq);
			rowBlueSq.push_back(blueRowSq);
		}
		sumRed.push_back(rowRed);
		sumGreen.push_back(rowGreen);
		sumBlue.push_back(rowBlue);
		sumsqRed.push_back(rowRedSq);
		sumsqGreen.push_back(rowGreenSq);
		sumsqBlue.push_back(rowBlueSq);
	}


	// next using our summed rows, sum them down by column
	for (unsigned int j = 0; j < im.width(); j++) {
		for (unsigned int i = 1; i < im.height(); i++) {	
			sumRed[i][j] += sumRed[i - 1][j];
			sumGreen[i][j] += sumGreen[i - 1][j];
			sumBlue[i][j] += sumBlue[i - 1][j];
			sumsqRed[i][j] += sumsqRed[i - 1][j];
			sumsqGreen[i][j] += sumsqGreen[i - 1][j];
			sumsqBlue[i][j] += sumsqBlue[i - 1][j];
		}
	}
}


// given a rectangle, return the number of pixels in the rectangle
/* @param ul is (x,y) of the upper left corner of the rectangle
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats::rectArea(pair<int, int> ul, pair<int, int> lr) {
	long h = lr.second + 1 - ul.second;
	long w = lr.first + 1 - ul.first;
	return h * w;
}

/* returns the sums of all pixel values across given color channels.
* useful in computing the score of a rectangle
* PA3 function
* @param channel is one of r, g, or b
* @param ul is (x,y) of the upper left corner of the rectangle
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats:: getSum(char channel, pair<int, int> ul, pair<int, int> lr) {
	switch (channel) {
		long lefthalf;
		long upperhalf;
		long overlap;
		long total;
		case 'r':
			if ((ul.first == 0 && ul.second == 0))
				return sumRed[lr.second][lr.first];
			else if (ul.first == 0 && ul.second != 0) {
				upperhalf = sumRed[ul.second - 1][lr.first];
				total = sumRed[lr.second][lr.first];
				return total - upperhalf;
			}
			else if (ul.first != 0 && ul.second == 0) {
				lefthalf = sumRed[lr.second][ul.first - 1];
				total = sumRed[lr.second][lr.first];
				return total - lefthalf;
			} else {
				total = sumRed[lr.second][lr.first];
				lefthalf = sumRed[lr.second][ul.first - 1];
				upperhalf = sumRed[ul.second - 1][lr.first];
				overlap = sumRed[ul.second - 1][ul.first - 1];
				return total - lefthalf - upperhalf + overlap;
			}
		case 'g':
			if ((ul.first == 0 && ul.second == 0))
				return sumGreen[lr.second][lr.first];
			else if (ul.first == 0 && ul.second != 0) {
				upperhalf = sumGreen[ul.second - 1][lr.first];
				total = sumGreen[lr.second][lr.first];
				return total - upperhalf;
			}
			else if (ul.first != 0 && ul.second == 0) {
				lefthalf = sumGreen[lr.second][ul.first - 1];
				total = sumGreen[lr.second][lr.first];
				return total - lefthalf;
			}
			else {
				total = sumGreen[lr.second][lr.first];
				lefthalf = sumGreen[lr.second][ul.first - 1];
				upperhalf = sumGreen[ul.second - 1][lr.first];
				overlap = sumGreen[ul.second - 1][ul.first - 1];
				return total - lefthalf - upperhalf + overlap;
			}
		case 'b':
			if ((ul.first == 0 && ul.second == 0))
				return sumBlue[lr.second][lr.first];
			else if (ul.first == 0 && ul.second != 0) {
				upperhalf = sumBlue[ul.second - 1][lr.first];
				total = sumBlue[lr.second][lr.first];
				return total - upperhalf;
			}
			else if (ul.first != 0 && ul.second == 0) {
				lefthalf = sumBlue[lr.second][ul.first - 1];
				total = sumBlue[lr.second][lr.first];
				return total - lefthalf;
			}
			else {
				total = sumBlue[lr.second][lr.first];
				lefthalf = sumBlue[lr.second][ul.first - 1];
				upperhalf = sumBlue[ul.second - 1][lr.first];
				overlap = sumBlue[ul.second - 1][ul.first - 1];
				return total - lefthalf - upperhalf + overlap;
			}

		default:
			// should never get here
			return 0;
				
	}
	
}

/* returns the sums of squares of all pixel values across all color channels.
* useful in computing the score of a rectangle
* PA3 function
* @param channel is one of r, g, or b
* @param ul is (x,y) of the upper left corner of the rectangle
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats::getSumSq(char channel, pair<int, int> ul, pair<int, int> lr) {
	switch (channel) {
		long lefthalf;
		long upperhalf;
		long overlap;
		long total;
	case 'r':
		if ((ul.first == 0 && ul.second == 0))
			return sumsqRed[lr.second][lr.first];
		else if (ul.first == 0 && ul.second != 0) {
			upperhalf = sumsqRed[ul.second - 1][lr.first];
			total = sumsqRed[lr.second][lr.first];
			return total - upperhalf;
		}
		else if (ul.first != 0 && ul.second == 0) {
			lefthalf = sumsqRed[lr.second][ul.first - 1];
			total = sumsqRed[lr.second][lr.first];
			return total - lefthalf;
		}
		else {
			total = sumsqRed[lr.second][lr.first];
			lefthalf = sumsqRed[lr.second][ul.first - 1];
			upperhalf = sumsqRed[ul.second - 1][lr.first];
			overlap = sumsqRed[ul.second - 1][ul.first - 1];
			return total - lefthalf - upperhalf + overlap;
		}
	case 'g':
		if ((ul.first == 0 && ul.second == 0))
			return sumsqGreen[lr.second][lr.first];
		else if (ul.first == 0 && ul.second != 0) {
			upperhalf = sumsqGreen[ul.second - 1][lr.first];
			total = sumsqGreen[lr.second][lr.first];
			return total - upperhalf;
		}
		else if (ul.first != 0 && ul.second == 0) {
			lefthalf = sumsqGreen[lr.second][ul.first - 1];
			total = sumsqGreen[lr.second][lr.first];
			return total - lefthalf;
		}
		else {
			total = sumsqGreen[lr.second][lr.first];
			lefthalf = sumsqGreen[lr.second][ul.first - 1];
			upperhalf = sumsqGreen[ul.second - 1][lr.first];
			overlap = sumsqGreen[ul.second - 1][ul.first - 1];
			return total - lefthalf - upperhalf + overlap;
		}
	case 'b':
		if ((ul.first == 0 && ul.second == 0))
			return sumsqBlue[lr.second][lr.first];
		else if (ul.first == 0 && ul.second != 0) {
			upperhalf = sumsqBlue[ul.second - 1][lr.first];
			total = sumsqBlue[lr.second][lr.first];
			return total - upperhalf;
		}
		else if (ul.first != 0 && ul.second == 0) {
			lefthalf = sumsqBlue[lr.second][ul.first - 1];
			total = sumsqBlue[lr.second][lr.first];
			return total - lefthalf;
		}
		else {
			total = sumsqBlue[lr.second][lr.first];
			lefthalf = sumsqBlue[lr.second][ul.first - 1];
			upperhalf = sumsqBlue[ul.second - 1][lr.first];
			overlap = sumsqBlue[ul.second - 1][ul.first - 1];
			return total - lefthalf - upperhalf + overlap;
		}

	default:
		// should never get here
		return 0;

	}
}


// given a rectangle, compute its sum of squared deviations from 
// mean, over all color channels. Will be used to make split when
// building tree.
/* @param ul is (x,y) of the upper left corner of the rectangle
* @param lr is (x,y) of the lower right corner of the rectangle */
long stats::getScore(pair<int, int> ul, pair<int, int> lr) {
	long area = rectArea(ul, lr);
	//cout << "area is " << area << endl;
	//cout << ul.first << ", " << ul.second << " and " << lr.first << " , " << lr.second << endl;
	long redSq = getSumSq('r', ul, lr);
	long redSum = getSum('r', ul, lr);
	long redTotal = redSq - ((redSum * redSum) / area);
	long greenSq = getSumSq('g', ul, lr);
	long greenSum = getSum('g', ul, lr);
	long greenTotal = greenSq - ((greenSum * greenSum) / area);
	long blueSq = getSumSq('b', ul, lr);
	long blueSum = getSum('b', ul, lr);
	long blueTotal = blueSq - ((blueSum * blueSum) / area);

	return redTotal + greenTotal + blueTotal;
}

// given a rectangle, return the average color value over the 
// rectangle as a pixel.
/* Each color component of the pixel is the average value of that
* component over the rectangle.
* @param ul is (x,y) of the upper left corner of the rectangle
* @param lr is (x,y) of the lower right corner of the rectangle */
RGBAPixel stats::getAvg(pair<int, int> ul, pair<int, int> lr) {
	long area = rectArea(ul, lr);
	long redSum = getSum('r', ul, lr);
	long redAvg = redSum / area;
	long greenSum = getSum('g', ul, lr);
	long greenAvg = greenSum / area;
	long blueSum = getSum('b', ul, lr);
	long blueAvg = blueSum / area;

	return RGBAPixel(redAvg, greenAvg, blueAvg);
}

