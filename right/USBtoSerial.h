#ifndef __USB_TO_SERIAL_H__
#define __USB_TO_SERIAL_H__

    /* Includes: */
        #include <avr/io.h>
        #include <LUFA/Drivers/USB/USB.h>
        #include <LUFA/Drivers/Peripheral/Serial.h>
        #include <LUFA/Drivers/Misc/RingBuffer.h>
        #include "USBtoSerialDescriptors.h"
        #include "Keyboard.h"

    /* Macros: */
        #define USB_TO_SERIAL_BUFFER_SIZE 128

    /* Function Prototypes: */
        int USBtoSerialMainLoop(void);
        void USBtoSerialRxCallback(void);
        bool EVENT_USB_USBtoSerial_Device_ConfigurationChanged(void);
        void EVENT_USB_USBtoSerial_Device_ControlRequest(void);
        void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo);
        void CatchReenumerateRequest(void);

#endif
