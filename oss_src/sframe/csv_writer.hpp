/**
 * Copyright (C) 2015 Dato, Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */
#ifndef GRAPHLAB_SFRAME_CSV_WRITER_HPP
#define GRAPHLAB_SFRAME_CSV_WRITER_HPP
#include <string>
#include <vector>
#include <iostream>
#include <flexible_type/flexible_type.hpp>
namespace graphlab {

class csv_writer {
 public:

  // the ordering is slightly odd. But this is compatible with the python csv
  // quote level
  enum class csv_quote_level {
    QUOTE_MINIMAL, ///< NOT IMPLEMENTED. Equivalent to QUOTE_NONNUMERIC
    QUOTE_ALL, /// Quotes all fields
    QUOTE_NONNUMERIC, ///< Equivalent to python csv.QUOTE_NONNUMERIC. Numbers are not quoted
    QUOTE_NONE, ///< Equivalent to python csv.QUOTE_NONE. No quoting is performed
  };

  /**
   * The delimiter character to use to separate fields (Default ',')
   */
  std::string delimiter = ",";

  /**
   * The character to use to identify the beginning of a C escape sequence 
   * (Defualt '\'). i.e. "\n" will be converted to the '\n' character, "\\"
   * will be converted to "\", etc. Note that only the single character 
   * escapes are converted. unicode (\Unnnn), octal (\nnn), hexadecimal (\xnn)
   * are not interpreted.
   */
  char escape_char = '\\';

  /**
   * If set to true, pairs of quote characters in a quoted string 
   * are interpreted as a single quote (Default false).
   * For instance, if set to true, the 2nd field of the 2nd line is read as
   * \b "hello "world""
   * \verbatim
   * user, message
   * 123, "hello ""world"""
   * \endverbatim
   */
  bool double_quote = true; 

  /**
   * The quote character to use (Default '\"')
   */
  char quote_char = '\"';

  /**
   * new line terminator. Defaults to "\n"
   */
  std::string line_terminator= "\n";

  /**
   * Whether the header is written
   */
  bool header = true;

  /**
   * The quoting level. Defaults to quoting everything except for numbers
   */
  csv_quote_level quote_level = csv_quote_level::QUOTE_NONNUMERIC;

  /**
   * String to emit for missing values
   */
  std::string na_value= "";

  /**
   * Writes an array of strings as a row, verbatim without escaping /
   * modifications.  (only inserting delimiter characters).
   * Not safe to use in parallel.
   */
  void write_verbatim(std::ostream& out, const std::vector<std::string>& row);

  /**
   * Writes an array of values as a row, making the appropriate formatting 
   * changes. Not safe to use in parallel.
   */
  void write(std::ostream& out, const std::vector<flexible_type>& row);
  
  /**
   * Converts one value to a string
   */
  void csv_print(std::ostream& out, const flexible_type& val);
 
 private:

  /**
   * Converts one value, appending it to a string. 
   * minimal quoting is performed: only strings are quoted.
   * This is used for recursive prints (ex: printing a list)
   */
  void csv_print_internal(std::string& out, const flexible_type& val);

  /*
   * These are basically some optimizations to csv_print / csv_print_internal
   * to avoid allocating additional strings everytime. We just 
   * repeatedly make use of the same set of buffers.
   * This does mean that csv_print is *not* thread safe. But that's alright.
   */ 

  /** The buffer used by csv_print to handle additional quoting required.
   * Specifically, csv_print calls csv_print_internal on some cases 
   * (like dictionary) into this buffer. Then calls escape_string on this
   * buffer to generate a dictionary in string form.
   */
  std::string m_complex_type_temporary; 
  std::string m_complex_type_escape_buffer; 
  size_t m_complex_type_escape_buffer_len = 0; 

  /**
   * Temporary storage used by csv_print, and csv_print_internal to escape 
   * flexible_types containing strings.
   */
  std::string m_string_escape_buffer; 
  size_t m_string_escape_buffer_len = 0; 
};
  

} // namespace graphlab
#endif
