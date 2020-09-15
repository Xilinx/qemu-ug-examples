/*
 * Copyright 2020, Xilinx Inc
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *    * The above copyright notice and this permission notice shall be included
 *      in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Copied from the Linux Kernel.
 *
 * Example of a message structure:
 *   0000  ff 8f 00 00 00 00 00 00      monotonic time in nsec
 *   0008  34 00                        record is 52 bytes long
 *   000a        0b 00                  text is 11 bytes long
 *   000c              1f 00            dictionary is 23 bytes long
 *   000e                    03 00      LOG_KERN (facility) LOG_ERR (level)
 *   0010  69 74 27 73 20 61 20 6c      "it's a l"
 *         69 6e 65                     "ine"
 *   001b           44 45 56 49 43      "DEVIC"
 *         45 3d 62 38 3a 32 00 44      "E=b8:2\0D"
 *         52 49 56 45 52 3d 62 75      "RIVER=bu"
 *         67                           "g"
 *   0032     00 00 00                  padding to next message header
 */
#define LOGBUF_MSG_MIN_LEN			16
struct logbuf_msg {
	unsigned long long m_time;
	unsigned short record_len;
	unsigned short text_len;
	unsigned short dictionary_len;
	unsigned short flags;
	unsigned char* text;
	unsigned char* dict;
};

struct logbuf_msg_state {
	struct logbuf_msg msg;
	char* raw_buffer;
	unsigned int len;
	unsigned int current_read;
};

int msg_read(int fd, struct logbuf_msg* state);

#define WORD_ALIGN(x)	(((((x) % 4) == 0) ? ((x) / 4) : (((x) / 4) + 1)) * 4)
//#define DEBUG
#ifdef DEBUG
#define DEBUGF(x, ...)	printf(x, __VA_ARGS__)
#else
#define DEBUGF(x, ...)	
#endif

struct logbuf_msg buffer[1024];

int main(void)
{
	int fd = 0;
	struct logbuf_msg* msg;
	int ret = 0;
	size_t count = 0;

	while (1) {
		msg = &(buffer[count]);
		memset(msg, 0, sizeof(struct logbuf_msg));
		ret = msg_read(fd, msg);
		if (ret != -1) {
			count++;
		} else {
			break;
		}
	}

	int i;
	for (i = 0; i < count; i++) {
		printf("%s\n", buffer[i].text);
	}

	return 0;
}

int msg_read(int fd, struct logbuf_msg* state)
{
	if (state == NULL || fd < 0) {
		return -1;
	}

	char buffer[1024];

	/* read the header */
	int ret = read(fd, buffer, LOGBUF_MSG_MIN_LEN);
	if (ret >= 0 && ret == LOGBUF_MSG_MIN_LEN) {
		state->m_time = *((unsigned long long*)buffer);
		state->record_len = *((unsigned short*)(buffer+8));
		state->text_len = *((unsigned short*)(buffer+10));
		state->dictionary_len = *((unsigned short*)(buffer+12));
		state->flags = *((unsigned short*)(buffer+14));

		/* check endian, only need to swap when host/target
		 * has a mis-match, e.g. x86->micrblaze be. */
		if (state->text_len > state->record_len) {
			unsigned short temp = state->text_len;
			state->text_len = state->record_len;
			state->record_len = state->text_len;
			temp = state->dictionary_len;
			state->dictionary_len = state->flags;
			state->flags = state->dictionary_len;
			/* TODO time */
		}

		DEBUGF("logbuf_msg (read %d bytes)\n", ret);
		DEBUGF("\tstate->m_time = %016llx\n", state->m_time);
		DEBUGF("\tstate->record_len = %08x\n", state->record_len);
		DEBUGF("\tstate->text_len = %08x\n", state->text_len);
		DEBUGF("\tstate->dictionary_len = %08x\n", state->dictionary_len);
		DEBUGF("\tstate->flags = %08x\n", state->flags);
	} else {
		fprintf(stderr, "Corrupt logbuf, output may be incorrect\n");		
		return -1;
	}

	/* read the rest */
	size_t expected_read = WORD_ALIGN(state->record_len) - LOGBUF_MSG_MIN_LEN;
	ret = read(fd, buffer, expected_read);
	if (ret >= 0 && ret == expected_read) {
		DEBUGF("\t\tread buffer, size = %d\n", ret);
		size_t index = 0;
		if (state->text_len > 0) {
			state->text = malloc((state->text_len + 1) * sizeof(char));
			memset(state->text, 0, (state->text_len + 1) * sizeof(char));
			memcpy(state->text, &(buffer[index]), state->text_len);
			index += state->text_len;
			DEBUGF("\t\t\tstate->text = %s\n", state->text);
		} else {
			state->text = NULL;
		}
		if (state->dictionary_len > 0) {
			state->dict = malloc((state->dictionary_len + 1) * sizeof(char));
			memset(state->dict, 0, (state->dictionary_len + 1) * sizeof(char));
			memcpy(state->dict, &(buffer[index]), state->dictionary_len);
			index += state->dictionary_len;
			DEBUGF("\t\t\tstate->dict = %s\n", state->dict);
		} else {
			state->dict = NULL;
		}
	} else {
		fprintf(stderr, "Corrupt logbuf, output may be incorrect\n");
		return -1;
	}
	return 0;
}

