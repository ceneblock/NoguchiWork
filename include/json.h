#ifndef JSON_H
#define JSON_H

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/schema.h"
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>

class JSON
{
  public:
    JSON(const char *fileLocation = NULL, const char *schema = NULL );
    ~JSON();

    bool parse();
    bool openFile(const char *fileLocation);

    bool getValue(const char *index, rapidjson::Value &value);

    bool getInt(const char *index, int &value);
    bool getDouble(const char *index, double &value);
    bool getBool(const char *index, bool &value);
    bool getString(const char *index, std::string &value);
    //If it's settable and readable then hiding it behind private is pointless.
    std::filesystem::path fileLocation;
    std::filesystem::path schemaLocation;
  private:
    std::ifstream jsonSchemaFile;
    std::string jsonSchemaData;
    std::ifstream jsonFile;
    std::string jsonData;
    rapidjson::Document document;
    rapidjson::SchemaDocument *schema;
    rapidjson::SchemaValidator *schemaValidator;
};
#endif
