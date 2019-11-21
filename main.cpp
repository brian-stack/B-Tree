/*************************************************************************************************************************
 * Author: Brian Stack
 * Assignment: B+Tree
 * Date: 11/26/18
 * Class: CS 8
 * CRN: 74231
 * ***********************************************************************************************************************
 * This program tests that the operations defined by the B+Tree class work correctly by inserting many integers
 * into the B+Tree, shuffling the array of items, then deleting each one of them, after each item is inserted or removed,
 * the verify function is called to ensure that all conditions required for a Btree are met.
 *  - A valid BTree must meet the following criteria:
 *    1) For any non-leaf node, a node with k data items has k+1 children.
 *    2) For any node, data[i] < data[i+1] (0 <= i < childCount-1)
 *    3) For any non-leaf node, data[i] > subset[i] (all data items in the subtree)
 *    4) For any non-leaf node, data[i] <= subset[i+1] (all data items in the subtree)
 *    5) All leaf nodes occur at a constant depth.
 *    6) Every node (except the root) must have MINIMUM data items. (the root may be empty if it also has 0 children)
 *    7) Every node may not have more than MAXIMUM data items.
 *    8) Data[i] == the smallest item in subset[i+1] (leftmost data item at the leaf of this subtree)
 ************************************************************************************************************************/
#include "bplustree.h"
#include "map.h"
#include "multimap.h"
#include <iostream>
#include <random>
using namespace std;

bool testBTreeAuto(int how_many, bool report);
void testBTreeAuto(int tree_size, int how_many, bool report);
void testIterator();
void autoMapTest(int n, int iterations);
void autoMMapTest(int n, int iterations);

int main()
{
    testBTreeAuto(1000,100,true);
    testBTreeAuto(1000,100,false);
    testIterator();
    autoMMapTest(1000,100);
    autoMapTest(1000,100);

    return 0;
}

//preconditions: none
//postconditions: BPlusTree iterator will be tested.
void testIterator()
{
    cout<< endl << endl
        << "**************************************************************************"<<endl
        << "                   Testing B+Tree Iterator                                "<<endl
        << "**************************************************************************"<<endl;

    const bool debug = false;
    BPlusTree<int> bpt;
    for (int i= 0; i<125; i++)
        bpt.insert(i);

    cout<<"------------------------------------------------------------"<<endl;
    for(BPlusTree<int>::Iterator it = bpt.begin(); it!= bpt.end(); ++it)
    {
        if (debug)
            it.printIterator();
        cout<<"["<<*it<<"] ->";
        if (debug)
            cout<<endl;
    }

    cout<<endl;
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"test ++ operator: "<<endl;
    BPlusTree<int>::Iterator it = bpt.begin();
    cout<<"{"<<*(it++)<<"}"<<endl;
    cout<<"{"<<*it<<"}"<<endl;

    for(int k = 0; k < 125; k++)
    {
        for (int i = k; i<125; i++)
        {
            it = bpt.getIteratorAtEntry(i);
            if (it.is_null())
                cout<< i <<" was not found."<<endl;
        }
        bpt.remove(k);
    }
    cout<<"===================================================================="<<endl;
}

//preconditions: howMany < MAX
//postconditions: this function will call testBTreeAuto(int,bool) to test the BTree class.
void testBTreeAuto(int treeSize, int howMany, bool report)
{
    bool verified = true;
    for (int i = 0; i < howMany; i++)
    {
        if(report)
        {
            cout<<"*********************************************************"<<endl;
            cout<<" T E S T:    "<<i<<endl;
            cout<<"*********************************************************"<<endl;
        }

        if(!testBTreeAuto(treeSize, report))
        {
            cout<<"T E S T :   ["<<i<<"]    F A I L E D ! ! !"<<endl;
            verified = false;
            return;
        }
    }

    cout<<"**************************************************************************"<<endl;
    cout<<"**************************************************************************"<<endl;
    cout<<"             E N D     T E S T: "<<howMany<<" tests of "<<treeSize<<" items: ";
    cout<<(verified?"VERIFIED": "VERIFICATION FAILED")<<endl;
    cout<<"**************************************************************************"<<endl;
    cout<<"**************************************************************************"<<endl;
}

//preconditions: howMany < MAX
//postconditions: the B+Tree will be tested:
// by inserting many integers into the B+Tree,
// shuffling the array of items, then deleting each one of them,
// after each item is inserted or removed, the verify function is called
// to ensure that all conditions required for a B+tree are met.
bool testBTreeAuto(int howMany, bool report)
{
    const int MAX = 10000;
    assert(howMany < MAX);
    BPlusTree<int> bt;
    int a[MAX];
    int original[MAX];
    int deletedList[MAX];

    int originalSize;
    int size;
    int deletedSize = 0;

    //fill a[ ]
    for (int i= 0; i< howMany; i++)
        a[i] = i;

    shuffleArray(a,howMany);

    //copy  a[ ] -> original[ ]:
    copyArray(original,a,howMany,howMany);
    size = howMany;
    originalSize = howMany;

    for (int i=0; i<size; i++)
        bt.insert(a[i]);

    if (report)
    {
        cout<<"========================================================"<<endl;
        cout<<"  "<<endl;
        cout<<"========================================================"<<endl;
        cout<<bt<<endl<<endl;
    }
    for (int i= 0; i < howMany; i++)
    {
        int r = rand() % (howMany-i);
        bt.remove(a[r]);
        deletedList[deletedSize++] = deleteItem(a, r, size);

        //only print the last 1/10 of deletions to avoid crashing terminal
        if (report && (deletedSize > (howMany / 10)))
        {
            cout<<"  REMOVING ["<<deletedList[deletedSize-1]<<"]"<<endl;
            cout<< "last 5 items deleted: "; printArraySegment(deletedList, deletedSize-5, deletedSize-1);
            cout<<endl << bt << endl;
            cout<<"========================================================"<<endl;
        }

        BPlusTree<int>::Iterator it = bt.begin();
        bool validIterator = true;
        int count = 0;

        while(it != bt.end() && validIterator)
        {
            BPlusTree<int>::Iterator current = it;
            ++it;

            if(!it.is_null() && *it <= *current)
                validIterator = false;
            count++;
        }

        if(!validIterator || count != bt.size())
            cout << "validIterator: " << validIterator << " count: " << count << " bt size: " << bt.size() << endl;

        if (!bt.isValid())
        {
            cout<<setw(6)<<i<<" I N V A L I D   T R E E"<<endl;
            cout<<"Original Array: "; printArray(original, originalSize);
            cout<<"Deleted Items : "; printArray(deletedList, deletedSize);
            cout<<endl<<endl<<bt<<endl<<endl;
            return false;
        }
    }
    if (report) cout<<" V A L I D    T R E E"<<endl;

    return true;
}

//preconditions: none
//postconditions: the MMap will be tested by inserting many random multi-pairs to the MMap,
// searching for them with operator[], and removing them, also the count will be verified for each MPair in the MMap.
// After each insertion or removal, the validity of the MMap will be confirmed using isValid().
void autoMMapTest(int n, int iterations)
{
    cout << string(50,'=') << endl
         << "Starting multimap auto test with: items = " << n << ", over iterations = " << iterations
         << endl << string(50,'=') << endl;

    const int VECTOR_MAX_SIZE = 10;
    const int VECTOR_MIN_SIZE = 1;

    bool isValid = true;
    for(int j = 0; j < iterations; j++)
    {
        MMap<int,int> theMap;

        int * keys = new int[n];
        vector<int> * valueLists = new vector<int>[n];

        for(int i = 0; i < n; i++)
        {
            vector<int> values;
            int numValues = (rand() % (VECTOR_MAX_SIZE - VECTOR_MIN_SIZE + 1)) + VECTOR_MIN_SIZE;
            keys[i] = i;

            for(int c = 0; c < numValues; c++)
            {
                values.push_back(rand() % 100);
                theMap.insert(keys[i],values.at(c));
            }

            valueLists[i] = values;

            if(!theMap.isValid() || theMap.size() != i+1)
            {
                isValid = false;
                cout << "Error, multimap is invalid, when inserting pair: (k,v): (" << keys[i] << ", " << values << ")" << endl;
            }
        }

        for(int i = 0; i < n; i++)
        {
            if(theMap[keys[i]] != valueLists[i])
            {
                cout << "Error, mutlimap is invalid, when calling [] with key = " << keys[i] << endl;
                isValid = false;
            }

            if(theMap.count(keys[i]) != valueLists[i].size())
            {
                cout << "Error, size of vector at: " << keys[i] <<  " does not match expected size. " << endl;
                isValid = false;
            }

        }

        for(int i = 0; i < n; i++)
        {
            theMap.erase(keys[i]);
            if(!theMap.isValid() || theMap.size() != n-i-1)
            {
                isValid = false;
                cout << "Error, multimap is invalid, when erasing pair: (k,v): (" << keys[i] << ", " << valueLists[i] << ")" << endl;
            }
        }

        delete [] keys;
        delete [] valueLists;
    }

    if(!isValid)
    {
        cout << string(50,'=') << endl
             << "Multimap Test Failed!"
             << endl << string(50,'=') << endl;
    }
    else
    {
        cout << string(50,'=') << endl
             << "Multimap Test Passed."
             << endl << string(50,'=') << endl;
    }

}

//preconditions: none
//postconditions: the Map will be tested by inserting many random pairs to the Map,
// searching for them with operator[], and then removing them.
// After each insertion or removal, the validity of the Map will be confirmed using isValid().
void autoMapTest(int n, int iterations)
{
    cout << string(50,'=') << endl
         << "Starting map auto test with: items = " << n << ", over iterations = " << iterations
         << endl << string(50,'=') << endl;

    bool isValid = true;
    for(int j = 0; j < iterations; j++)
    {
        Map<int,int> theMap;
        int * keys = new int[n];
        int * values = new int[n];

        for(int i = 0; i < n; i++)
        {
            keys[i] = i;
            values[i] = rand();
            theMap.insert(keys[i],values[i]);
            if(!theMap.isValid() || theMap.size() != i+1)
            {
                isValid = false;
                cout << "Error, map is invalid, when inserting pair: (k,v): (" << keys[i] << ", " << values[i] << ")" << endl;
            }
        }

        for(int i = 0; i < n; i++)
        {
            if(theMap[keys[i]] != values[i])
            {
                cout << "Error, map is invalid, when calling [] with key = " << keys[i] << endl;
                isValid = false;
            }
        }

        for(int i = 0; i < n; i++)
        {
            theMap.erase(keys[i]);

            if(!theMap.isValid() || theMap.size() != n-i-1)
            {
                isValid = false;
                cout << "Error, map is invalid, when erasing pair: (k,v): (" << keys[i] << ", " << values[i] << ")" << endl;
            }
        }

        delete [] keys;
        delete [] values;
    }

    if(!isValid)
    {
        cout << string(50,'=') << endl
             << "Map Test Failed!"
             << endl << string(50,'=') << endl;
    }
    else
    {
        cout << string(50,'=') << endl
             << "Map Test Passed."
             << endl << string(50,'=') << endl;
    }

}
