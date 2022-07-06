#include "flights.h"

sem_t mutex[FLIGHT_COUNT];
fly *head = NULL;
pthread_t *passengers;

int order[MAX_PASS_NUM][3];
int satisfiedCnt = 0;
int unsatisfiedCnt = 0;
int unsatisfiedArr[MAX_PASS_NUM];
int seats = ROW_COUNT * SEATS_PER_ROW;

int main(int argc, char* argv[]) {
	FILE* fd1 = fopen(argv[1], "r+");
	FILE* fd2 = fopen(argv[2], "w+");
	fly *newNode, *temp;
	clock_t end, start = clock();
	double time_taken;
	int i = 0, j, k;
	int arg[MAX_PASS_NUM];
	int cnt1 = 0, cnt2 = 0;
	int click = 0;
	srand(time(NULL));

	if (argc != 3) exit(1); //check if we got 2 files in input
	if (!fd1 || !fd2) { //check if files are successfuly opened
		printf("Bad source");
		exit(1);
	}
	/* CREATE LIST */
	for (i = 0; i < FLIGHT_COUNT; i++) {
		sem_init(&mutex[i], 0, 1); //init semaphores for flights

		newNode = (fly*)malloc(sizeof(fly)); //memory allocation for newNode

		for (j = 0; j < ROW_COUNT; j++) { //fill matrix of seats on node with -1 and maxtrix of number customers per seat with 0
			for (k = 0; k < SEATS_PER_ROW; k++) {
				newNode->matrix[j][k] = -1;
				newNode->numOfCustomersPerSeat[j][k] = 0;
			}
		}
		newNode->OccupatedSeats = 0; //occupated seats counter for each fly
		newNode->next = NULL; //set next node to null instead of garbage
		newNode->flightNum = i; //set flight number
		if (head == NULL)
			head = newNode; //set this node head if head was null
		else { //insert into beginning
			temp = head; //save head
			head = newNode; //make newNode head
			newNode->next = temp; //insert to the head
		}
	}

	/*	READ DATA */
	while (fscanf(fd1, "%d %d %d", &order[click][0], &order[click][1], &order[click][2]) > 0) {
		click++; //read all data
	}
	fclose(fd1); //close file requests

				 /* ALLOCATE MEMORY */
	if (!(passengers = malloc(click * sizeof(pthread_t)))) { //allocate memory for threads
		printf("Can't allocate memory for passengers");
		exit(1);
	}

	/* CREATE THREADS */
	for (i = 0; i < click; i++) {
		arg[i] = i; //save arguments for sending
		pthread_create(&passengers[i], NULL, BuyTickets, (void*)&arg[i]);
	}

	for (i = 0; i < click; i++) { //join all the threads
		pthread_join(passengers[i], NULL);
	}

	/* OUTPUTS */
	temp = head; //get head of list to temp
	for (i = 0; i < FLIGHT_COUNT; i++) { //for each flight
		fprintf(fd2, "Flight #%d: Free seats = %d\n", temp->flightNum, seats - temp->OccupatedSeats);
		for (j = 0; j < ROW_COUNT; j++) { //for each row
			for (k = 0; k < SEATS_PER_ROW; k++) { //for each seat
				click = j * SEATS_PER_ROW + k + 1; //formula for getting exact seat number
				if (temp->matrix[j][k] == -1 && click <= temp->OccupatedSeats) {
					fprintf(fd2, "Flight#%d Seat:[%d][%d] were allocated but free\n", temp->flightNum, j, k);
					cnt1++;       //counter for number of allocated seats but free 
				}
				if (temp->numOfCustomersPerSeat[j][k] > 1) {
					fprintf(fd2, "Flight#%d Seat:[%d][%d] were allocated for %d passengers\n", temp->flightNum, j, k, temp->numOfCustomersPerSeat[j][k]);
					cnt2++;       //counter for seats that were allocated for more than one person
				}
			}
		}
		temp = temp->next;
	}

	fprintf(fd2, "Allocated but free seats: %d\n", cnt1);
	fprintf(fd2, "Allocated for more than one person: %d\n", cnt2);

	for (i = 0; i < unsatisfiedCnt; i++) {
		fprintf(fd2, "Flight#%d Client#%d Asked for %d seats, but was unsatisfied\n", order[unsatisfiedArr[i]][1], order[unsatisfiedArr[i]][0], order[unsatisfiedArr[i]][2]);
	}

	printf("%d clients were satisfied\n", satisfiedCnt);
	fprintf(fd2, "%d clients were satisfied\n", satisfiedCnt);
	fprintf(fd2, "%d clients were unsatisfied\n", unsatisfiedCnt);

	/* FREE MEMORY */
	free(passengers);

	temp = head;
	newNode = temp->next;
	while (newNode != NULL) {
		free(temp);
		temp = newNode;
		newNode = temp->next;
	}

	/* STOP TIMER */
	end = clock(); //set end timer
	time_taken = ((double)(end - start)) / CLOCKS_PER_SEC; //count time taken
	fprintf(fd2, "All the program took %f seconds to execute.\n", time_taken); //output 

	fclose(fd2); //close output file
	return 0;
}

void* BuyTickets(void* arg) {  //buying tickets system
	int k = *(int*)arg;        //cast arg from void to int
	int i = 0, j = 0, h;
	int startSeat_before = 0, startSeat_after = 1;
	int seatsRequested = order[k][2];              //number of seats that passenger requested
	fly* f = seekForFlight(order[k][1]);           //get the flight node with all information
	int number = f->flightNum;                     //get number of requested flight

	while (startSeat_after != startSeat_before) {
		sem_wait(&mutex[number]);
		startSeat_before = checkSeatsInFlight(f, seatsRequested);    //save the first of booked seats
																	 //check if there are free seats in the flight and the number of requested seats is not out of bounds
		if (startSeat_before >= 0 && seatsRequested <= MAX_SEATS && seatsRequested > 0) {
			sem_post(&mutex[number]);
			usleep(rand() % 50000);                     //sleep for decision process
			sem_wait(&mutex[number]);
			startSeat_after = checkSeatsInFlight(f, seatsRequested);   //get first free seat in the flight
																	   //check if the proposed seats after decision process are the same to the proposial before  
			if (startSeat_after == startSeat_before) {
				i = startSeat_after / SEATS_PER_ROW;            //the tickets buying process
				j = startSeat_after % SEATS_PER_ROW;
				for (h = seatsRequested; h > 0; h--) {
					if (j < SEATS_PER_ROW)
						f->matrix[i][j] = order[k][0];
					else {
						j = 0;
						f->matrix[++i][j] = order[k][0];
					}
					f->numOfCustomersPerSeat[i][j]++;         //counter of persons per seat refresh
					j++;
					f->OccupatedSeats++; //f->OccupatedSeats refresh
				}
				break;
			}
		}
		else {
			unsatisfiedArr[unsatisfiedCnt++] = k; //counter of unsatisfied customers refresh//add the thread number to unsatisfied array		
			sem_post(&mutex[number]);
			pthread_exit(&passengers[k]);		//no more seats, thread out
		}
		sem_post(&mutex[number]);
	}
	sem_post(&mutex[number]);
	satisfiedCnt++;                                    //counter of satisfied customers refresh
	pthread_exit(&passengers[k]);		                   //passenger bought his tickets, thread out
}

fly* seekForFlight(int flightNum) { //move in the list to the node of the requested flight 
	fly *temp = head;
	if (temp != NULL) {
		while (flightNum != temp->flightNum) {
			temp = temp->next;
		}
	}
	return temp;
}

int checkSeatsInFlight(fly* temp, int seatsNum) { //seek for the first free seat if there is one
	fly *f = temp;
	if (f != NULL) {
		if ((seats - (f->OccupatedSeats + seatsNum)) >= 0)
			return f->OccupatedSeats; //the first free seat 
	}
	return -1;       //there are no free seats in flight
}
