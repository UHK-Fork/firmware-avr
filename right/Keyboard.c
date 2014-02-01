/** \file
 *
 *  Main file for the keyboard function of the right keyboard half.
 */

#include "Keyboard.h"

KeyMatrix_t KeyMatrices[KEYMATRICES_NUM];

/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/** Buffer to hold the previously generated Mouse HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevMouseHIDReportBuffer[sizeof(USB_MouseReport_Data_t)];

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevHIDReportBuffer[GENERIC_REPORT_SIZE];

USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
    {
        .Config =
            {
                .InterfaceNumber              = INTERFACE_ID_Keyboard,
                .ReportINEndpoint             =
                    {
                        .Address              = KEYBOARD_IN_EPADDR,
                        .Size                 = HID_EPSIZE,
                        .Banks                = 1,
                    },
                .PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
                .PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
            },
    };

USB_ClassInfo_HID_Device_t Mouse_HID_Interface =
    {
        .Config =
            {
                .InterfaceNumber              = INTERFACE_ID_Mouse,
                .ReportINEndpoint             =
                    {
                        .Address              = MOUSE_IN_EPADDR,
                        .Size                 = HID_EPSIZE,
                        .Banks                = 1,
                    },
                .PrevReportINBuffer           = PrevMouseHIDReportBuffer,
                .PrevReportINBufferSize       = sizeof(PrevMouseHIDReportBuffer),
            },
    };

USB_ClassInfo_HID_Device_t Generic_HID_Interface =
    {
        .Config =
            {
                .InterfaceNumber              = INTERFACE_ID_GenericHID,
                .ReportINEndpoint             =
                    {
                        .Address              = GENERIC_IN_EPADDR,
                        .Size                 = GENERIC_EPSIZE,
                        .Banks                = 1,
                    },
                .PrevReportINBuffer           = PrevHIDReportBuffer,
                .PrevReportINBufferSize       = sizeof(PrevHIDReportBuffer),
            },
    };

const __flash KeyMatrixInfo_t KeyMatrixInfoLeft = {
    .ColNum = LEFT_COLS_NUM,
    .RowNum = ROWS_NUM,
    .ShouldSetDDR = false
};

const __flash KeyMatrixInfo_t KeyMatrixInfoRight = {
    .ColNum = RIGHT_COLS_NUM,
    .RowNum = ROWS_NUM,
    .RowPins = (Pin_t[]) {
        { .Direction=&DDRB, .Name=&PINB, .Number=PINB7 },
        { .Direction=&DDRB, .Name=&PINB, .Number=PINB6 },
        { .Direction=&DDRB, .Name=&PINB, .Number=PINB5 },
        { .Direction=&DDRD, .Name=&PIND, .Number=PIND5 },
        { .Direction=&DDRD, .Name=&PIND, .Number=PIND6 }
    },
    .ColPorts = (Pin_t[]) {
        { .Direction=&DDRC, .Name=&PORTC, .Number=PORTC6 },
        { .Direction=&DDRC, .Name=&PORTC, .Number=PORTC7 },
        { .Direction=&DDRB, .Name=&PORTB, .Number=PORTB4 },
        { .Direction=&DDRD, .Name=&PORTD, .Number=PORTD4 },
        { .Direction=&DDRD, .Name=&PORTD, .Number=PORTD1 },
        { .Direction=&DDRD, .Name=&PORTD, .Number=PORTD0 },
        { .Direction=&DDRC, .Name=&PORTC, .Number=PORTC2 }
    },
    .ShouldSetDDR = true
};

int KeyboardMainLoop(void)
{
    USART_Init();

    KeyMatrix_Init(KEYMATRIX_RIGHT, &KeyMatrixInfoRight);
    KeyMatrix_Init(KEYMATRIX_LEFT, &KeyMatrixInfoLeft);

    for (;;)
    {
        HID_Device_USBTask(&Keyboard_HID_Interface);
        HID_Device_USBTask(&Mouse_HID_Interface);
        HID_Device_USBTask(&Generic_HID_Interface);

        USB_USBTask();
    }
}

void KeyboardRXCallback(void)
{
    uint8_t Event = USART_ReceiveByte();

    uint8_t KeyId = GET_EVENT_PAYLOAD(Event);
    uint8_t Row = EXTRACT_KEYCODE_ROW(KeyId, LEFT_COLS_NUM);
    uint8_t Col = EXTRACT_KEYCODE_COL(KeyId, LEFT_COLS_NUM);

    uint8_t KeyState = KeyMatrix_GetElement(KEYMATRIX_LEFT, Row, Col);
    uint8_t WasKeyPressed = GET_KEY_STATE_CURRENT(KeyState);
    uint8_t IsKeyPressed = GET_EVENT_STATE(Event);
    uint8_t IsKeySuppressed = GET_KEY_STATE_SUPPRESSED(KeyState);
    uint8_t NewKeyState = CONSTRUCT_KEY_STATE(WasKeyPressed, IsKeyPressed, IsKeySuppressed);

    KeyMatrix_SetElement(KEYMATRIX_LEFT, Row, Col, NewKeyState);
}

bool EVENT_USB_Keyboard_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);
    ConfigSuccess &= HID_Device_ConfigureEndpoints(&Mouse_HID_Interface);
    ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);

    USB_Device_EnableSOFEvents();

    return ConfigSuccess;
}

void EVENT_USB_Keyboard_Device_ControlRequest(void)
{
    HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
    HID_Device_ProcessControlRequest(&Mouse_HID_Interface);
    HID_Device_ProcessControlRequest(&Generic_HID_Interface);
}

void EVENT_USB_Device_StartOfFrame(void)
{
    HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
    HID_Device_MillisecondElapsed(&Mouse_HID_Interface);
    HID_Device_MillisecondElapsed(&Generic_HID_Interface);
}

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
    if (HIDInterfaceInfo == &Keyboard_HID_Interface) {
        return CreateKeyboardHIDReport(ReportData, ReportSize);
    } else if (HIDInterfaceInfo == &Mouse_HID_Interface) {
        return CreateMouseHIDReport(ReportData, ReportSize);
    } else if (HIDInterfaceInfo == &Generic_HID_Interface) {
        return CreateGenericHIDReport(ReportData, ReportSize);
    }
    return false;
}

void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
    if (HIDInterfaceInfo == &Generic_HID_Interface) {
        ProcessGenericHIDReport(ReportData, ReportSize);
    }
}
