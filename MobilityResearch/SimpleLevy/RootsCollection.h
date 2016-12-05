#ifndef ROOTSCOLLECTION_H_
#define ROOTSCOLLECTION_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "INETDefs.h"
#include "Data.h"
#include "Coord.h"
#include "DevelopmentHelper.h"

using namespace std;

/*
 * ��������� ��������� (� ���� ������������������ �������) ��� �������������
 */
class RootsCollection {
public:
    static bool isRootsCollectionReady;
    static vector<RootDataShort> RootsDataShort;        // ����� ��������� �������������. ��������� ������������� ����� allroots.roo
    static vector<vector<HotSpotDataRoot>*> RootsData;  // ����� ��������� �������������. ��������� - ����� ������������������� *.hts

    void readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePatter);

    void printRootsDataShort();
    void printRootsData();
};


#endif /* ROOTSCOLLECTION_H_ */
