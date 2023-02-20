//
// Created by Peter Savory on 08/10/2022.
//

#ifndef MASTALGIA_EXCEPTIONS_H
#define MASTALGIA_EXCEPTIONS_H

/* A general exception class.
 * Should be extended by other TaleScripter exceptions or thrown as a general error.
 * Put as much functionality for error handling in this class as possible, as we should
 * use this class to write error logs and to display information to the user in future.
 */

#ifndef GENERAL_EXCEPTION_INCLUDED
#define GENERAL_EXCEPTION_INCLUDED

#include <fstream>
#include <exception>
#include <stdexcept>
#include <vector>
#include <sstream>
#include "Utils.h"

class GeneralException : public std::exception {
public:

    /**
     * Very basic constructor, probably shouldn't be used outside of quick debugging.
     */
    GeneralException() {
        fullMessage = "No information provided (This is not helpful.)";
        writeToFile();
    };

    /**
     * Simple constructor - To be used when we don't care about displaying the type of error.
     *
     * @param message
     */
    explicit GeneralException(const std::string &message) {
        fullMessage = message;
        writeToFile();
    };

    /**
     * Should be called by exceptions which inherit from this one.
     *
     * @param errorType - The type of error, will usually be the class name of the inheriting exception
     * @param message - The exception message/reason
     */
    GeneralException(const std::string &errorType, const std::string &message) {

        std::vector<std::string> errorStrings = {
                errorType,
                ": ",
                message
        };

        fullMessage = Utils::implodeString(errorStrings);
        writeToFile();
    }

    const char *what() {
        return fullMessage.c_str();
    }

private:
    std::string fullMessage;

    /**
     * Writes information about the exception to the error log
     */
    void writeToFile() {
        std::ofstream file("errors.log", std::ios::app);

        if (!file) {
            fullMessage.append(" - was not able to write to error log");
            return;
        }

        // TODO: Make the date format customisable
        std::string dateTime = Utils::getSystemDateTime(DateFormat::FORMAT_DATETIME_UK);
        std::stringstream ss;
        ss<<"------------"<<std::endl<<
          "Occurred at: "<<dateTime<<std::endl<<std::endl<<
          fullMessage<<std::endl<<std::endl;

        file<<ss.str();
        file.close();
    }
};

class Z80Exception : public GeneralException {
public:
    explicit Z80Exception(const std::string &message) : GeneralException(std::string("Z80 Exception"),
                                                                             message) {};
};

class VDPException : public GeneralException {
public:
    explicit VDPException(const std::string &message) : GeneralException(std::string("Z80 Exception"),
                                                                         message) {};
};

#endif

#endif //MASTALGIA_EXCEPTIONS_H
