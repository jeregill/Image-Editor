
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"

/* given */
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, RGBAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),left(NULL),right(NULL)
	{}

/* given */
twoDtree::~twoDtree(){
	clear();
}

/* given */
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}

/* given */
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

twoDtree::twoDtree(PNG & imIn){
	stats obj(imIn);
	height = imIn.height();
	width = imIn.width();
	root = buildTree(obj, pair<int, int>(0, 0), pair<int, int>(width - 1, height - 1));	
}

twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr) {
	Node * newNode = new Node(ul, lr, s.getAvg(ul, lr));
	if (ul == lr)
		return newNode; // base case
	else {
		// initialize needed variables
		long min;
		bool vert;
		int split;

		// find the lowest score between the two rectangles
		if (ul.first == lr.first) {
			//in this case, we can only have a horizontal cut
			min = s.getScore(ul, ul) + s.getScore(pair<int, int>(ul.first, ul.second + 1), lr); // the first possible cut (single block and the rest)
			// now check every other cut
			for (int i = ul.second + 1; i <= lr.second; i++) {
				long score = s.getScore(pair<int, int>(ul.first, i), lr) + s.getScore(ul, pair<int, int>(lr.first, i - 1));
				if (score <= min) {
					min = score;
					split = i;
					vert = false;
				}
			}

			// recursively call on the new subtrees
			newNode->left = buildTree(s,ul, pair<int, int>(lr.first, split - 1));
			newNode->right = buildTree(s,pair<int, int>(ul.first, split), lr);

		}
		else if (ul.second == lr.second) {
			// in this case we have vertical cuts
			min = s.getScore(ul, ul) + s.getScore(pair<int, int>(ul.first + 1, ul.second), lr); // the first possible cut (single block and the rest)
			// now check every other cut
			for (int i = ul.first + 1; i <= lr.first; i++) {
				long score = s.getScore(pair<int, int>(i, ul.second), lr) + s.getScore(ul, pair<int, int>(i - 1, lr.second));
				if (score <= min) {
					min = score;
					split = i;
					vert = true;
				}
			}

			// recursively call on the new subtrees
			newNode->left = buildTree(s,ul, pair<int, int>(split - 1, ul.second));
			newNode->right = buildTree(s,pair<int, int>(split, ul.second), lr);
		}
		else {
			// case where could have vertical or horizontal splits

			min = s.getScore(ul, pair<int, int>(ul.first, lr.second)) + s.getScore(pair<int, int>(ul.first + 1, ul.second), lr); // first possible cut
			vert = true;
			// find the split location
			// first, check vertical cuts
			for (int i = ul.first+1; i <= lr.first; i++) {
				long score = s.getScore(pair<int, int>(i, ul.second), lr) + s.getScore(ul, pair<int, int>(i - 1, lr.second));
				if (score <= min) {
					min = score;
					split = i;
				}
			}
			// then check horizontal cuts
			for (int i = ul.second + 1; i <= lr.second; i++) {
				long score = s.getScore(pair<int, int>(ul.first, i), lr) + s.getScore(ul, pair<int, int>(lr.first, i - 1));
				if (score <= min) {
					min = score;
					split = i;
					vert = false;
				}
			}

			if (vert) {

				newNode->left = buildTree(s, ul, pair<int, int>(split - 1, lr.second));
				newNode->right = buildTree(s, pair<int, int>(split, ul.second), lr);

			}
			else {
				newNode->left = buildTree(s, ul, pair<int, int>(lr.first, split - 1));
				newNode->right = buildTree(s, pair<int, int>(ul.first, split), lr);
			}

		}
	}

	return newNode;
}

PNG twoDtree::render(){
	PNG photo(width, height);
	renderHelper(root, photo);
	return photo;
	
}

void twoDtree::renderHelper(Node* root, PNG & photo) {
	
	if (root == NULL) {
		return;
	}

	if (root->right == NULL && root->left == NULL) {
		// it is a leaf node, so iterate through all of its points from upper left to lower right and print them on the PNG
		for (int i = root->upLeft.first; i <= root->lowRight.first; i++) {
			for (int j = root->upLeft.second; j <= root->lowRight.second; j++) {
				RGBAPixel* curr = photo.getPixel(i, j);
				*curr = root->avg;
			}
		}
	}

	// recursively call on rest of the photo
	renderHelper(root->left,photo);
	renderHelper(root->right, photo);
}

void twoDtree::prune(double pct, int tol){
	pruneHelper(pct, tol, root);
}

void twoDtree::pruneHelper(double pct, int tol, Node* root) {
	if (root == NULL) {
		return;
	}
	double amount = (double) getTotalLeavesWithin(root, tol, root) / (double) getTotalLeaves(root);
	if (amount >= pct) {
		clearHelper(root->right);
		clearHelper(root->left);
		root->left = NULL;
		root->right = NULL;
		return;
	}
	else {
		pruneHelper(pct, tol, root->right);
		pruneHelper(pct, tol, root->left);
	}

}

int twoDtree::getTotalLeaves(Node* root) {
	if (root == NULL) 
		return 0;
	if (root->left == NULL && root->right == NULL)
		return 1;
	else
		return getTotalLeaves(root->left) + getTotalLeaves(root->right);

}

int twoDtree::getTotalLeavesWithin(Node* root, int tol, Node* thisNode) {
	if (root == NULL)
		return 0;
	if (thisNode->left == NULL && thisNode->right == NULL) {
		RGBAPixel thisPixel = thisNode->avg;
		RGBAPixel rootAvg = root->avg;
		int diff = ((thisPixel.r - rootAvg.r)*(thisPixel.r - rootAvg.r)) + ((thisPixel.g - rootAvg.g)*(thisPixel.g - rootAvg.g)) + ((thisPixel.b - rootAvg.b)*(thisPixel.b - rootAvg.b));
		if (diff <= tol)
			return 1;
		else
			return 0;
	}
	else
		return getTotalLeavesWithin(root, tol, thisNode->right) + getTotalLeavesWithin(root, tol, thisNode->left);

}

void twoDtree::clearHelper(Node* orig) {
	if (orig == NULL) {
		return;
	}else {
		clearHelper(orig->left);
		clearHelper(orig->right);
		delete(orig);
	}
}

void twoDtree::clear() {
	clearHelper(root);
}




void twoDtree::copy(const twoDtree & orig){
	root = copyHelper(orig.root);
	height = orig.height;
	width = orig.width;
}

twoDtree::Node * twoDtree::copyHelper(Node* orig) {
	if (orig == NULL)
		return NULL;

	Node* subroot = new Node(orig->upLeft, orig->lowRight, orig->avg);
	
	subroot->left = copyHelper(orig->left);
	subroot->right = copyHelper(orig->right);
	return subroot;
} 



