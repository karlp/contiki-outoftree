#ifndef BLIPPER_H
#define BLIPPER_H

struct blipper_info {
	clock_time_t period;
	char id;
};

void blipper_start(struct blipper_info *blinfo);
void blipper_stop(void);
void blipper2_start(struct blipper_info *blinfo);
void blipper2_stop(void);

#endif
