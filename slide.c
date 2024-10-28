#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define WINDOW_SIZE 4
#define MAX_SEQ 7
#define TIMEOUT 3

typedef struct
{
    int seq;
    char data[50];
} Frame;

Frame window[WINDOW_SIZE];
int ack[MAX_SEQ + 1];
int base = 0, next_seq_num = 0;
int timer[WINDOW_SIZE];

void start_timer(int i)
{
    timer[i] = TIMEOUT;
}

void stop_timer(int i)
{
    timer[i] = 0;
}

int is_timeout(int i)
{
    return timer[i] == 0;
}

void decrement_timers()
{
    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        if (timer[i] > 0)
            timer[i]--;
    }
}

void send_frame(int seq, const char *data)
{
    Frame frame;
    frame.seq = seq;
    strncpy(frame.data, data, sizeof(frame.data) - 1);
    frame.data[sizeof(frame.data) - 1] = '\0';
    window[seq % WINDOW_SIZE] = frame;

    printf("Sending Frame: %d, Data: %s\n", frame.seq, frame.data);
    start_timer(seq % WINDOW_SIZE);
}

void receive_ack(int seq)
{
    printf("Received Ack: %d\n", seq);
    ack[seq] = 1;
    if (seq == base)
    {
        while (ack[base])
        {
            stop_timer(base % WINDOW_SIZE);
            base++;
        }
    }
}

void simulate_receiver(int seq)
{
    printf("Simulating receiver for Frame: %d\n", seq);
    receive_ack(seq);
}

void resend_frame(int i)
{
    printf("Resending Frame: %d, Data: %s\n", window[i].seq, window[i].data);
    start_timer(i);
}

int main()
{
    memset(ack, 0, sizeof(ack));
    memset(timer, 0, sizeof(timer));

    char data[8][50] = {"Frame0", "Frame1", "Frame2", "Frame3", "Frame4", "Frame5", "Frame6", "Frame7"};
    int total_frames = 8;
    int sent = 0;

    srand(time(NULL));

    // Continue until all frames are acknowledged
    while (base < total_frames)
    {
        // Send frames while there is room in the window and frames to send
        while ((next_seq_num != (base + WINDOW_SIZE) % (MAX_SEQ + 1)) && (sent < total_frames))
        {
            send_frame(next_seq_num, data[sent]);
            sent++;
            next_seq_num = (next_seq_num + 1) % (MAX_SEQ + 1);
        }

        decrement_timers();

        // Resend any frames that have timed out
        for (int i = 0; i < WINDOW_SIZE; i++)
        {
            if (timer[i] == 0 && ack[window[i].seq] == 0)
            {
                resend_frame(i);
            }
        }

        Sleep(1000);

        // Simulate receiving an acknowledgment for the base frame
        if (rand() % 2)
        {
            simulate_receiver(base);
        }
    }

    printf("All frames sent and acknowledged.\n");
    return 0;
}
