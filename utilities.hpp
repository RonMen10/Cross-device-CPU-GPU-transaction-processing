//
// Created by gurumurt on 28/04/20.
//

#ifndef HERO_UTILITIES_HPP
#define HERO_UTILITIES_HPP


#include "headers.hpp"
#include <sstream>
#include <fstream>


/*!
 * Function to read the kernel string from given file
 * @param _path - Location of the kernel file
 * @return - Kernel string
 */
namespace utilities{

    /**
     * Reads a file content and returns them as string
     * @param _path location of the file
     * @return contents of file
     */
    static std::string readFile(std::string _path) {

    std::string _m_output;



    std::ifstream _m_myfile(_path);

    if (!_m_myfile) { // failed to open file
        std::cout << "File failed to open : " << _path << std::endl;
    } else {
        //cout << "File successfully to opened" << endl;

        std::stringstream _m_buffer;
        _m_buffer << _m_myfile.rdbuf();
        _m_output = _m_buffer.str();
        _m_myfile.close();
    }
    return _m_output;
}

}
#endif //HERO_UTILITIES_HPP
