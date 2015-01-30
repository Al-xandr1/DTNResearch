#include "math.h"
#include <queue>

using namespace std;

class Histogram {
	private:
		double* hist;		//histogram of values
		int cells;		    //intervals count for histogram
		double maxVal;		    //maximum accountable value
		double widthOfCell;
		long countValues;	//total count of putted values
		long overflowValues;
		long underflowValues;

	public:
		Histogram(int cells, double maxVal){
			this->cells = cells;
			this->maxVal = maxVal;
			this->widthOfCell = maxVal / (1.0 * cells);
			this->countValues = 0;
			this->overflowValues = 0;
			this->underflowValues = 0;

			this->hist = new double[cells];
			for (int i = 0; i < this->cells; this->hist[i++] = 0);
		};

		~Histogram(){
			delete[] hist;
		};

		int put(double val){
			return recursivePut(val, getStratIndex(val));
		};

		double getCheckSum(){
			double checkSum = 0;
			for(int i = 0; i < cells; i++)
                checkSum += getHistValue(i);
			return checkSum;
		};

		vector<double>* toVector()
		{
		    vector<double>* vec = new vector<double>();
            for (int i = 0; i < this->cells; i++)
                vec->push_back(getHistValue(i));
            return vec;
		}

		double getHistValue(int index) {return hist[index] / countValues;}
		int getCells()                 {return cells;}
		double getAccountableMaxVal()  {return maxVal;}
		double getWidthOfCell()        {return widthOfCell;}
		long getOverflowValues()       {return overflowValues;}
		long getUnderflowValues()      {return underflowValues;}

    private:
		//check value entering (value) into range [leftIndex, leftIndex + widthOfCell).
		int isIntoRange(int leftIndex, double value){
			int rightIndex = leftIndex + 1;

			double leftEnd = leftIndex * widthOfCell;
			double rightEnd = rightIndex * widthOfCell;

			bool rightBound;
			if (rightIndex == cells){// is it include right end of section or not
				rightBound = value <= rightEnd;
			} else{
				rightBound = value < rightEnd;
			}

			if ( leftEnd <= value  &&  rightBound ){
				return 0;
			} else if (value < leftEnd){
				return -1;
			} else if (rightEnd <= value){
				return 1;
			}
		};

		int recursivePut(double val, int index){
			if (index < 0){
				underflowValues++;
				return -1;
			}
			if (index > cells-1){
				overflowValues++;
				return -1;
			}

			int res = isIntoRange(index, val);
			if (res == 0){
				hist[index]++;
				countValues++;
				return index;
			} else if(res < 0){//go left
				return recursivePut(val, index--);//index-1
			} else{//go right
				return recursivePut(val, index++);//index+1
			}
		};

		int getStratIndex(double val){
			return (int) floor(val / widthOfCell);
		}
};
