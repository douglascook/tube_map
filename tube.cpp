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

// Strip first two characters from line of text to leave name of the station
void get_station_name(char *line)
{
	for (int i = 0; line[i]; i++){
		line[i] = line[i+2];
	}
}
	
// Search file for given string and return corresponding symbol
bool find_symbol_matching_name(const char *file, const char *name, char &symbol)
{	
	ifstream i_stream;
	// 100 characters is sufficient for any line in the text files
	char line[100];

	i_stream.open(file);

	while (!i_stream.eof()){
		i_stream.getline(line,100);
		symbol = line[0];
		get_station_name(line);	

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
	
	while (!i_stream.eof()){
		i_stream.getline(line,100);

		if (line[0] == symbol){
			get_station_name(line);
			strcpy(station, line);
			return true;
		}
	}
	return false;
}

// create array based on route string and return the number of directions involved
int create_direction_array(const char *route, Direction direction_array[])
{
	int index = 0, position = 0, direction_count = 0; 

	while (route[position] != '\0'){
		char direction_string[] = {'\0','\0','\0'};
		int direction_length = 0;
		
		// strip out next direction from the route
		for (int i = 0; (route[position + i] != ',') && (route[position + i] != '\0'); i++){
			direction_string[i] = route[position + i];
			direction_length++;
		}
		position += direction_length;

		// want to skip over commas to start at next direction unless we are at end of the route
		if (route[position] == ','){
			position++;
		}

		direction_array[index] = string_to_direction(direction_string);
		direction_count++;
		index++;
	}
	return direction_count;
}

// Translate direction into coordinate change
void coord_change(Direction direction, int& row_change, int& column_change)
{
	switch (direction){
		cout << "direction: " << direction << endl;
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
			// already tested for invalid directions so will not reach here 
				;	
	}
}

// Question 1
// Find first occurrence of target symbol, return true if found and set coordinates r and c
bool get_symbol_position(char **map, int height, int width, char target, int& r, int& c)
{
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			if (map[i][j] == target){
				r = i;
				c = j;
				return true;
			}
		}
	}
	return false;
}

// Question 2
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
	// return ' ' as the default if there is no match
	return return_symbol;
}

// Question 3
// Check whether given route from start to destination is valid and return number of changes required
int validate_route(char **map, int height, int width, const char *start_station, const char *route, char *destination)
{
	char prev_symbol, current_line, current_symbol = get_symbol_for_station_or_line(start_station);	
	int direction_count, row, column, prev_row, prev_column, row_change, column_change, changes = 0;
	Direction direction, direction_array[100];
 
	// if the symbol is alphanumeric then it is a station so find corresponding station and set coords 
	if (isalnum(current_symbol)){
		get_symbol_position(map, height, width, current_symbol, row, column);
	}else{
	// otherwise return not a valid station error
		return -1; 
	}

	direction_count = create_direction_array(route, direction_array);
	// check for any invalid directions (treating empty route string as a valid route that goes nowhere)
	for (int i = 0; i < direction_count; i++){
		if (direction_array[i] == 8)
			return -5;
	}	
	
	for (int i = 0; i < direction_count; i++){
		direction = direction_array[i];

		// set row and column change variables so we can update the map position
		coord_change(direction, column_change, row_change);

		// check for back tracking
		// at this point 'previous' variables are still holding data about the point before last
		// so compare the next point with point before last to see if it matches
		if ((row + row_change == prev_row) && (column + column_change == prev_column) && (prev_symbol == current_symbol)){
			return -4;
		}
			
		// now update the current coords
		prev_row = row;
		prev_column = column;
		row += row_change;
		column += column_change;
		
		// check for out of bounds
		if (row < 0 || row > 29 || column < 0 || column > 76){
			return -7;
		}

		prev_symbol = current_symbol;
		current_symbol = map[row][column];

		// check for off track
		if (current_symbol == ' '){
			return -6;
		}

		// check for line hopping between stations
		// if neither previous nor current point is station and they are on different lines
		if (!isalnum(prev_symbol) && !isalnum(current_symbol) && (current_symbol != prev_symbol)){
			return -3;
		}

		// check if we have changed line and increment count if needed 
		// started changes at -1 so starting line doesn't count as a change
		if (!isalnum(current_symbol) && current_symbol != current_line){
			current_line = current_symbol;
			if (i != 0)
				changes++;
		}
	}	

	// check that final point is a station
	if (!find_station_matching_symbol(current_symbol, destination)){
		return -2;
	}

	return changes;
}
