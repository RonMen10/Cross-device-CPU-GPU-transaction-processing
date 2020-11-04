using namespace std;
#include <array>
#include <vector>
#include <list>
#include "include/hashing.hpp"


MainArray::MainArray(){
    arraySize = 1;
}

LinkList myArray[1024];

//______Hash Build____
LinkList MainArray::createArray(vector<int> data){
    
    for ( int i = 0; i < 1024; i++)
    {
    int element = data.at(i);
    // Inserting in the array
    int hashValue = -1;
    Hashing newTable;
    hashValue = newTable.hashFunction(element, data);
    if (hashValue != -1){
        myArray[hashValue].addNode(element); //adding values to the lists
    }

    for (LinkList elem : myArray){
         elem.printLinkList();
    } 
    }
    return myArray[1024];                     

}

//_________Hash Join_____
LinkList MainArray::createTableJoin(vector<int> data){

    Hashing newTable;
    for (int i = 0; i < 1024; i++)
    {
        int element = data.at(i);
        int hashValue = -1;
        hashValue = newTable.hashFunction(element, data);
        if (hashValue != -1)
        {
            myArray[hashValue].addNode(element); //adding values to the lists
        }

    for (LinkList elem : myArray){
        elem.printLinkList();
    }
    }
    return myArray[1024];
}
