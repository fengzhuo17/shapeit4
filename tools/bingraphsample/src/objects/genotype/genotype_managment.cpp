////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 Olivier Delaneau, University of Lausanne
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////
#include <objects/genotype/genotype_header.h>

genotype::genotype(unsigned int _index) {
	index = _index;
	n_segments = 0;
	n_variants = 0;
	n_ambiguous = 0;
	n_stored_transitionProbs = 0;
	n_storage_events = 0;
	std::fill(curr_dipcodes, curr_dipcodes + 64, 0);
	this->name = "";
}

genotype::~genotype() {
	free();
}

void genotype::free() {
	std::fill(curr_dipcodes, curr_dipcodes + 64, 0);
	name = "";
	vector < unsigned char > ().swap(Variants);
	vector < unsigned char > ().swap(Ambiguous);
	vector < unsigned long > ().swap(Diplotypes);
	vector < unsigned short > ().swap(Lengths);
}

void genotype::init() {
	H0 = vector < bool > (n_variants, false);
	H1 = vector < bool > (n_variants, false);
	for (unsigned int v = 0 ; v < n_variants ; v ++) {
		H0[v] = VAR_GET_HAP0(MOD2(v), Variants[DIV2(v)]);
		H1[v] = VAR_GET_HAP1(MOD2(v), Variants[DIV2(v)]);
	}
}

void genotype::makeSample(vector < unsigned char > & DipSampled) {
	for (unsigned int s = 0, vabs = 0, a = 0, m = 0 ; s < n_segments ; s ++) {
		unsigned char hap0 = DIP_HAP0(DipSampled[s]);
		unsigned char hap1 = DIP_HAP1(DipSampled[s]);
		for (unsigned int vrel = 0 ; vrel < Lengths[s] ; vrel++, vabs++) {
			if (VAR_GET_MIS(MOD2(vabs), Variants[DIV2(vabs)])) {
				H0[vabs] = ((rng.getDouble()*n_storage_events)<=ProbMissing[m*HAP_NUMBER+hap0]);
				H1[vabs] = ((rng.getDouble()*n_storage_events)<=ProbMissing[m*HAP_NUMBER+hap1]);
				m++;
			}
			if (VAR_GET_AMB(MOD2(vabs), Variants[DIV2(vabs)])) {
				H0[vabs] = HAP_GET(Ambiguous[a], hap0);
				H1[vabs] = HAP_GET(Ambiguous[a], hap1);
				a++;
			}
		}
	}
}

void genotype::makeBest(vector < unsigned char > & DipSampled) {
	for (unsigned int s = 0, vabs = 0, a = 0, m = 0 ; s < n_segments ; s ++) {
		unsigned char hap0 = DIP_HAP0(DipSampled[s]);
		unsigned char hap1 = DIP_HAP1(DipSampled[s]);
		for (unsigned int vrel = 0 ; vrel < Lengths[s] ; vrel++, vabs++) {
			if (VAR_GET_MIS(MOD2(vabs), Variants[DIV2(vabs)])) {
				H0[vabs] = (ProbMissing[m*HAP_NUMBER+hap0]>=(0.5f*n_storage_events));
				H1[vabs] = (ProbMissing[m*HAP_NUMBER+hap1]>=(0.5f*n_storage_events));
				m++;
			}
			if (VAR_GET_AMB(MOD2(vabs), Variants[DIV2(vabs)])) {
				H0[vabs] = HAP_GET(Ambiguous[a], hap0);
				H1[vabs] = HAP_GET(Ambiguous[a], hap1);
				a++;
			}
		}
	}
}
