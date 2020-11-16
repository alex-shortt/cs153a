// [ zero move, zero output, one move, one output ]

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6

int TURN_FSM[7][4] = {
// next state for 00, 01, 10, 11
	   {A, B, E, A}, // A
	   {C, B, B, A}, // B
	   {C, B, D, C}, // C
	   {C, D, D, A}, // D
	   {F, E, E, A}, // E
	   {F, G, E, F}, // F
	   {F, G, G, A}  // G
};

int move_turn_fsm(int cur_state_index, int input) {
	// input: 00 (0), 01 (1), 10 (2), or 11 (3)
	int next_state = TURN_FSM[cur_state_index][input];

	int output = 0;
	if(next_state == A && cur_state_index == D){
		output = 1;
	} else if (next_state == A && cur_state_index == G) {
		output = 2;
	}

	return (next_state << 4) + output;
}
