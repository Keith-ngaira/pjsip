#include <pjsua2.hpp>
#include <iostream>
#include <thread>
#include <chrono>

using namespace pj;
using namespace std;

// Derived Account Class for Registration and Managing Calls
class MyAccount : public Account {
public:
    // Constructor
    MyAccount() {}

    // Method to handle registration state changes
    virtual void onRegState(OnRegStateParam &prm) {
        AccountInfo ai = getInfo();
        if (ai.regIsActive) {
            cout << "Successfully registered with the SIP server!" << endl;
        } else {
            cout << "Registration failed." << endl;
        }
    }

    // Method to handle incoming calls
    virtual void onIncomingCall(OnIncomingCallParam &iprm) {
        Call *call = new Call(*this, iprm.callId);
        CallOpParam prm;
        prm.statusCode = PJSIP_SC_OK;
        call->answer(prm); // Automatically answer incoming calls for this example
        cout << "Incoming call received and answered." << endl;
    }
};

// Derived Call Class for Managing Call States
class MyCall : public Call {
public:
    MyCall(Account &acc) : Call(acc) {}

    // Method to handle call state changes
    virtual void onCallState(OnCallStateParam &prm) {
        CallInfo ci = getInfo();
        cout << "Call state changed: " << ci.stateText << endl;

        if (ci.state == PJSIP_INV_STATE_DISCONNECTED) {
            delete this; // Clean up the call instance when the call ends
        }
    }
};

// Main Function
int main() {


    // Step 1: Initialize PJSUA
    EpConfig ep_cfg;
    Endpoint ep;

    try {
        ep.libCreate();
        ep_cfg.logConfig.level = 4;  // Adjust log level as necessary
        ep.libInit(ep_cfg);

        TransportConfig tcfg;
        tcfg.port = 5060;
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
        ep.libStart();
        cout << "PJSIP started and ready." << endl;

        // Step 2: Set up the Account
        AccountConfig acc_cfg;
        acc_cfg.idUri = "sip:joe@demo.dial-afrika.com";
        acc_cfg.regConfig.registrarUri = "sip:demo.dial-afrika.com";
        acc_cfg.sipConfig.authCreds.push_back(AuthCredInfo("digest", "*", "joe", 0, "Temp@123"));

        MyAccount *acc = new MyAccount();
        acc->create(acc_cfg);

        // Step 3: Keep Alive Mechanism
        // Using a thread to periodically send keep-alive REGISTER requests
        std::thread keepAliveThread([acc]() {
            while (true) {
                this_thread::sleep_for(chrono::seconds(30)); // Check every 30 seconds
                try {
                    acc->setRegistration(true); // Re-register to keep the account alive
                    cout << "Re-registered to keep the account alive." << endl;
                } catch (const Error &err) {
                    cout << "Error re-registering: " << err.info() << endl;
                }
            }
        });

        // Step 4: Make an Outgoing Call
        MyCall *call = new MyCall(*acc);
        CallOpParam call_prm;
        call_prm.opt.audioCount = 1;
        call_prm.opt.videoCount = 0;

        string destination = "sip:destination_user@their_sip_domain";
        call->makeCall(destination, call_prm);
        cout << "Outgoing call to " << destination << " initiated." << endl;

        // Step 5: Hold and Unhold the Call
        this_thread::sleep_for(chrono::seconds(10));  // Wait a bit before holding
        CallOpParam hold_prm;
        call->setHold(hold_prm);
        cout << "Call put on hold." << endl;

        this_thread::sleep_for(chrono::seconds(5));   // Wait a bit before resuming
        call->reinvite(hold_prm);
        cout << "Call resumed from hold." << endl;

        // Wait indefinitely to keep the program running (or implement your own exit strategy)
        cout << "Press Enter to exit..." << endl;
        cin.get();

        // Clean up
        keepAliveThread.detach();
        delete acc;
        ep.libDestroy();
        cout << "PJSIP shutdown complete." << endl;
    } catch (const Error &err) {
        cerr << "PJSIP error: " << err.info() << endl;
        return 1;
    }
    std::cout << "PJSIP application is running." << std::endl;

    return 0;
}
