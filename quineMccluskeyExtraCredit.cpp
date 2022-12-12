#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
using namespace std;


// Miscellaneous functions

// Simple selection sort algorithm
void sort(int* arr, int n){
	
	for(int i = 0; i < n; i++){
		int min = *(arr+i);
		int minIndex = i;
		for(int j = i; j < n; j++){
			if(*(arr+j) < min){
				min = *(arr+j);
				minIndex = j;
			}
		}
		*(arr+minIndex) = *(arr+i);
		*(arr+i) = min;
	}
}

//Term represents the minterms and grouping of minterms done in the Quine-McCluskey Algorithm
class Term
{
	private:
	//nums is an array of numbers represented by the groupings
	int* nums;
	int numsLength;
	// bits represents the bits of the groupings
	int* bits;
	int bitsLength;
	// used represents whether the term has been combined further
	bool used;
	
	public:
	//Constructor
	Term(int, int);
	
	//Accessor Methods
	int* getNums();
	int getNumsLength();
	int* getBits();
	int getBitsLength();
	bool isUsed();
	
	//Mutator Methods
	void setNums(int, int);
	void setBits(int, int);
	//use sets used to true;
	void use();
	
	bool isEqual(Term);
	
	// Compare returns true if the parameter Term can group with the current Term
	bool compare(Term);
	
	// Combine returns the current Term combined with the parameter Term as in the next iteration of the Quine-McCluskey Algorithm
	Term combine(Term);
	
};

//Constructor
Term::Term(int n, int bLength){
	numsLength = n;
	nums = new int[numsLength];
	bitsLength = bLength;
	bits = new int[bitsLength];
}

//Accessor Methods - self explanatory
int* Term::getBits(){
	return bits;
}
int Term::getBitsLength(){
	return bitsLength;
}
int* Term::getNums(){
	return nums;
}
int Term::getNumsLength(){
	return numsLength;
}
bool Term::isUsed(){
	return used;
}

// Mutator Methods - self explanatory
void Term::setNums(int index, int num){
	*(nums+index) = num;
}
void Term::setBits(int index, int bit){
	*(bits+index) = bit;
}
void Term::use(){
	used = true;
}

bool Term::isEqual(Term term2){
	int* nums2 = term2.getNums();
	
	for(int i = 0; i < numsLength; i++){
		if(nums[i] != nums2[i]){
			return false;
		}
	}
	return true;
	
}


//
bool Term::compare(Term term2){
	int* bits2 = term2.getBits();
	//Count is number of different bits between the 2 Terms
	int count = 0;
	for(int i = 0; i < bitsLength; i++){
		if(*(bits+i) != *(bits2+i)){
			count++;
		}
	}
	
	return count == 1;
}

Term Term::combine(Term term2){
	int* bits2 = term2.getBits();
	int* nums2 = term2.getNums();
	this->use();
	term2.use();
	
	Term combined((numsLength + term2.getNumsLength()), bitsLength);
	int index = 0;
	
	// Copy the nums from current term to new term
	for(int i = 0; i < numsLength; i++){
		*(combined.getNums()+index) = *(nums+i);
		index++;
	}
	
	// Copy the nums from term2 to new term
	for(int i = 0; i < term2.getNumsLength(); i++){
		*(combined.getNums()+index) = *(nums2+i);
		index++;
	}
	sort(combined.getNums(), combined.getNumsLength());
	
	// Copy bits to new term as described in the algorithm
	
	for(int i = 0; i < bitsLength; i++){
		// Set bits[i] to -1, which is equivalent to X, if the bits of the two terms are different
		if(*(bits+i) != *(bits2+i)){
			*(combined.getBits()+i) = -1;
		}
		else{
			*(combined.getBits()+i) = *(bits+i);
		}
	}
	
	return combined;
}

// takes the entire iteration and determines if there are more terms that can be combined
bool canCombine(vector<vector<Term>> groups){
	for(unsigned int i = 0; i < groups.size()-1; i++){
		for(unsigned int j = 0; j < groups[i].size(); j++){
			for(unsigned int k = 0; k < groups[i+1].size(); k++){
				if(groups[i][j].compare(groups[i+1][k])){
					return true;
				}
			}
		}
	}
	return false;
}

// Searches the vector of terms to find one that has n
Term searchMinterm(vector<Term> terms, int n){
	for(unsigned int j = 0; j < terms.size(); j++){
		for(int k = 0; k < terms[j].getNumsLength(); k++){
			if(terms[j].getNums()[k] == n){
				return terms[j];
			}
		}
	}
	return Term(0, 0);
}

bool isPresent(vector<Term> terms, int n){
	for(unsigned int j = 0; j < terms.size(); j++){
		for(int k = 0; k < terms[j].getNumsLength(); k++){
			if(terms[j].getNums()[k] == n){
				return true;
			}
		}
	}
	return false;
}



int main(){
	//Design
	//Input file
	/*
	First line: Number of bits, b
	Second line: Number of minterms, n
	Third line : Number of don't cares, d
	
	Next n lines: minterms as integers
	Next d lines: don't cares as integers
	*/
	
	//Declare input stream
	ifstream input;
	ofstream output;
	input.open("data.txt");
	output.open("output.txt");
	//Bits is number of bits
	int bits;
	input >> bits;
	
	//num is number of minterms
	int num;
	input >> num;
	
	// dNum is number of don't cares;
	int dNum;
	input >> dNum;
	
	// Get Inputs
	int* minterms = new int[num];
	for(int i = 0; i < num; i++){
		input >> *(minterms+i);
	}
	sort(minterms, num);
	
	int dontCares[dNum];
	for(int i = 0; i < dNum; i++){
		input >> *(dontCares+i);
	}
	
	
	// iteration is a vector that holds vectors of each group of terms
	vector<vector<Term>> iteration;
	iteration.resize(bits+1);
	// Put minterms into Term objects and put them into iteration
	for(int i = 0; i < num; i++){
		Term term(1, bits);
		term.setNums(0, *(minterms+i));
		int count = 0;	//Counts number of 1s to put into correct group
		int x = *(minterms+i);
		// Gets bits of minterm
		for(int j = bits-1; j >= 0; j--){
			term.setBits(j, x%2);
			count += x%2;
			x /= 2;
		}
		iteration[count].emplace_back(term);
	}
	
	// Put dontCares into Term objects and put them into iteration
	for(int i = 0; i < dNum; i++){
		Term term(1, bits);
		term.setNums(0, *(dontCares+i));
		int count = 0;	//Counts number of 1s to put into correct group
		int x = *(dontCares+i);
		// Gets bits of minterm
		for(int j = bits-1; j >= 0; j--){
			term.setBits(j, x%2);
			count += x%2;
			x /= 2;
		}
		iteration[count].emplace_back(term);
	}
	
	vector<Term> primeImplicants;
	
	/*
	---------------------------------
	Step 1 - Finding Prime Implicants
	---------------------------------
	
	Applies Quine-McCluskey Algorithm step 1 normally
	
	
	*/
	
	while(canCombine(iteration)){
		vector<vector<Term>> nextIter;
		nextIter.resize(iteration.size()-1);
		//Loops through the groups of iteration
		for(unsigned int i = 0; i < iteration.size()-1; i++){
			// Checks if there are comparable groups
			if(!(iteration[i].empty() || iteration[i+1].empty())){
				// Loops through all terms of group i
				for(unsigned int j = 0; j < iteration[i].size(); j++){
					// Loops through all terms of group i+1
					for(unsigned int k = 0; k < iteration[i+1].size(); k++){
						// Decides if the two terms are comparable according to Quine-McCluskey
						if(iteration[i][j].compare(iteration[i+1][k])){
							Term term = iteration[i][j].combine(iteration[i+1][k]);
							iteration[i][j].use();
							iteration[i+1][k].use();
							
							bool isPresent = false;
							//Check for duplicates
							for(unsigned int d = 0; d < nextIter[i].size(); d++){
								if(term.isEqual(nextIter[i][d])){
									isPresent = true;
								}
							}
							// Puts the combined term in the next iteration if it's not already present
							if(!isPresent){
								nextIter[i].emplace_back(term);
							}
						}
					}
				}
			}
		}
		
		//Check for Prime Implicants by checking if a term has been or could've been further combined
		
		for(unsigned int i = 0; i < iteration.size(); i++){
			for(unsigned int j = 0; j < iteration[i].size(); j++){
				if(!iteration[i][j].isUsed()){
					primeImplicants.emplace_back(iteration[i][j]);
				}
			}
		}
		
		iteration = nextIter;
		
	}
	
	for(unsigned int i = 0; i < iteration.size(); i++){
		for(unsigned int j = 0; j < iteration.at(i).size(); j++){
			primeImplicants.emplace_back(iteration[i][j]);
		}
	}
	
	
	/*
	-------------------------------------------
	Step 2 - Eliminating unnecessary implicants
	-------------------------------------------
	
	First, get the essential prime implicants
	Then, get the implicant with the most number of minterms that are not accounted for
	Repeat step 2 until all minterms have been accounted for
	
	*/
	
	//tracks number of occurances of each minterm in the prime implicant vector
	// this is to catch essential prime implicants
	vector<Term> minimized;
	int* mintermFreq = new int[num];
	//Initialize array to 0
	for(int i = 0; i < num; i++){
		mintermFreq[i] = 0;
	}
	
	
	for(unsigned int i = 0; i < primeImplicants.size(); i++){
		// Add term to minimized
		// mintermCount counts number of minterms per primeImplicant element. this is to catch duplicates
		int mintermCount = 0;
		for(int j = 0; j < num; j++){
			for(int k = 0; k < primeImplicants[i].getNumsLength(); k++){
				if(primeImplicants[i].getNums()[k] == minterms[j]){
					mintermFreq[j]++;
					
					// to catch duplicates, increase mintermFreq by 1 more so the later functions don't count the prime implicant twice
					if(mintermCount > 0){
						mintermFreq[j]++;
					}
					mintermCount++;
				}
			}
		}
	}
	
	// Put essential prime implicants into the final term vector
	for(int i = 0; i < num; i++){
		if(mintermFreq[i] == 1){
			minimized.emplace_back(searchMinterm(primeImplicants, minterms[i]));
		}
	}
	
	// mintermsVec is a vector of minterms so minterms can be removed from the list
	vector<int> mintermsVec;
	for(int i = 0; i < num; i++){
		mintermsVec.emplace_back(minterms[i]);
	}
	
	// Removes minterms that have already been accounted for
	for(int i = mintermsVec.size(); i >= 0; i--){
		if(isPresent(minimized, mintermsVec[i])){
			mintermsVec.erase(mintermsVec.begin()+i);
		}
	}
	
	
	
	//stores number of currently unassigned minterms each term holds
	int mintermCount[primeImplicants.size()];
	
	
	
	while(!mintermsVec.empty()){
		// Resets mintermCount
		for(unsigned int i = 0; i < primeImplicants.size(); i++){
			mintermCount[i] = 0;
		}
		//Gets the index of the term with the most possible minterms
		int max = 0;
		int maxIndex = 0;
		
		//counts up number of minterms in each term that are still available
		for(unsigned int i = 0; i < primeImplicants.size(); i++){
			
			for(int j = 0; j < primeImplicants[i].getNumsLength(); j++){
				for(unsigned int k = 0; k < mintermsVec.size(); k++){
					if(mintermsVec[k] == primeImplicants[i].getNums()[j]){
						mintermCount[i]++;
					}
				}
			}
			
			if(*(mintermCount+i) > max){
				max = mintermCount[i];
				maxIndex = i;
			}
		}
		
		minimized.emplace_back(primeImplicants[maxIndex]);
		
		// Removes minterms that have already been accounted for
		for(int i = 0; i < primeImplicants[maxIndex].getNumsLength(); i++){
			for(int j = mintermsVec.size()-1; j >= 0; j--){
				if(primeImplicants[maxIndex].getNums()[i] == mintermsVec[j]){
					mintermsVec.erase(mintermsVec.begin()+j);
				}
			}
		}
		
		// End of while loop
	}
	
	// Convert terms to boolean notation
	// f(a,b,..,z) = 
	output << "f(";
	for(int i = 0; i < bits; i++){
		char a = 97+i;
		output << a << ",";
	}
	output << "\b) = ";
	
	// expression
	
	// Print the expression
	for(unsigned int i = 0; i < minimized.size(); i++){
		
		for(int j = 0; j < minimized[i].getBitsLength(); j++){
			char a = 97+j;
			if(minimized[i].getBits()[j] == 1){
				output << a;
			}
			if(minimized[i].getBits()[j] == 0){
				output << a << "\'";
			}
			
		}
		if(i != minimized.size()-1){
			output << " + ";
		}
		
	}
	
	
	
	input.close();
	output.close();
	
	
	return 0;
}
