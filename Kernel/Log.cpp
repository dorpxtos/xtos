#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <Utf8.h>
#include <TextMode.h>
#include <Bochs.h>
#include <CoreMods.h>
#include <Serial.h>
#include <Device.h>
#include <Driver.h>
#include <Io.h>
#include <Video.h>
#include <Log.h>

#define ITOA_BUFFER_LENGTH 64

#define LOG_TEXTMODE 1
#define LOG_BOCHS 1
#define LOG_SERIAL 0
#define LOG_VIDEO 1

void LogPrintChar(utf8_int32_t c) {
#if LOG_TEXTMODE
	if (useTextMode) {
		TextmodePrintChar(c);
	}
#endif

#if LOG_BOCHS
	BochsPrintChar(c);
#endif

#if LOG_SERIAL
	if (useSerial) {
		SerialPrintChar(c);
	}
#endif

#if LOG_VIDEO
	if (useVideoLog) {
		VideoLogPrintChar(videoLog, c);
	}
#endif
}

static void LogPrintString(char* message) {
	utf8_int32_t c;
	while (*message) {
		utf8codepoint(message, &c);
		LogPrintChar(c);
		message++;
	}
}

void LogPrint(char* format, ...) {
	va_list args;
	va_start(args, format);

	bool inFormatting = false;
	bool gettingRepeatChar = false;
	char itoaBuffer[ITOA_BUFFER_LENGTH];
	int repeatTimes = 0;
	char repeatChar = 0;
	int r;
	utf8_int32_t uc;

	// Loop through every character
	while (*format) {
		// Read codepoint
		format = (char*)utf8codepoint(format, &uc);

		// If in formatting
		if (inFormatting) {
			// If getting the repeat character
			if (gettingRepeatChar) {
				repeatChar = uc;
				gettingRepeatChar = false;
			}
			// If just formatting
			else {
				switch (uc) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					repeatTimes = uc - '0';
					gettingRepeatChar = true;
					break;

				// String
				case 's':
					LogPrintString(va_arg(args, char*));
					inFormatting = false;
					break;

				// Character
				case 'c':
					LogPrintChar(va_arg(args, char));
					inFormatting = false;
					break;

				// Binary
				case 'b':
					itoa(va_arg(args, int), itoaBuffer, 2);
					r = repeatTimes - strlen(itoaBuffer);
					if (r < 0) r = 0;
					for (int i = 0; i < r; i++) {
						LogPrintChar(repeatChar);
					}
					LogPrintString(itoaBuffer);
					inFormatting = false;
					break;

				// Octal
				case 'o':
					itoa(va_arg(args, int), itoaBuffer, 8);
					r = repeatTimes - strlen(itoaBuffer);
					if (r < 0) r = 0;
					for (int i = 0; i < r; i++) {
						LogPrintChar(repeatChar);
					}
					LogPrintString(itoaBuffer);
					inFormatting = false;
					break;

				// Decimal
				case 'd':
					itoa(va_arg(args, int), itoaBuffer, 10);
					r = repeatTimes - strlen(itoaBuffer);
					if (r < 0) r = 0;
					for (int i = 0; i < r; i++) {
						LogPrintChar(repeatChar);
					}
					LogPrintString(itoaBuffer);
					inFormatting = false;
					break;

				// Hex
				case 'x':
					itoa(va_arg(args, int), itoaBuffer, 16);
					r = repeatTimes - strlen(itoaBuffer);
					if (r < 0) r = 0;
					for (int i = 0; i < r; i++) {
						LogPrintChar(repeatChar);
					}
					LogPrintString(itoaBuffer);
					inFormatting = false;
					break;

				// Hex (Uppercase)
				case 'X':
					itoa(va_arg(args, int), itoaBuffer, 16);
					r = repeatTimes - strlen(itoaBuffer);
					if (r < 0) r = 0;
					for (int i = 0; i < r; i++) {
						LogPrintChar(repeatChar);
					}
					strupr(itoaBuffer);
					LogPrintString(itoaBuffer);
					inFormatting = false;
					break;

				// Unknown
				default:
					LogPrintString("(?)");
					inFormatting = false;
					break;
				}
			}
		}
		// If not in formatting
		else {
			switch (uc) {
			case '%':
				// Clear buffers
				memset(itoaBuffer, 0, ITOA_BUFFER_LENGTH);

				// Reset variables
				repeatTimes = 0;

				// Switch to formatting mode
				inFormatting = true;
				break;
			default:
				LogPrintChar(uc);
				break;
			}
		}
	}

	LogPrintChar('\n');

	va_end(args);
}

DriverObject* logDriver;
DeviceObject* logDevice;

IoStatus LogDevWrite(IoStack* req) {
	for (int i = 0; i < req->parameters.write.count; i++) {
		LogPrintChar(req->parameters.write.buffer[i]);
	}

	return IOSTATUS_SUCCESS;
}

void LogDevInit() {
	// Create device and driver
	DriverObCreate("Log", "Log Driver", &logDriver);
	IoCreateDevice("Log", DEVICE_TYPE_PIPE, 0, logDriver, &logDevice);
	DriverObRegisterFunction(logDriver, DRIVER_FUNCTION_WRITE, &LogDevWrite);

	// Test driver
	IoRequest* ir = IoCreateRequest();
	ir->stack.majorFunction = DRIVER_FUNCTION_WRITE;
	ir->stack.parameters.write.buffer = (uint8_t*)"LogDev Test\n";
	ir->stack.parameters.write.count = strlen((const char*)ir->stack.parameters.write.buffer);
	IoCallDriver(logDevice, ir);

	// Done
	LogPrint("LOGDEV");
}