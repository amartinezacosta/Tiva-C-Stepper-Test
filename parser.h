#ifndef __GCODE_H__
#define __GCODE_H__
#include <stdint.h>
#include <stdbool.h>

#include "config.h"


#define BIT(n)							(1 << n)
#define BIT_FALSE(n, mask)				((n & mask) == 0)
#define BIT_TRUE(n, mask)				((n & mask) != 0)
#define WORD_EXIST(n, word)				((n & BIT(word)))
#define AXIS_EXIST(n)					((n & (BIT(X)|BIT(Y)|BIT(Z))))
#define ARC_OFFSETS_EXIST(n)			((n & (BIT(I)|BIT(J)|BIT(K))))

#define CHECK(n, group)\
if(BIT_TRUE(n, BIT(group))){return REPEATED_COMMAND;}\
else{n |= group; }\

#define IS_NEGATIVE(n)\
if(n < 0){return NEGATIVE_VALUE_ERROR;}\

#define COMMAND_NOT_SUPPORTED			-1
#define NEGATIVE_VALUE_ERROR			-2
#define NO_COMMANDS_FOUND				-3
#define REPEATED_COMMAND				-4
#define PARSER_OK						 1
#define FILE_BEGIN						 2

//G M commands groups
#define GROUP_1							 0
#define GROUP_2							 1
#define GROUP_3							 2
#define GROUP_4							 3
#define GROUP_5							 4
#define GROUP_6							 5
#define GROUP_7							 6
#define GROUP_8							 7
#define GROUP_9							 8
#define GROUP_10						 9
#define GROUP_11						 10
#define GROUP_12						 11

//Input letter tracking
#define X								 0
#define Y								 1
#define Z								 2
#define I								 3
#define J								 4
#define K								 5
#define F								 6
#define R								 7
#define S								 8
#define P								 9
#define T								 10

//-----------------G MODAL GROUP ------------------//
// Group 1: motion flags
#define RAPID_POSITIONING				 0
#define LINEAR_INTERPOLATION			 1
#define CIRCULAR_INTERPOLATION_CW		 2
#define CIRCULAR_INTERPOLATION_CCW		 3
#define STRAIGHT_PROBE					 4
#define CANCEL_MOTION_MODE				 5

// Group 2: plane selection flags
#define XY_PLANE						 0
#define XZ_PLANE						 1
#define YZ_PLANE						 2

//Group 3: distance flags
#define ABSOLUTE_DISTANCE				 0
#define RELATIVE_DISTANCE				 1

//Group 5: feedrate flags
#define UNITS_PER_MIN					 0
#define	INVERSE_TIME					 1

//Group 6: units flags
#define MILIMITERS			 			 0
#define INCHES							 1

//Group 8: Tool Lenght offset flags
#define TOOL_LENGTH_OFFSET				 0
#define CANCEL_TOOL_LENGHT_OFFSET		 1

//-----------------M MODAL GROUP-----------------------//
//Group 4: stop flags, group 4
#define PROGRAM_RUNNING					 0
#define OPTTIONAL_PROGRAM_STOP			 1
#define PROGRAM_END						 2

//Group 6: tool change flag
#define TOOL_CHANGE						 0

//Group 7: spindle flags
#define TURN_SPINDLE_CW			 		 3
#define TURN_SPINDLE_CCW	 			 2
#define STOP_SPINDLE					 1

//Group 8: coolant flags
#define FLOOD_COOLANT_ON				 1
#define FLOOD_COOLANT_OFF				 0

//Group 9: enable/disable feed and speed overrides switches flags
/*#define ENABLE_OVERRIDES				 1
#define DISABLE_OVERRIDES				 2
 */

//--------------NON MODAL CODES--------------------------//
//Group 0: non_modal G-codes
#define DWELL							 1
#define COORDINATE_SYSTEM_ORIGIN_SET	 2
#define RETURN_HOME						 3

typedef struct {
	  uint8_t motion;          // {G0,G1,G2,G3,G38.2,G80}
	  uint8_t feed_rate;       // {G93,G94}
	  uint8_t units;           // {G20,G21}
	  uint8_t distance;        // {G90,G91}
	  uint8_t plane_select;    // {G17,G18,G19}
	  uint8_t tool_length;     // {G43,G49}
	  uint8_t coord_select;    // {G54,G55,G56,G57,G58,G59}
	  uint8_t program_flow;    // {M0,M1,M2,M30}
	  uint8_t coolant;         // {M7,M8,M9}
	  uint8_t spindle;         // {M3,M4,M5}
	  uint8_t tool_select;
}Modal;

typedef struct{
	float xyz[3];	//x, y, z coordinates
	float ijk[3];   //i, j, k arc offset coordinates
	float f;		//feedrate value
	float r;		//radius for arc generation
	float s;		//spindle speed
	float p;		//dwell time
	uint8_t t;		//select tool
	uint8_t axis[3]; //axis selection value
}Values;

typedef struct{
	uint8_t value_flags;
	uint16_t command_flags;
}Flags;

typedef struct{
	uint8_t non_modal;
	Modal modal;
	Values value;
	Flags flags;
}Block;


int8_t parse_gcode(const char *data, Block *block);


#endif // __CMDLINE_H__
