/*  Panic Dumping
 * 
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 * 
 */

#include "Common/Cpp/Exceptions.h"
#include "ClientSource/Libraries/Logging.h"
#include "PrettyPrint.h"
#include "PanicDump.h"

namespace PokemonAutomation{



void panic_dump(const char* location, const char* message){
    std::string body;
    body += "\xef\xbb\xbf"; //  UTF-8 BOM
//    body += "Panic Dump:\r\n";

    body += "Caught Location: ";
    body += location;
    body += "\r\n\r\n";

//    body += "Exception: ";
    body += message;
    body += "\r\n";

    FILE* file = fopen(("PanicDump-" + now_to_filestring() + ".log").c_str(), "wb");
    fwrite(body.c_str(), sizeof(char), body.size() * sizeof(char), file);
    fclose(file);
}


void run_with_catch(const char* location, std::function<void()>&& lambda){
    try{
        lambda();
    }catch (ProgramCancelledException&){
        panic_dump(location, "ProgramCancelledException");
        throw;
    }catch (InvalidConnectionStateException&){
        panic_dump(location, "InvalidConnectionStateException");
        throw;
    }catch (OperationCancelledException&){
        panic_dump(location, "OperationCancelledException");
        throw;
    }catch (const char* e){
        panic_dump(location, e);
        throw;
    }catch (const std::string& e){
        panic_dump(location, e.c_str());
        throw;
    }catch (const std::exception& e){
        panic_dump(location, e.what());
        throw;
    }catch (...){
        panic_dump(location, "Unknown Exception");
        throw;
    }
}



}
