#include "error_codes.h"

// Error code list received from xinxi.liang@broadcom.com
// The actual error returns are likely valid only for APConnect as all the
// other api's seem to have custom return values :-/
const char* BcmErrorCodeString(int code) {
  switch (code) {
    // these are scan error codes. May not be accurate for other calls (or
    // accurate in general). Here to have at least something.
    case -1:
      return "Semaphore not initialized";
    case -2:
      return "Semaphore timeout";  // 500ms timeout
    case -3:
      return "Failed to get semaphore";
    case -4:
      return "Generic Wifi error";

    // connection error codes
    case 0:
      return "Success";
    case 1:
      return "Pending";
    case 2:
      return "Timeout";
    case 4:
      return "Error";
    case 5:
      return "Bad Arguments";
    case 6:
      return "Mode not supported";
    case 1003:
      return "Partial results";
    case 1004:
      return "Invalid key";
    case 1005:
      return "Does not exist";
    case 1006:
      return "Not authenticated";
    case 1007:
      return "Not keyed";
    case 1008:
      return "IOCTL fail";
    case 1009:
      return "Buffer unavailable temporarily";
    case 1010:
      return "Buffer unavailable permanently";
    case 1011:
      return "WPS PBC overlap";
    case 1012:
      return "Connection lost";
    case 1013:
      return "Cannot add extra event handler";
    case 1014:
      return "Error manipulating a semaphore";
    case 1015:
      return "Packet retrieval cancelled due to flow control";
    case 1016:
      return "Packet retrieval cancelled due to lack of bus credits";
    case 1017:
      return "Packet retrieval cancelled due to no pending packets";
    case 1018:
      return "Core disabled due to no clock";
    case 1019:
      return "Core disabled - in reset";
    case 1020:
      return "Unsupported function";
    case 1021:
      return "Error writing to WLAN register";
    case 1022:
      return "SDIO bus failed to come up";
    case 1023:
      return "Join not finished yet";
    case 1024:
      return "Specified network was not found";
    case 1025:
      return "Join status error";
    case 1026:
      return "Unknown interface specified";
    case 1027:
      return "Error during SDIO receive";
    case 1028:
      return "Hardware tag header corrupt";
    case 1029:
      return "Failed to allocate a buffer to receive into";
    case 1030:
      return "Error reading a bus hardware register";
    case 1031:
      return "Failed to create a new thread";
    case 1032:
      return "Error manipulating a queue";
    case 1033:
      return "Error moving the current pointer of a packet buffer ";
    case 1034:
      return "Error setting size of packet buffer";
    case 1035:
      return "Null stack pointer passed when non null was reqired";
    case 1036:
      return "Error deleting a thread";
    case 1037:
      return "Error sleeping a thread";
    case 1038:
      return "Failed to allocate a packet buffer";
    case 1039:
      return "No Packets waiting to be received";
    case 1040:
      return "Requested interface is not active";
    case 1041:
      return "Requested delay is too long";
    case 1042:
      return "Duty cycle is outside limit 0 to 100";
    case 1043:
      return "Returned pmk was the wrong length";
    case 1044:
      return "AP security type was unknown";
    case 1045:
      return "AP not allowed to use WEP - it is not secure - use Open instead";
    case 1046:
      return "WPA / WPA2 key length must be between 8 & 64 bytes";
    case 1047:
      return "Specified filter id not found";
    case 1048:
      return "Failed to read 0xfeedbead SPI id from chip";
    case 1049:
      return "Mismatch in sizes between SPI header and SDPCM header";
    case 1050:
      return "Attempt to register a multicast address twice";
    case 1051:
      return "SDIO transfer failed too many times.";
    case 1052:
      return "Null Pointer argument passed to function.";
    case 1053:
      return "Error deleting a thread";
    case 1054:
      return "Semaphore/mutex wait has been aborted";
    case 1055:
      return "Failed to set block ack window";
    case 1056:
      return "Requested delay is too short";
    case 1057:
      return "Invalid interface provided";
    case 1058:
      return "WEP / WEP_SHARED key length must be 5 or 13 bytes";
    case 1059:
      return "EAPOL handler already registered";
    case 1060:
      return "Soft AP or P2P group owner already up";
    case 1061:
      return "Timeout occurred while waiting for EAPOL packet M1 from AP";
    case 1062:
      return "Timeout occurred while waiting for EAPOL packet M3 from AP, "
             "which may indicate incorrect WPA2/WPA passphrase";
    case 1063:
      return "Timeout occurred while waiting for EAPOL packet G1 from AP";
    case 1064:
      return "Unknown failure occurred during the EAPOL key handshake";
    case 1065:
      return "Memory allocation failure";
    case 1066:
      return "Access point not found";
    case 2020:
      return "Channel not allowed (may not be in current power map).";
    default:
      return "<UNKNOWN ERROR>";
  }
}
