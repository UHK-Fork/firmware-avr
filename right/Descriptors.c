/** \file
 *
 *  Main file for USB device descriptors.
 */

#include "Descriptors.h"

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
    if (Wormhole->EnumerationMode == ENUMERATION_MODE_Keyboard) {
            return CALLBACK_USB_GetKeyboardDescriptor(wValue, wIndex, DescriptorAddress);
    } else if (Wormhole->EnumerationMode == ENUMERATION_MODE_USBtoSerial) {
            return CALLBACK_USB_GetUSBtoSerialDescriptor(wValue, wIndex, DescriptorAddress);
    }
    return 0;
}
