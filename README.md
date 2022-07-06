# Ticket-Purchase.
## Multithreaded Flight Ticket Purchase System.

### Description:
- There are several flights, each with several seats. 
Given a file of requests for seats on the flights: each request is for a number of seats (between 1 and 6) on a particular flight.

- To satisfy a request, by using the seat map of the flight, check where there are available number of seats as required.
Choosing the seats requires time, which is simulated by reading the appropriate usleep() function.
If the seats requested are still available, they are registered on customer's name and marked as occupied.
If, on the other hand, there are among the seats he chose to obtain were taken by other customers (in time
he has been thinking about which one to choose), he must try again. 
If there are not enough places left available on the selected flight, the request cannot be filled out.

- Clients operate independently of each other, so it is appropriate to implement each of them in their own thread.
There may be many situations of customers competition for seats, therefore the system is equipped with a mechanism that prevent it.
Most customers should get, in the end, the number of seats they have requested.
In addition, two customers should not receive the same seat on the same flight.


