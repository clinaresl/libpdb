// -*- coding: utf-8 -*-
// PDBprogress_t.h
// -----------------------------------------------------------------------------
//
// Started on <vie 16-05-2025 10:10:50.971688352 (1747383050)>
// Carlos Linares López <carlos.linares@uc3m.es>
//

//
// Definition of a progress bar
//

#ifndef _PDBPROGRESS_T_H_
#define _PDBPROGRESS_T_H_

#include<iostream>
#include<sstream>
#include<string>
#include<sys/ioctl.h>
#include <unistd.h>

namespace pdb {

    class progress_t {

    private:

        // INVARIANT: a progress bar is defined using two values: the current
        // value and an upper bound. The ratio between them is used to compute
        // the chunk of the progress bar to show
        int _value;
        int _upper_bound;

        // In addition, the progress bar might have a prefix, and also a
        // suffix
        std::string _prefix;
        std::string _suffix;

        // create a string which consists of an arbitrary number of repetitions
        // of a utf-8 character
        std::string _repeat_utf8(const std::string& utf8_char, size_t count) const {
            std::string result;
            result.reserve(utf8_char.size() * count);
            for (size_t i = 0; i < count; ++i) {
                result += utf8_char;
            }
            return result;
        }

    public:

        // Default constructors are forbidden
        progress_t () = delete;

        // Explicit constructors - it is mandatory to provide an upper bound
        explicit progress_t (int upper_bound) :
            _value       {           0 },
            _upper_bound { upper_bound }
            {}

        // getters
        int get_value () const {
            return _value;
        }
        int get_upper_bound () const {
            return _upper_bound;
        }
        const std::string get_prefix () const {
            return _prefix;
        }
        const std::string get_suffix () const {
            return _suffix;
        }

        // setters
        void set_value (int value) {
            _value = value;
        }
        void set_prefix (std::string value) {
            _prefix = value;
        }
        void set_suffix (std::string value) {
            _suffix = value;
        }

        // When showing a progress bar it is always assumed that the cursor is
        // already placed at the first column of the line where the progress bar
        // has to be displayed. The progress bar is shown on the standard output
        void show () const {

            // the width of the terminal is re-computed with every redrawing
            // because the user might have scale it. In case it is not possible
            // to determine the width of the console taek 100
            int width = 100;
            struct winsize w;
            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1) {

                // in case it was possible to determine the width of the console
                // substract the length of the prefix and suffix.
                width = w.ws_col - _prefix.size () - _suffix.size () - 2;
            }

            // generate the full contents of the progress bar in a stream
            // string
            std::stringstream stream;

            // Add the prefix
            stream << _prefix;

            // and then show the progress bar taking a length which is
            // proportional to the ratio _value/_upper_bound
            int length = width*_value/_upper_bound;
            stream << _repeat_utf8 ("▒", length) << " ";

            // and display the suffix at the end
            stream << std::string (w.ws_col - _prefix.size () - length - 2, ' ') << _suffix;

            // finally, display it
            std::cout << "\033[?25l" << "\r" << stream.str () << "\033[?25h";
        }

    }; // class progress_t

} // namespace pdb

#endif // _PDBPROGRESS_T_H_

// Local Variables:
// mode:cpp
// fill-column:80
// End:
