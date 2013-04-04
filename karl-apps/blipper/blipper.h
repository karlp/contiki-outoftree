#ifndef BLIPPER_H
#define BLIPPER_H

struct blipper_info {
	clock_time_t period;
	char id;
};

PROCESS_NAME(blipper_process);
PROCESS_NAME(blipper2_process);

#endif
