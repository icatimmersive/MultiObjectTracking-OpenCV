#include "blobSender.h"


blobSender::blobSender(const char* const address, int port) {
    sender = std::unique_ptr<Sender>(new Sender(address,port));
}

bool blobSender::sendNewBlob(Blob *const blob) {
    blob->age = newStr;
    return sendBlob(blob);
}

bool blobSender::sendUpdateBlob(Blob *const blob) {
    blob->age = updateStr;
    return sendBlob(blob);
}

bool blobSender::sendRemoveBlob(Blob *const blob) {
    blob->age = removeStr;
    return sendBlob(blob);
}

bool blobSender::sendBlob(Blob *const blob) {
    string ageStr(blob->age);

    return sender->writeDataToServer("{\"age\":" + ageStr + ", \"connectionType\": \"DATASOURCE\"," + "\"id\":" + to_string(blob->id) +
                     ",\"cameraID\":" + to_string(blob->cameraID) +
                     ",\"origin\": {\"x\":" + to_string(blob->origin_x) + ",\"y\":" + to_string(blob->origin_y) +
                     ",\"z\":" + to_string(blob->origin_z) + "}, \"orientation\": {\"x\":" +
                     to_string(blob->orientation_x) + ", \"y\":" + to_string(blob->orientation_y) + ", \"z\":" +
                     to_string(blob->orientation_z) + ", \"theta\":" + to_string(blob->orientation_theta) +
                     "}, \"source\":\"MATLAB\", \"updateTime\":0, \"creationTime\":0," +
                     "\"boundingBox\":{\"x\":" + to_string(blob->bounding_x) + ", \"y\":" +
                     to_string(blob->bounding_y) +
                     ",\"width\":" + to_string(blob->bounding_width) + ",\"height\":" +
                     to_string(blob->bounding_height) + ",\"image_width\":" +
                     to_string(blob->image_width) + ",\"image_height\":" +
                     to_string(blob->image_height) + "}}&") == 0;
}
