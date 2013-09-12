/*
Functions for handling fifo buffer.
Atomic blocks allow writing or reading buffers in ISR's, however:
	you cannot write to the same buffer from both ISR's and main code (this will corrupt the head pointer)
	you cannot read from the same buffer with both ISR's and main code (this will corrupt the tail pointer)

Functions are passed a pointer to a wait function 'void (*wait_fptr)(void)' , if this pointer is passed as NULL
then the function will simply loop while waiting, otherwise it will call the pointer repeatedly while waiting.
The function pointer can be used for releasing the thread in a cooperative multi-thread system, or can be used for error trapping buffer underflow/overflow events.

While writing,
if the source data is larger than the buffer size, the write function will fragment the write, waiting while the buffer is full,
otherwise it will wait until the buffer has room to write the source data in one call.

NOTE, there is a potential deadlock situation,
	thread1 is waiting for fifo to have X bytes free before writing X bytes
	thread2 is waiting for fifo to have X bytes in it before reading X bytes
	to prevent this, buffer must be at least 2X

*/


	struct fifo_struct
    {
		volatile unsigned int bytes_free;	// ensure atomic access to these
		volatile unsigned int bytes_used;
		volatile char empty;				// these may seem excessive, but they don't require
		volatile char full;					// disabling of int's for atomic access (because they are only 1 byte).
		char* volatile head_ptr;
		char* volatile tail_ptr;
		char* volatile start;
		char* volatile end;
		void(*post_fptr)(void);		// post write function pointer, called after a write if not NULL, may be used to enable uart ISR's
		void (*wait_fptr)(void);	// function pointer called repeatedly while waiting for data to read, or space to write (if not NULL)
    };

void fifo_copy(struct fifo_struct *target, struct fifo_struct *source);

void* fifo_read(struct fifo_struct*, void* item, unsigned int size);
void* fifo_preview(struct fifo_struct*, void* item, unsigned int size);
void fifo_write(struct fifo_struct*, void* item, unsigned int size);

void fifo_init(struct fifo_struct *, unsigned int size, void(*post_fptr)(void), void(*wait_fptr)(void), void* data_space);

void fifo_flush(struct fifo_struct *);

char fifo_read_char(struct fifo_struct *);
int  fifo_read_int(struct fifo_struct *);
long fifo_read_long(struct fifo_struct *);
char* fifo_read_string(struct fifo_struct *, char *, int);

void fifo_write_char(struct fifo_struct *, char);
void fifo_write_int(struct fifo_struct *, int);
void fifo_write_long(struct fifo_struct *, long);

void fifo_write_string(struct fifo_struct *obj, char*string);
