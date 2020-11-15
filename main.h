#pragma once
#define RECURSIVE 1
#define DEADEND 2
#define LEFTSIDE 3
#define RIGHTSIDE 4

#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

#define CLEAR false
#define WALL true

#pragma region includes
#include <cstdlib>
#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#pragma endregion

#pragma region main
int cols, rows; // stores height and width of maze
bool isStart = false, isEnd = false; // used for generation

std::string outputPath;
cv::Mat inputImg; // input img matrix

cv::Mat maze; // maze's matrix
cv::Mat wasHere; // movements history
cv::Mat correctPath; // computed solution of the maze

int startX, startY, endX, endY; // start and end coordinates of the maze

// generate matrix from given image file and stores it to {maze}
int generateMaze(std::string filename);

// launch selected method with the correct function and arguments
int launch(std::string filename, int solvingMethod);

// saves {maze} to {outputPath} on the disk
void outputMaze();

// solver's entry point
int main(int argc, char* argv[]);
#pragma endregion

#pragma region checks
// check if the {maze}'s tile is clear
bool isClear(int x, int y);

// check if the {maze}'s tile is a wall
bool isWall(int x, int y);

// check if the {maze}'s tile is a deadend
bool isDeadend(int x, int y);

// return the number of clear tiles around the given coordinates
int nbrClearAround(int x, int y);
#pragma endregion

// return a vector of coordinates until the next intersection (need to start on a deadend)
std::vector<std::vector<int>> pathFollower(int x, int y);

#pragma region Recursive Solve Method
unsigned int countRecursive = 0; // nbr of recursives used by {recursiveSolve}

// solves maze by recursively calling itself
int recursiveSolve(int x, int y);
#pragma endregion

#pragma region Deadend Filling Method
// return first deadend in the maze
std::vector<int> getDeadend(int offset_x, int offset_y);

// return a vector of all deadends in the maze
std::vector<std::vector<int>> getDeadends();

// fill given deadend with magenta in {inputImg}
void fillDeadend(std::vector<int> deadend);

// same as fillDeadend but with multiple deadends
void fillDeadends(std::vector<std::vector<int>> deadends);

// solve maze by filling all deadends until no deadends are left
int deadendFilling();
#pragma endregion

#pragma region Wall Follow Methods
// solve maze by following the left wall
int leftSideSolving(int x, int y);

// solve maze by following the right wall
int rightSideSolving(int x, int y);
#pragma endregion
