/* clkint.c - clkint */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <io.h>
#include <math.h>
/*------------------------------------------------------------------------
 *  clkint  --  clock service routine
 *  called at every clock tick and when starting the deferred clock
 *------------------------------------------------------------------------
 */
extern int timer;
INTPROC clkint(mdevno)
int mdevno;				/* minor device number		*/
{
	int	i;
    int resched_flag;

        
	tod++;

        resched_flag = 0;
	if (slnempty)
		if ( (--*sltop) <= 0 )
                     {
                        resched_flag = 1;
			wakeup();
                     } /* if */
	if ( (--preempt) <= 0 )
             resched_flag = 1;
    if (tod > 18) {
        tod = 0;
        timer = 1;
    }
    if (resched_flag == 1)
 	    resched();

}

