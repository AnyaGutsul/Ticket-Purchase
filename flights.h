/*Standrat includes*/
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

/* flights.h */

#ifndef FLIGHTS_H_
#define FLIGHTS_H_

/* Number of flights in the system  */
#define FLIGHT_COUNT 20

/* Number of rows of seats on each plane */
#define ROW_COUNT 40

/* Number of seats in each row */
#define SEATS_PER_ROW 8

/* Maximum number of seats that could be allocated in a single request
 * (you need not expect more than this many seats in a single request) */ 
#define MAX_SEATS 6

/* Maximum number of passengers */
#define MAX_PASS_NUM FLIGHT_COUNT * ROW_COUNT * SEATS_PER_ROW

/* fly struct for saving fly-details */
typedef struct fly {
	int flightNum;
	int matrix[ROW_COUNT][SEATS_PER_ROW];
	int numOfCustomersPerSeat[ROW_COUNT][SEATS_PER_ROW];
	int OccupatedSeats;
	struct fly *next;
} fly;

/* function declaration  */
void* BuyTickets(void* arg);
fly* seekForFlight(int flightNum);
int checkSeatsInFlight(fly* temp, int seatsNum);

#endif /* FLIGHTS_H_ */
