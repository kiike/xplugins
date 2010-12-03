#ifndef LIBUSB_INTERFACE_H
#define LIBUSB_INTERFACE_H

#include <libusb-1.0/libusb.h>

extern libusb_device_handle *radio_handle[4];
extern unsigned char radiowdata[];

//  Initilize libusb get list get discriptors
//  Find saitek radio multi and switch panels
//  Returns true if no problems were encountered
bool find_saitek_panels();
bool write_radio_panel(libusb_device_handle *radio_handle, unsigned char radiowdata[]);
bool close_saitek_panels();

bool test_write_radio_panel();


#endif // LIBUSB_INTERFACE_H
