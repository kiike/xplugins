#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>


static libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
static libusb_context *context = NULL; //a libusb session
static libusb_context *ctx = NULL;     //a libusb session
libusb_device_handle *radio_handle[4];
libusb_device_handle *multi_handle;
libusb_device_handle *switch_handle;
char radiowdata[4][20]; //data to write

int radiocnt = 0;
int r; //for return values


//radiowdata[2][0] = 1, radiowdata[2][1] = 2, radiowdata[2][2] = 3, radiowdata[2][3] = 4;
//radiowdata[2][4] = 5, radiowdata[2][5] = 6, radiowdata[2][6] = 7, radiowdata[2][7] = 8;
//radiowdata[2][8] = 9, radiowdata[2][9] = 0, radiowdata[2][10] = 1, radiowdata[2][11] = 2;
//radiowdata[2][12] = 3, radiowdata[2][13] = 4, radiowdata[2][14] = 5, radiowdata[2][15] = 6;
//radiowdata[2][16] = 7, radiowdata[2][17] = 8, radiowdata[2][18] = 9, radiowdata[2][19] = 0;


bool find_saitek_panels()
{
   //int r; //for return values
   ssize_t cnt; //holding number of devices in list
   r = libusb_init(&context);  //initialize a library session
   libusb_device_descriptor desc;
   //int radiocnt = 0;
   if(r != 0)              //there was an init error
     return false;
   libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
   cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
   if(cnt < 0) {
     return false; //there was an error
   }
   ssize_t i; //for iterating through the list
   printf("\n");
   for(i = 0; i < cnt; i++) {
     r = libusb_get_device_descriptor(devs[i], &desc);
     if (r < 0) {
      return false; //failed to get device descriptor
     }

     if (desc.idVendor == 0x06a3){ //looking for saitel panels

        if (desc.idProduct == 0x0d05){ //looking for radio panels
            radiocnt++;  //looking for more than one
            printf("Found Radio Panel # %d  ", radiocnt);
            r = libusb_open(devs[i], &radio_handle[radiocnt]); //Use devs[i] from libusb list to create handle
            if(radio_handle[radiocnt] < 0){
                    printf("Cannot open Radio Panel\n");
                    return false;
            }
                    else
                    printf("Radio Panel Opened\n");

        }

        if (desc.idProduct == 0x0d06){ //looking for multi panel
           printf("Found Multi Panel ");
           r = libusb_open(devs[i], &multi_handle); //Use devs[i] from libusb list to create handle
           if(multi_handle < 0){
                   printf("Cannot open Multi Panel\n");
                   return false;
           }
                   else
                   printf("Multi Panel Opened\n");


        }

        if (desc.idProduct == 0x0d67){ // looking for switch panel
           printf("Found Switch Panel ");
           r = libusb_open(devs[i], &switch_handle); //Use devs[i] from libusb list to create handle
           if(switch_handle < 0){
                   printf("Cannot open switch Panel\n");
                   return false;
           }
                   else
                   printf("Switch Panel Opened\n");

        }
     }

   }

   libusb_free_device_list(devs, 1); //free the list, unref the devices in it

   return true;
}


bool write_radio_panel(libusb_device_handle *radio_handle[], unsigned char radiowdata[4]){
    int radionumb = radiocnt;
    if(libusb_kernel_driver_active(radio_handle[radionumb], 0) == 1) { //find out if kernel driver is attached
            printf("Kernel Driver Active\n");
            if(libusb_detach_kernel_driver(radio_handle[radionumb], 0) == 0) //detach it
                    printf("Kernel Driver Detached!\n");
    }
    r = libusb_claim_interface(radio_handle[radionumb], 0); //claim interface 0 (the first) of device
    if(r < 0) {
            printf("Cannot Claim Interface\n");
            return false;
    }

    //radiowdata[2][0] = 1, radiowdata[2][1] = 2, radiowdata[2][2] = 3, radiowdata[2][3] = 4;
    //radiowdata[2][4] = 5, radiowdata[2][5] = 6, radiowdata[2][6] = 7, radiowdata[2][7] = 8;
    //radiowdata[2][8] = 9, radiowdata[2][9] = 0, radiowdata[2][10] = 1, radiowdata[2][11] = 2;
    //radiowdata[2][12] = 3, radiowdata[2][13] = 4, radiowdata[2][14] = 5, radiowdata[2][15] = 6;
    //radiowdata[2][16] = 7, radiowdata[2][17] = 8, radiowdata[2][18] = 9, radiowdata[2][19] = 0;



    r = libusb_control_transfer(radio_handle[radionumb], //handle
            LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,  //request type
            0x09,                     //HID set_report request
            (3<< 8),                  //HID feature value
            0,                        //index
            radiowdata[radionumb],    //data
            20,                       //length
            500);                    //timeout


    if(r > 0) //we wrote the 4 bytes successfully
            printf("Writing Successful!\n");
    else
            printf("Write Error\n");

    return true;
}



bool close_saitek_panels(){
   int radionumb = radiocnt;
   while (radionumb > 0){
     r = libusb_release_interface(radio_handle[radionumb], 0); //release the claimed interface
     if(r!=0) {
       printf("Cannot Release Interface\n");
       return false;
     }
     printf("Released Interface\n");
       if (libusb_attach_kernel_driver(radio_handle[radionumb], 0) == 0) //Attach Driver
         printf("Kernel Driver Atached!\n");

     libusb_close(radio_handle[radionumb]);
     printf("Closed Radio Panel # %d\n",radionumb);
     radionumb--;
   }

   libusb_close(multi_handle);
   libusb_close(switch_handle);

   libusb_exit(ctx); //close the session
   return true;
}
