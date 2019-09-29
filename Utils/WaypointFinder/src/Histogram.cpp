#include "math.h"
#include <queue>

using namespace std;

struct Cell
{
    double pdfValue;
    double width;
    double hight;
    double leftBound;
    double rightBound;
};

class Histogram {
	private:
		double* hist;		//histogram of values
		int cells;		    //intervals count for histogram
		double rightBound;	//maximum accountable value
		double widthOfCell;
		long puttedValues;	//total count of putted values
		long overflowValues;
		long underflowValues;

	public:
		Histogram(int cells, double rightBound){
			this->cells = cells;
			this->rightBound = rightBound;
			this->widthOfCell = rightBound / (1.0 * cells);
			this->puttedValues = 0;
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
            vector<double>* pdf = toPDFVector();
			for(unsigned int i = 0; i < pdf->size(); i++)
                checkSum += pdf->at(i);
            delete pdf;
			return checkSum;
		};

		vector<double>* toPDFVector()
		{
		    vector<double>* pdf = new vector<double>();
            for (unsigned int i = 0; i < this->cells; i++)
                pdf->push_back(getPDFValue(i));
            return pdf;
		}

		vector<double>* toCDFVector()
		{
		    vector<double>* cdf = new vector<double>();
            vector<double>* pdf = toPDFVector();
            for (unsigned int i = 0; i < pdf->size(); i++)
            {
                double val = 0;
                for (unsigned int j = 0; j <= i; j++) val += pdf->at(j);
                cdf->push_back(val);
            }
            delete pdf;
            return cdf;
		}

        vector<double>* toCCDFVector()
		{
		    vector<double>* ccdf = new vector<double>();
            vector<double>* pdf = toPDFVector();
            for (int i = 0; i < pdf->size(); i++) {
                double val = 0;
                for (int j = i+1; j < pdf->size(); j++) val+= pdf->at(j);
                ccdf->push_back(val);
            }
            delete pdf;
            return ccdf;
		}

		Cell getCell(int index)
		{
		    Cell cell;
		    cell.pdfValue = hist[index] / this->puttedValues;
            cell.width = this->widthOfCell;
            cell.hight = cell.pdfValue / cell.width;
		    cell.leftBound = this->widthOfCell * index;
		    cell.rightBound = this->widthOfCell * (index+1);
		    return cell;
        }

        void truncate()
        {
            int maxCellIndex = 0;
            for (int i = 0; i < this->cells; i++) if (hist[i] > 0) maxCellIndex = i;
            if (maxCellIndex < this->cells - 1)
            {
                this->cells = maxCellIndex + 1;
                this->rightBound = this->widthOfCell * this->cells;
            }
        }

		double getPDFValue(int index) {return getCell(index).pdfValue;}
		int getCells()                 {return cells;}
		double getRightBound()         {return rightBound;}
		double getLeftBound()          {return 0;}
		double getWidthOfCell()        {return widthOfCell;}
		long getTotalValues()          {return puttedValues;}
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
				puttedValues++;
				return index;
			} else if(res < 0){//go left
				return recursivePut(val, --index);//index-1
			} else{//go right
				return recursivePut(val, ++index);//index+1
			}
		};

		int getStratIndex(double val){
			return (int) floor(val / widthOfCell);
		}
};
