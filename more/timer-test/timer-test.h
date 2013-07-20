/* 
 * File:   timer-test.h
 * Author: karlp
 *
 * Created on June 29, 2013, 2:17 PM
 */

#ifndef TIMER_TEST_H
#define	TIMER_TEST_H

#ifdef	__cplusplus
extern "C" {
#endif

PROCESS_NAME(test_clock_wait);
void test_etimer_init(int i);
void test_clock_usecs_init(int i);


#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_TEST_H */

