#ifndef TIME_H
#define TIME_H

// Time structure to retreive from the Real-Time Clock
struct rtc_time {
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
};

// Procedure to provide delay functionality
void delay(unsigned long u_sec);

// Function to get the current time
void gettime(struct rtc_time __far* current_time);

#endif
