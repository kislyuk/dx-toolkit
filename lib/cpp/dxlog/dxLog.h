#ifndef DXLOG_H
#define DXLOG_H

#include <dxjson/dxjson.h>
#include "unixDGRAM.h"

using namespace std;

namespace DXLog {
  enum Level { EMERG, ALERT, CRIT, ERR, WARN, NOTICE, INFO, DEBUG };

  // Read a json object from a file
  dx::JSON readJSON(const string &filename);
  
  /** Validate that message has proper data for logging based on information specified in schema
   *  Return false if failed and errMsg has detailed information of error
   */
  bool ValidateLogData(const dx::JSON &schema, dx::JSON &message, string &errMsg); 

  // API for writing logs to rsyslog and/or mongodb
  class logger {
    private:
      dx::JSON schema;  // schema of log data
      string hostname, errmsg, txtMsgFile, dbMsgFile;  // hostname of local machine
      bool ready; // whether or not schema is valid

      void formMessage(const dx::JSON &message, string &msg);

    public:
      logger(dx::JSON &schema, const string &txtFile = "/var/log/dnanexusLocal/Cppsyslog", const string &dbFile = "/var/log/dnanexusLocal/CppDBSocket");

      bool isReady(string &errMsg);

      /** Send log message to rsyslog and/or mongodb.
	*  data is a hash contains the log message and related information 
	*  Reture whether or not if the message was sent successfully.
	*  Otherwise return false and errMsg contains detailed error message
	*/
      bool Log(dx::JSON &data, string &errMsg); 
  }; 
  
  // Wrapper for writing logs from an app
  class AppLog {
    private: 
      /** Each job has two limits of total number of messages
	*  One is for EMERG, ALERT, and CRIT messages
	*  The other is for ERR, WARN, NOTICE, INFO, DEBUG messages
	*  msgCoutn stores current number of messages being stored 
	*/
      static int msgCount[2], msgLimit; // 

      // Data associated with messages obtained from execution environment
      static string socketPath[2];
      static dx::JSON schema;
      
      // Determine which rsyslog socket to use for message with this level
      static int socketIndex(int level);

    public:
      // Set the above static variables based on execution environment
      static bool initEnv(const dx::JSON &conf, const dx::JSON &schema, string &errMsg);

      /** The public log function for any app
	*  Returns true if the message is successfully delivered to the log system;
	*  Otherwise returns false and errMsg contains some details of the error
	*/
      static bool log(dx::JSON &msg, string &errMsg);

      static bool done(string &errMsg);
  };

//  bool Log(int facility, int level, const string &tag, const string &msg, string &errMsg);
};

#endif
