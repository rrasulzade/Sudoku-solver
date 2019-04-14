//
//  sudokuSolver.cpp
//  Sudoku
//
//  Created by Rasul:
//  Copyright (c) 2015 ***Rasul Rasulzade***. All rights reserved.
//


#include <iostream>
#include <time.h>
#include <vector>

using namespace std;
int s = 0;


struct constraints {
    int row;
    int col;
    int mrv;            //minimum remaining values - mrv
    int mcv;            //most constraining values - mcv
};

//global variables
vector <struct constraints> mrv_mcv;   //stores values of mrv and mcv for each cell in sudoku grid which are unassigned
int  least_used_values[9] = {0};        // least used number in whole sudoku grid
int vector_index = 0;

//function declarions
bool isFull(int (&grid)[9][9]);
void unassignedLoc(int (&grid)[9][9], int &row, int &col);
int getLeastValue(vector<int> &curDomain);
int getValue(vector<int> &curDomain);
bool updateDomain(int (&grid)[9][9], int row, int col, vector<int> &curDomain);
bool checkConstraints(int (&grid)[9][9], int row, int col, int val);
void cell_mrv_mcv(int (&grid)[9][9], int row, int col);
bool SudokuSolverB(int (&grid)[9][9]);
bool SudokuSolverBFC(int (&grid)[9][9]);



// find least constraining values
void lcv(int (&grid)[9][9])
{
    for (int i = 0; i < 9; i++) {
        least_used_values[i] = 0;
    }
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] > 0) {
                least_used_values[grid[i][j]-1]++;
            }
        }
    }
}

//find mrv and mcv for each cell separately
//by calling cell_mrv_mcv
void find_mrv_mcv(int (&grid)[9][9])
{
    mrv_mcv.resize(0);
    vector_index = 0;
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) {
                cell_mrv_mcv(grid, i, j);
            }
        }
    }
}

//find mrv and mcv for the given cell and
//store in "mrv_mcv" vector array with its location in the grid
void cell_mrv_mcv(int (&grid)[9][9], int row, int col)
{
    int mvr_temp[9] = {0};
    int remaining_values = 0;
    int unassigned_locations = 0, max = 0;
    
    for (int j = 0; j < 9; j++){
        if (grid[row][j] > 0){
            mvr_temp[grid[row][j]-1]++;
        }else{
            unassigned_locations++;
        }
    }
    
    if (unassigned_locations > max) {
        max = unassigned_locations;
        unassigned_locations = 0;
    }
    
    for (int i = 0; i < 9; i++){
        if (grid[i][col] > 0){
            mvr_temp[grid[i][col]-1]++;
        }else{
            unassigned_locations++;
        }
    }
    
    if (unassigned_locations > max) {
        max = unassigned_locations;
        unassigned_locations = 0;
    }
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if (grid[i + row - row%3][j + col - col%3] > 0){
                mvr_temp[grid[i + row - row%3][j + col - col%3] - 1]++;
            }else{
                unassigned_locations++;
            }
        }
    }
    
    if (unassigned_locations > max) {
        max = unassigned_locations;
    }
    
    for (int i = 0; i < 9; i++) {
        if(mvr_temp[i] == 0){
            remaining_values++;
        }
    }
    
    mrv_mcv.push_back(constraints());
    mrv_mcv[vector_index].row = row;
    mrv_mcv[vector_index].col = col;
    mrv_mcv[vector_index].mrv = remaining_values;
    mrv_mcv[vector_index].mcv = max;
    vector_index++;
}



// check whether sudoku grid is fully solved
bool isFull(int (&grid)[9][9]){
    
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

// find an unassigned location
void unassignedLoc(int (&grid)[9][9], int &row, int &col){
    int i, j;
    i = rand() % 9;
    j = rand() % 9;
    while (grid[i][j] != 0) {
        i = rand() % 9;
        j = rand() % 9;
    }
    row = i;
    col = j;
}

//check whether constraints are satisfied
//if so, return "true"; otherwise "false"
bool checkConstraints(int (&grid)[9][9], int row, int col, int val){
    for (int j = 0; j < 9; j++){
        if (grid[row][j] == val){
            return false;
        }
    }
    for (int i = 0; i < 9; i++){
        if (grid[i][col] == val){
            return false;
        }
    }
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            if (grid[i + row - row%3][j + col - col%3] == val){
                return false;
            }
        }
    }
    return true;
}

//create new domain for newly "discovered" cell
bool updateDomain(int (&grid)[9][9], int row, int col, vector<int> &curDomain){
    vector<int> counting (9,0);
    
    for (int j = 0; j < 9; j++){
        if (grid[row][j] > 0){
            counting[grid[row][j]-1]++;
        }
    }
    
    for (int i = 0; i < 9; i++){
        if (grid[i][col] > 0){
            counting[grid[i][col]-1]++;
        }
    }
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++)
            if (grid[i + row - row%3][j + col - col%3] > 0){
                counting[grid[i + row - row%3][j + col - col%3] - 1]++;
            }
    }
    
    vector<int> temp;
    for (int i = 0; i < counting.size(); i++) {
        if(counting[i] == 0){
            temp.push_back(i+1);
        }
    }
    
    curDomain = temp;
    
    return false;
}


//use forward checking to find remaining values
//and using lcv find the least used one
int getLeastValue(vector<int> &curDomain)
{
    vector<int> temp;
    
    for (int i = 0; i < curDomain.size(); i++) {
        temp.push_back(least_used_values[curDomain[i]-1]);
    }
    
    int min = 10, val = -1;
    
    for (int i = 0; i < temp.size(); i++) {
        if (min > temp[i]) {
            min = temp[i];
            val = curDomain[i];
        }
    }
    
    return val;
}

//randomly pick currently available numbers in the domain
//of each cell
int getValue(vector<int> &curDomain)
{
    if (!curDomain.empty()) {
        int i = rand() % curDomain.size();
        return curDomain[i];
    }
    return -1;
}


//BT
bool SudokuSolverB(int (&grid)[9][9]){
    int row, col;
    s++;
    
    if (isFull(grid)) {
        return true;
    }
    unassignedLoc(grid, row, col);
    
    vector<int> occured;
    vector<int>::iterator it;
    
    while (occured.size() != 9) {
        int val = rand() % 9 + 1;
        it = find (occured.begin(), occured.end(), val);
        if (it == occured.end()){
            occured.push_back(val);
        }

        if (checkConstraints(grid, row, col, val)) {
            grid[row][col] = val;
            if (SudokuSolverB(grid)) {
                return true;
            }
        }
        grid[row][col] = 0;
    }
    return false;
}


// BT + FC
bool SudokuSolverBFC(int (&grid)[9][9]){
    int row, col;
    s++;
    
    if (isFull(grid)) {
        return true;
    }
    unassignedLoc(grid, row, col);
    
    //create curDomain for each cell
    vector<int> curDomain;
    
    updateDomain(grid, row, col, curDomain);
    int val = getValue(curDomain);
    if (val != -1) {
        curDomain.erase(remove(curDomain.begin(), curDomain.end(), val));
    }
    
    while (val != -1) {
        grid[row][col] = val;
        val = getValue(curDomain);
        if (val != -1) {
            curDomain.erase(remove(curDomain.begin(), curDomain.end(), val));
        }
        
        if (SudokuSolverBFC(grid)) {
            return true;
        }
        grid[row][col] = 0;
    }
    return false;
}


// BT + FC + H
bool SudokuSolverBFCH(int (&grid)[9][9]){
    int row, col;
    vector <int> locations;
    s++;
    
    if (isFull(grid)) {
        return true;
    }
    
    find_mrv_mcv(grid);
    
    //find minimum mrv among the cells of the grid
    int min_mrv = 9, loc = 0;
    for (int i = 0; i < mrv_mcv.size(); i++) {
        if (min_mrv > mrv_mcv[i].mrv) {
            loc = i;
            min_mrv = mrv_mcv[i].mrv;
        }
    }
    
    // check whether there is a tie
    // if so, take maximum mcv
    for (int i = 0; i < mrv_mcv.size(); i++) {
        if (min_mrv == mrv_mcv[i].mrv) {
            if (mrv_mcv[loc].mcv < mrv_mcv[i].mcv) {
                loc = i;
            }
        }
    }
    // collect locations which has the same mrv and mcv
    // then randomly choose one
    for (int i = 0; i < mrv_mcv.size(); i++) {
        if (min_mrv == mrv_mcv[i].mrv) {
            if (mrv_mcv[loc].mcv == mrv_mcv[i].mcv) {
                locations.push_back(i);
            }
        }
    }
    
    while (!locations.empty()) {
        loc = locations[rand() % locations.size()];
        locations.erase(remove(locations.begin(), locations.end(), loc));
        
        row = mrv_mcv[loc].row;
        col = mrv_mcv[loc].col;
        
        //create curDomain for each location
        vector<int> curDomain;
        updateDomain(grid, row, col, curDomain);
        
        // create lcv and use it for forward checking
        lcv(grid);
        int val = getLeastValue(curDomain);
        if (val != -1) {
            curDomain.erase(remove(curDomain.begin(), curDomain.end(), val));
        }
        
        while (val != -1) {
            grid[row][col] = val;
            val = getLeastValue(curDomain);
            if (val != -1) {
                curDomain.erase(remove(curDomain.begin(), curDomain.end(), val));
            }
            if (SudokuSolverBFCH(grid)) {
                return true;
            }
            grid[row][col] = 0;
        }
        return false;
        
    }
    return false;
}


int main(){
    
    srand((unsigned)time(0));
    
    int easy[9][9] = {
        {3, 0, 9, 0, 0, 0, 0, 4, 2},
        {0, 1, 8, 9, 4, 3, 6, 0, 0},
        {0, 0, 0, 0, 0, 0, 8, 9, 0},
        {0, 0, 3, 0, 9, 0, 0, 6, 0},
        {4, 2, 7, 0, 0, 0, 5, 8, 9},
        {0, 6, 0, 0, 8, 0, 2, 0, 0},
        {0, 7, 2, 0, 0, 0, 0, 0, 0},
        {0, 0, 4, 5, 7, 6, 3, 2, 0},
        {6, 3, 0, 0, 0, 0, 7, 0, 4}
    };
    
    int medium[9][9] = {
        {0, 0, 0, 1, 2, 0, 0, 3, 0,},
        {0, 0, 3, 0, 8, 0, 0, 1, 6,},
        {4, 0, 0, 5, 3, 0, 0, 9, 0,},
        {0, 1, 0, 8, 0, 0, 5, 2, 0,},
        {0, 4, 0, 0, 0, 0, 0, 6, 0,},
        {0, 6, 8, 0, 0, 2, 0, 7, 0,},
        {0, 8, 0, 0, 9, 3, 0, 0, 2,},
        {6, 9, 0, 0, 5, 0, 3, 0, 0,},
        {0, 3, 0, 0, 4, 8, 0, 0, 0,}
    };
    
    int hard[9][9]={
        {6, 0, 0, 8, 0, 9, 0, 0, 0},
        {0, 0, 5, 0, 0, 7, 0, 8, 6},
        {0, 7, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 4, 0, 1, 3, 0, 7},
        {8, 0, 1, 0, 0, 0, 5, 0, 4},
        {7, 0, 9, 2, 0, 5, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 4, 0},
        {1, 8, 0, 5, 0, 0, 6, 0, 0},
        {0, 0, 0, 3, 0, 4, 0, 0, 5}
    };
    
    int evil[9][9] = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0},
        {7, 0, 0, 0, 0, 8, 1, 0, 2},
        {0, 6, 3, 0, 5, 0, 0, 0, 0},
        {0, 7, 0, 3, 9, 0, 0, 0, 0},
        {0, 0, 5, 8, 0, 4, 6, 0, 0},
        {0, 0, 0, 0, 2, 5, 0, 4, 0},
        {0, 0, 0, 0, 1, 0, 8, 7, 0},
        {2, 0, 8, 9, 0, 0, 0, 0, 3},
        {0, 0, 0, 0, 0, 0, 0, 0, 6},
    };

    //For running the provided SudokuSolver
    //for the given puzzles above call:
    
    /************************************************
       SudokuSolverB("puzzle name")    for BT
       SudokuSolverBFC("puzzle name")  for BT+FC
       SudokuSolverBFCH("puzzle name") for BT+FC+H
    ***********************************************/
  
    //Example:
    if (SudokuSolverBFC(hard)){
        
       /* prints the solved puzzle
        
        for(int i = 0; i < 9; i++){
            for(int j = 0; j < 9; j++){
                cout << evil[i][j] << ' ';
        
            cout << endl;
        }
     */
        //number of nodes
        cout << s << endl;
    }
    
    return 0;
}