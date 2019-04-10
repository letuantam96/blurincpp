#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int col, row, br; //br: the depth of blur
unsigned char IMG1[2000][2000][3]; //original image
unsigned char IMG2[2000][2000][3]; //horizontal blur image
unsigned char IMG3[2000][2000][3]; //vertical blur image
int T[2000][2000][3]; //the sum matrix

int main(int argc, char** argv)
{
	//read the image file
	Mat image = imread("C:/input.png");
	Mat impre = imread("C:/predict.png");

	cout << "Enter the depth of blur: ";
	cin >> br;

	//check for failure
	if (image.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	row = image.rows;
	col = image.cols;
	//export the original image to IMG1 matrix:
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; j++)
			for (int k = 0; k < 3; k++) IMG1[i][j][k] = image.at<Vec3b>(i, j).val[k];

	//HORIZONTAL BLUR - blur theo chieu ngang
	int wH[3000];
	for (int c = 0; c < col; c++)
	{
		if (c < br) wH[c] = br + c + 1;
		else if (c >= col - br) wH[c] = col + br - c;
		else wH[c] = 2 * br + 1;
	}
	//cal IMG2[r][0]
	for (int r = 0; r < row; r++)
	{
		//int T[3];
		T[r][0][0] = T[r][0][1] = T[r][0][2] = 0;
		for (int c = 0; c < wH[0]; c++)
		{
			for (int k = 0; k < 3; k++) T[r][0][k] += IMG1[r][c][k];
		}
		for (int k = 0; k < 3; k++) IMG2[r][0][k] = T[r][0][k] / wH[0];
	}

	//horizontal blur
	for (int r = 0; r < row; r++)
	{
		for (int c = 1; c < col; c++)
		{
			unsigned char L[3], R[3];
			
			if (c - br - 1 < 0) L[0] = L[1] = L[2] = 0;
			else for (int k = 0; k < 3; k++) L[k] = IMG1[r][c - br - 1][k];

			if (c + br >= col) R[0] = R[1] = R[2] = 0;
			else for (int k = 0; k < 3; k++) R[k] = IMG1[r][c + br][k];

			for (int k = 0; k < 3; k++)
			{
				T[r][c][k] = T[r][c-1][k] - L[k] + R[k];
				IMG2[r][c][k] = T[r][c][k] / wH[c];
			}
		}
	}


	//VERICAL BLUR - blur theo chieu doc
	int wV[3000];
	for (int r = 0; r < row; r++)
	{
		if (r < br) wV[r] = br + r + 1;
		else if (r >= row - br) wV[r] = row + br - r;
		else wV[r] = 2 * br + 1;
	}
	//cal IMG3[0][c]
	for (int c = 0; c < col; c++)
	{
		T[0][c][0] = T[0][c][1] = T[0][c][2] = 0;
		for (int r = 0; r < wV[0]; r++)
		{
			for (int k = 0; k < 3; k++) T[0][c][k] += IMG2[r][c][k];
		}
		for (int k = 0; k < 3; k++) IMG3[0][c][k] = T[0][c][k] / wV[0];
	}

	//vertical blur
	for (int c = 0; c < col; c++)
	{
		for (int r = 1; r < row; r++)
		{
			unsigned char L[3], R[3];

			if (r - br - 1 < 0) L[0] = L[1] = L[2] = 0;
			else for (int k = 0; k < 3; k++) L[k] = IMG2[r - br - 1][c][k];

			if (r + br >= row) R[0] = R[1] = R[2] = 0;
			else for (int k = 0; k < 3; k++) R[k] = IMG2[r + br][c][k];

			for (int k = 0; k < 3; k++)
			{
				T[r][c][k] = T[r - 1][c][k] - L[k] + R[k];
				IMG3[r][c][k] = T[r][c][k] / wV[r];
			}
		}
	}

	//overwrite the "blur" part from IMG3 to the original image:
	for (int r = 0; r < row; r++)
		for (int c = 0; c < col; c++)
			if (impre.at<Vec3b>(r, c).val[0] == 0 && impre.at<Vec3b>(r, c).val[1] == 0 && impre.at<Vec3b>(r, c).val[2] == 0)
				for (int k = 0; k < 3; k++) image.at<Vec3b>(r, c).val[k] = IMG3[r][c][k];


	bool isSuccess = imwrite("D:/output.png", image); //write the image to a file as PNG 
	//bool isSuccess = imwrite("D:/output.jpg", image); //write the image to a file as JPG
	if (isSuccess == false)
	{
		cout << "Failed to save the image" << endl;
		cin.get(); //wait for a key press
		return -1;
	}
	cout << "Image is succusfully saved to a file. Check out D:/output.png." << endl;

	String windowName = "D:/output.png"; //name of the window
	namedWindow(windowName); //create a window
	imshow(windowName, image); //show our image inside the created window

	waitKey(0); // Wait for any keystroke in the window

	destroyWindow(windowName); //destroy the created window

	return 0;
}