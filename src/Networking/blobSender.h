//
// Created by Eric on 10/11/2015.
//

#ifndef MULTIOBJECTTRACKING_C_SENDBLOB_H
#define MULTIOBJECTTRACKING_C_SENDBLOB_H

#include "blob.h"
#include "sender.h"
#include <string>
#include <memory>
#define newStr  "\"NEW\""
#define updateStr "\"OLD\""
#define removeStr "\"LOST\""

class blobSender {

public:
    blobSender(const char* const address, int port);

    bool sendNewBlob(Blob *const blob);

    bool sendUpdateBlob(Blob *const blob);

    bool sendRemoveBlob(Blob *const blob);

    bool sendNewBlob(Blob blob) {
        return sendNewBlob(&blob);
    }

    bool sendUpdateBlob(Blob blob) {
        return sendUpdateBlob(&blob);
    }

    bool sendRemoveBlob(Blob blob) {
        return sendRemoveBlob(&blob);
    }

private:
    bool sendBlob(Blob *const blob);

    std::unique_ptr<Sender> sender;
};

using namespace std;


#endif //MULTIOBJECTTRACKING_C_SENDBLOB_H
