#include "main.h"

#pragma region main
int generateMaze(std::string filename) {
	std::cout << "loading file" << std::endl;
	inputImg = cv::imread(filename, cv::IMREAD_COLOR); // read given image and converts it to a usable matrix
	std::cout << "image loaded" << std::endl;

	if (inputImg.empty()) return 2; // if image doesn't exists or is corrupted

	// stores width and height of loaded image
	cols = inputImg.cols;
	rows = inputImg.rows;

	// creates empty matrix to process maze
	maze = cv::Mat(cv::Size(cols, rows), CV_8U);
	wasHere = cv::Mat(cv::Size(cols, rows), CV_8U);
	correctPath = cv::Mat(cv::Size(cols, rows), CV_8U);

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			wasHere.at<bool>(cv::Point(x, y)) = CLEAR;
			correctPath.at<bool>(cv::Point(x, y)) = CLEAR;

			cv::Vec3b pxl = inputImg.at<cv::Vec3b>(cv::Point(x, y));
			int b = pxl.val[0];
			int g = pxl.val[1];
			int r = pxl.val[2];

			if (!r && !g && !b) {
				maze.at<bool>(cv::Point(x, y)) = WALL;
			}
			else if (!r && g && !b) {
				maze.at<bool>(cv::Point(x, y)) = WALL;
				startX = x;
				startY = y;
				isStart = true;
			}
			else if (r && !g && !b) {
				maze.at<bool>(cv::Point(x, y)) = CLEAR;
				endX = x;
				endY = y;
				isEnd = true;
			}
			else {
				maze.at<bool>(cv::Point(x, y)) = CLEAR;
				if (x == 0 || x == cols-1 || y == 0 || y == rows - 1) {
					if (!isStart) {
						startX = x;
						startY = y;
						isStart = true;
						inputImg.at<cv::Vec3b>(cv::Point(x, y)) = cv::Vec3b(0, 255, 0);
					}
					else {
						endX = x;
						endY = y;
						isEnd = true;
						inputImg.at<cv::Vec3b>(cv::Point(x, y)) = cv::Vec3b(0, 0, 255);
					}
				}
			}
		}
	}
	if (!(isStart && isEnd)) return 3;
	std::cout << "maze generated" << std::endl;
	return 0;
}

int launch(std::string filename, int solvingMethod) {
	int status = generateMaze(filename);
	if (status) return status;

	switch (solvingMethod)
	{
	case RECURSIVE:
		outputPath = "./output_recursive.png";
		status = recursiveSolve(startX, startY);
		break;
	case DEADEND:
		outputPath = "./output_deadend.png";
		std::cout << "solving with the deadend filling method..." << std::endl;
		deadendFilling();
		std::cout << "solved!" << std::endl;
		status = 0;
		break;
	case LEFTSIDE:
		outputPath = "./output_leftside.png";
		status = leftSideSolving(startX, startY);
		break;
	case RIGHTSIDE:
		outputPath = "./output_rightside.png";
		status = rightSideSolving(startX, startY);
		break;
	default:
		status = -1;
	}

	return status;
}

void outputMaze() {
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			if (!(startX == col && startY == row || endX == col && endY == row) && correctPath.at<bool>(cv::Point(col, row))) {
				inputImg.at<cv::Vec3b>(cv::Point(col, row)) = cv::Vec3b(255, 0, 0);
			}
		}
	}

	imwrite(outputPath, inputImg);
};

int main(int argc, char* argv[]) {
	//std::string filename = "D:/Dev/Scripts/cpp/utils/Maze_Solver/examples/perfect2k.png";
	//int solveMethod = 2;
	if (argc != 3) {
		std::cout << "only 2 arguments are allowed\nsyntax: 'maze_solver <input_file> <solve_method>'\nAvailable methods:" << std::endl;
		std::cout << "1. Recursive Solve (limited)" << std::endl
			 << "2. Deadend Filling Method" << std::endl
			 << "3. Left-Wall Method" << std::endl
			 << "4. Right-Wall Method" << std::endl;
		return -1;
	}
	std::string filename = std::string(argv[1]);
	int solveMethod = atoi(argv[2]);
	
	int errorCode = launch(filename, solveMethod);

	switch (errorCode)
	{
	case -1:
		std::cout << "Unknown solve method" << std::endl;
		break;
	case 0:
		outputMaze();
		std::cout << "solved maze exported!" << std::endl;
		break;
	case 1:
		std::cout << "can't solve maze." << std::endl;
		break;
	case 2:
		std::cout << "error occured while opening image" << std::endl;
		break;
	case 3:
		std::cout << "error while generating maze" << std::endl;
		break;
	default:
		std::cout << "Unknown error occured" << std::endl;
		break;
	}

	return errorCode;
}
#pragma endregion

#pragma region checks
bool isClear(int x, int y) {
	return !isWall(x, y);
}

bool isWall(int x, int y)
{
	if (x < 0 || x >= cols || y < 0 || y >= rows) return false;
	else return maze.at<bool>(cv::Point(x, y));
}

bool isDeadend(int x, int y)
{
	//deadend = self + 1 empty cell only
	if (isClear(x, y) && nbrClearAround(x, y) == 1)
		return true;
	else
		return false;
}

int nbrClearAround(int x, int y) {
	int countEmptyCells = 0;

	if (isClear(x - 1, y)) countEmptyCells++;
	if (isClear(x + 1, y)) countEmptyCells++;
	if (isClear(x, y - 1)) countEmptyCells++;
	if (isClear(x, y + 1)) countEmptyCells++;
	return countEmptyCells;
}
#pragma endregion

std::vector<std::vector<int>> pathFollower(int x, int y)
{
	if (!isDeadend(x, y) || isWall(x, y) || nbrClearAround(x, y) != 1) return { {NULL, NULL} };

	std::vector<std::vector<int>> path;
	int lastDir = 0;
	do {
		path.push_back({ x, y });
		if (isClear(x - 1, y) && lastDir != LEFT)
		{
			lastDir = RIGHT;
			x--;
		}
		else if (isClear(x + 1, y) && lastDir != RIGHT)
		{
			lastDir = LEFT;
			x++;
		}
		else if (isClear(x, y - 1) && lastDir != UP)
		{
			lastDir = DOWN;
			y--;
		}
		else if (isClear(x, y + 1) && lastDir != DOWN)
		{
			lastDir = UP;
			y++;
		}
	} while (nbrClearAround(x, y) == 2);

	return path;
}

#pragma region Recursive Solving Method
int recursiveSolve(int x, int y) {
	countRecursive++;
	if (x == endX && y == endY) return 0; // if you reached the end
	if (maze.at<bool>(cv::Point(x, y)) || wasHere.at<bool>(cv::Point(x, y))) return 2; // if you are on a wall or already were here

	wasHere.at<bool>(cv::Point(x, y)) = true;
	if (x != 0) // Checks if not on left edge
		if (!recursiveSolve(x - 1, y)) { // Recalls method one to the left
			correctPath.at<bool>(cv::Point(x, y)) = true; // Sets that path value to true;
			return 0;
		}
	if (x != cols - 1) // Checks if not on right edge
		if (!recursiveSolve(x + 1, y)) { // Recalls method one to the right
			correctPath.at<bool>(cv::Point(x, y)) = true;
			return 0;
		}
	if (y != 0)  // Checks if not on top edge
		if (!recursiveSolve(x, y - 1)) { // Recalls method one up
			correctPath.at<bool>(cv::Point(x, y)) = true;
			return 0;
		}
	if (y != rows - 1) // Checks if not on bottom edge
		if (!recursiveSolve(x, y + 1)) { // Recalls method one down
			correctPath.at<bool>(cv::Point(x, y)) = true;
			return 0;
		}
	return 1;
}
#pragma endregion

#pragma region Deadend Filling Method
std::vector<int> getDeadend(int offset_x, int offset_y)
{
	for (int y = offset_y; y < rows; y++) {
		for (int x = offset_x; x < cols; x++) {
			if (isDeadend(x, y) && !(startX == x && startY == y || endX == x && endY == y)) {
				return { x, y };
			}
		}
	}
	return {};
}

std::vector<std::vector<int>> getDeadends()
{
	std::vector<std::vector<int>> allDeadends;

	std::vector<int> deadend = getDeadend(0, 0);
	while (!deadend.empty()) {
		allDeadends.push_back(deadend);
		deadend = getDeadend(deadend[0]+1, deadend[1]);
	}

	return allDeadends;
}

void fillDeadend(std::vector<int> deadend) {

	cv::Point currentPos(deadend[0], deadend[1]);
	maze.at<bool>(currentPos) = true;
	inputImg.at<cv::Vec3b>(currentPos) = cv::Vec3b(255, 0, 255);
}

void fillDeadends(std::vector<std::vector<int>> deadends)
{
	std::vector<std::vector<int>>::size_type len = deadends.size();

	for (int i = 0; i < len; ++i) {
		fillDeadend(deadends[i]);
	}
}

int deadendFilling()
{
	while (true)
	{
		std::vector<std::vector<int>> deadends = getDeadends();
		if (!deadends.empty())
			fillDeadends(deadends);
		else
			return 0;
	}
}
#pragma endregion

#pragma region Wall Follow Methods
int leftSideSolving(int x, int y) {
	return 0;
}

int rightSideSolving(int x, int y) {
	return 0;
}
#pragma endregion
