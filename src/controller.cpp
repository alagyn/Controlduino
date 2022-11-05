#include "controller.h"
#include "errorUtils.h"

#include <sstream>

namespace bdd {

    Controller::Controller()
        : client()
        , pad()
    {
        client = vigem_alloc();
        if(!client)
        {
            displayError("Controller()", "Cannot allocate ViGEm client");
            throw ControllerException();
        }

        auto ret = vigem_connect(client);
        if(!VIGEM_SUCCESS(ret))
        {
            std::stringstream ss;
            ss << "Cannot connect to ViGEm client, error code: 0x" << std::hex << ret;
            displayError("main()", ss.str().c_str());
            vigem_free(client);
            throw ControllerException();
        }

        pad = vigem_target_x360_alloc();
        ret = vigem_target_add(client, pad);
        if(!VIGEM_SUCCESS(ret))
        {
            std::stringstream ss;
            ss << "Cannot allocate controller, error code: 0x" << std::hex << ret;
            displayError("main()", ss.str().c_str());
            vigem_free(client);
            vigem_target_free(pad);
            throw ControllerException();
        }
    }

    Controller::~Controller()
    {
        vigem_target_remove(client, pad);
        vigem_target_free(pad);
        vigem_free(client);
    }

    void Controller::update(const Ard_XInput& ard_state)
    {
        XUSB_REPORT state = calib->clampState(ard_state);
        vigem_target_x360_update(client, pad, state);
    }

} //namespace bdd