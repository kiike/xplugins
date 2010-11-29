#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdio.h>

static libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
static libusb_context *context = NULL; //a libusb session
static libusb_context *ctx = NULL;     //a libusb session
libusb_device_handle *radio_handle[4];
libusb_device_handle *multi_handle;
libusb_device_handle *switch_handle;


bool find_saitek_panels()
{
   int r; //for return values
   ssize_t cnt; //holding number of devices in list
   r = libusb_init(&context);  //initialize a library session
   libusb_device_descriptor desc;
   int radcnt = 0;
   if(r != 0)              //there was an init error
     return false;
   libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
   cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
   if(cnt < 0) {
     return false; //there was an error
   }
   ssize_t i; //for iterating through the list
   printf("\n***********   ");
   for(i = 0; i < cnt; i++) {
     r = libusb_get_device_descriptor(devs[i], &desc);
     if (r < 0) {
      return false; //failed to get device descriptor
     }

     if (desc.idVendor == 0x06a3){ //looking for saitel panels

        if (desc.idProduct == 0x0d05){ //looking for radio panels
            radcnt++;  //looking for more than one
            printf("Found Radio Panel # %d  ", radcnt);
       }

       if (desc.idProduct == 0x0d06){ //looking for multi panel
           printf("Found Multi Panel ");
       }

       if (desc.idProduct == 0x0d67){ // looking for switch panel
           printf("Found Switch Panel ");
       }
     }
   }

   libusb_free_device_list(devs, 1); //free the list, unref the devices in it
   libusb_exit(ctx); //close the session
   printf("**********\n");
   return true;
}

