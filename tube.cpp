#include <iostream>
#include <iomanip>
#include <fstream>
#include <cassert>
#include <cstring>
#include <cctype>
#include <cstdlib>

using namespace std;

#include "tube.h"

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* internal helper function which allocates a dynamic 2D array */
char **allocate_2D_array(int rows, int columns) {
  char **m = new char *[rows];
  assert(m);
  for (int r=0; r<rows; r++) {
    m[r] = new char[columns];
    assert(m[r]);
  }
  return m;
}

/* internal helper function which deallocates a dynamic 2D array */
void deallocate_2D_array(char **m, int rows) {
  for (int r=0; r<rows; r++)
    delete [] m[r];
  delete [] m;
}

/* internal helper function which gets the dimensions of a map */
bool get_map_dimensions(const char *filename, int &height, int &width) {
  char line[512];
  
  ifstream input(filename);

  height = width = 0;

  input.getline(line,512);  
  while (input) {
    if ( (int) strlen(line) > width)
      width = strlen(line);
    height++;
    input.getline(line,512);  
  }

  if (height > 0)
    return true;
  return false;
}

/* pre-supplied function to load a tube map from a file*/
char **load_map(const char *filename, int &height, int &width) {

  bool success = get_map_dimensions(filename, height, width);
  
  if (!success)
    return NULL;

  char **m = allocate_2D_array(height, width);
  
  ifstream input(filename);

  char line[512];
  char space[] = " ";

  for (int r = 0; r<height; r++) {
    input.getline(line, 512);
    strcpy(m[r], line);
    while ( (int) strlen(m[r]) < width )
      strcat(m[r], space);
  }
  
  return m;
}

/* pre-supplied function to print the tube map */
void print_map(char **m, int height, int width) {
  cout << setw(2) << " " << " ";
  for (int c=0; c<width; c++)
    if (c && (c % 10) == 0) 
      cout << c/10;
    else
      cout << " ";
  cout << endl;

  cout << setw(2) << " " << " ";
  for (int c=0; c<width; c++)
    cout << (c % 10);
  cout << endl;

  for (int r=0; r<height; r++) {
    cout << setw(2) << r << " ";    
    for (int c=0; c<width; c++) 
      cout << m[r][c];
    cout << endl;
  }
}

/* pre-supplied helper function to report the errors encountered in Question 3 */
const char *error_description(int code) {
  switch(code) {
  case ERROR_START_STATION_INVALID: 
    return "Start station invalid";
  case ERROR_ROUTE_ENDPOINT_IS_NOT_STATION:
    return "Route endpoint is not a station";
  case ERROR_LINE_HOPPING_BETWEEN_STATIONS:
    return "Line hopping between stations not possible";
  case ERROR_BACKTRACKING_BETWEEN_STATIONS:
    return "Backtracking along line between stations not possible";
  case ERROR_INVALID_DIRECTION:
    return "Invalid direction";
  case ERROR_OFF_TRACK:
    return "Route goes off track";
  case ERROR_OUT_OF_BOUNDS:
    return "Route goes off map";
  }
  return "Unknown error";
}

/* presupplied helper function for converting string to direction enum */
Direction string_to_direction(const char *token) {
  const char *strings[] = {"N", "S", "W", "E", "NE", "NW", "SE", "SW"};
  for (int n=0; n<8; n++) {
    if (!strcmp(token, strings[n])) 
      return (Direction) n;
  }
  return INVALID_DIRECTION;
}

// Find first occurrence of target symbol, return true if found and set coordinates r  
bool get_symbol_position(char **map, int height, int width, char target, int &r, int &c)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (map[i][j] == target){
				r = i;
				c = j;
				return true;
			}
		}
	}
	return false;
}

// Return symbol representing given station or line
char get_symbol_for_station_or_line(const char *name)
{
	char return_symbol = ' ';
	if (find_symbol_matching_name("stations.txt", name, return_symbol)){
		return return_symbol;
	}
	else if (find_symbol_matching_name("lines.txt", name, return_symbol)){
		return return_symbol;
	}
	// this returns ' ' as the default if there is no match
	return return_symbol;
}

// Search file for given string and return corresponding symbol
bool find_symbol_matching_name(const char *file, const char *name, char &symbol)
{	
	ifstream i_stream;
	// 100 characters are sufficient for any line in the text files
	char line[100];

	i_stream.open(file);

	while (!i_stream.eof())
	{
		i_stream.getline(line,100);
		symbol = line[0];
		// shift everything left by 2 here so we can use strcmp
		shift_left(line, 2);
		
		if (!strcmp(line, name)){
			i_stream.close();
			return true;
		}
	}
		
	i_stream.close();
	return false;
}

// Search station list for matching symbol and return corresponding name
bool find_station_matching_symbol(char symbol, char *station)
{
	ifstream i_stream;
	char line[100];
	i_stream.open("stations.txt");
	
	while (!i_stream.eof())
	{
		i_stream.getline(line,100);
		if (line[0] == symbol)
		{
			shift_left(line, 2);
			strcpy(station, line);
			cout << station[0] << station[1] << station[2];
			return true;
		}
	}
	
	return false;
}

// shift all elements of an array to the left by 2
void shift_left(char a[], int shift)
{
	// does this screw things up if we go out of bounds ie shift a 4 element array by 5???
	
	for (int i = 0; a[i]; i++)
	{
		a[i] = a[i + shift];
	}
}

// translate direction into coordinate change
void coord_change(Direction direction, int& row_change, int& column_change)
{
	switch (direction)
	{
		case N: row_change = 0; 
			column_change = -1;
			break;
		case S: row_change = 0; 
			column_change = 1;
			break;
		case W: row_change = -1; 
			column_change = 0;
			break;
		case E: row_change = 1; 
			column_change = 0;
			break;
		case NE: row_change = 1;
			 column_change = -1;
			break;
		case NW: row_change = -1; 
			 column_change = -1;
			break;
		case SE: row_change = 1; 
			 column_change = 1;
			break;
		case SW: row_change = -1; 
			 column_change = 1;
			break;
		case INVALID_DIRECTION:
			// using 2 as sentinel value so we can display error message
			row_change = 2;
	}
}

// Check whether given route from start to destination is valid and return number of changes required
int validate_route(char **map, int height, int width, const char *start_station, char *route, char *destination)
{
	char prev_symbol, current_line, current_symbol = get_symbol_for_station_or_line(start_station);	
	int row, column, prev_row, prev_column, row_change, column_change, changes = -1;

	// if the symbol is alphanumeric then it is a station so return its coords
	if (isalnum(current_symbol))
		get_symbol_position(map, height, width, current_symbol, row, column);
	else
	// otherwise return not a valid station error
		return -1; 


	// MAY WANT TO CREATE FUNCTION TO DO ALL OF THIS 
	while (route[0] != '\0') 
	{
		char direction[] = {'\0','\0','\0'};
		/*  below doesn't work for some reason
		char *direction = new char[2];
		direction[2] = '\0';

		delete [] direction; */
		

		// want to start count at one so the comma is stripped off too
		int count = 1;
		for (int i = 0; route[i]!=',' && route[i]!='\0'; i++)
		{
			direction[i] = route[i];
			count++;
		}
		// this takes off the direction from the array so next direction is ready
		if (route[0]!='\0')
			shift_left(route, count);
		
		// this should set row and column change variables
		coord_change(string_to_direction(direction), column_change, row_change);
		// 2 used as sentinel value to indicate invalid direction
		if (column_change == 2)
			return -5;
			
		// check for back tracking
		// at this point 'previous' variables are still holding data about the point before last
		// so we can compare the next point with point before last to see if it matches
		if ((row + row_change == prev_row) && (column + column_change == prev_column) && (prev_symbol == map[row + row_change][column + column_change]))
			return -4;
			
		// now update the current coords
		prev_row = row;
		prev_column = column;
		
		row += row_change;
		column += column_change;
		
		prev_symbol = current_symbol;

		// check for off track
		if (current_symbol == ' ')
			return -6;

		// check for out of bounds
		if (row < 0 || row > 29 || column < 0 || column > 76)
			return -7;

		current_symbol = map[row][column];

		// check line and count if changed
		if (!isalnum(current_symbol))
		{
			// started changes at -1 so first line doesn't count as a change
			if (current_symbol != current_line)
				changes++;

			current_line = current_symbol;
		}

		// check for line hopping between stations
		// if neither previous nor current point is station and they are on different lines
		if (!isalnum(prev_symbol) && !isalnum(current_symbol) && (current_symbol != prev_symbol))
			return -3;
	}	
	// check if final point is station
	if (!find_station_matching_symbol(current_symbol, destination))
		return -2;

	return changes;
}
