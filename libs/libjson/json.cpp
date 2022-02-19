#include "json.h"

// #define NDEBUG
#include <cassert>
#include <string>
#include <filesystem>

JSON::JSON(const char *fileLocation, const char *schemaLocation):
  schema(NULL),
  schemaValidator(NULL),
  document(NULL)
{
  if(schemaLocation != NULL)
  {
    this->schemaLocation = std::filesystem::path(schemaLocation);
    jsonSchemaFile.open(schemaLocation,std::ios::in);
    
    //Find out how big a file is and then create string to hold all of it
    std::uintmax_t size = std::filesystem::file_size(schemaLocation);
    jsonSchemaData = std::string(size, '\0');

    //Slurp in all of the data
    jsonSchemaFile.read(jsonSchemaData.data(), size);


    //TODO: don't use assert..
    rapidjson::Document sd;
    assert (!sd.Parse(jsonSchemaData.c_str()).HasParseError());

    this->schema = new rapidjson::SchemaDocument(sd);
    this->schemaValidator = new rapidjson::SchemaValidator(*this->schema);
  }
  if(fileLocation != NULL)
  {
    openFile(fileLocation);
  }
}

JSON::~JSON()
{
  if(jsonFile.is_open())
  {
    jsonFile.close();
  }

  if(schema)
  {
    delete schema;
  }
  if(schemaValidator)
  {
    delete schemaValidator;
  }
}

/**
 * @brief parses the json file and checks for errors and (optionally) matches the schema
 * @return error true if there was some sort of error
 */
bool JSON::parse()
{
  bool error = false;

  if(jsonData.size() > 0)
  {
    if (!document.HasParseError())
    {
      //valid JSON now to check if it follows the schema
      if(schema != NULL)
      {
        if(!document.Accept(*schemaValidator))
        {
          // Input JSON is invalid according to the schema
          // Output diagnostic information
          rapidjson::StringBuffer sb;
          schemaValidator->GetInvalidSchemaPointer().StringifyUriFragment(sb);
          printf("Invalid schema: %s\n", sb.GetString());
          printf("Invalid keyword: %s\n", schemaValidator->GetInvalidSchemaKeyword());
          sb.Clear();
          schemaValidator->GetInvalidDocumentPointer().StringifyUriFragment(sb);
          printf("Invalid document: %s\n", sb.GetString());
      
          //rejected
          error = false;
        }
      }
    }
    else
    {
      error = true;
    }
  }
  else
  {
    error = true;
  }

  return error;
}

/**
 * @breif opens a JSON document for parsing
 * @param fileLocation the location of the json file
 * @return error true if we couldn't open the file
 */
bool JSON::openFile(const char *fileLocation)
{
    bool error = false;
    this->fileLocation = std::filesystem::path(fileLocation);
    jsonFile.open(fileLocation,std::ios::in);
    
    if(jsonFile.good())
    {
      //Find out how big a file is and then create string to hold all of it
      std::uintmax_t size = std::filesystem::file_size(fileLocation);
      jsonData = std::string(size, '\0');

      //Slurp in all of the data
      jsonFile.read(jsonData.data(), size);

      document.Parse(jsonData.c_str());
    }
    else
    {
      error = true;
    }

    jsonFile.close();

    return error;
}

/**
 * @brief gets a value from the document
 * @param member the member name
 * @param value the return value
 * @return error true if the member didn't exist
 * @TODO maybe template or something so I'm not returning a generic rapidjson::Value
 */
bool JSON::getValue(const char *member, rapidjson::Value &value)
{
  bool error = false;
  if(document.HasMember(member))
  {
    value = document[member];
  }
  else
  {
    error = true;
  }
  return error;
}

/**
 * @brief gets a value from the document
 * @param member the member name
 * @param value the return value
 * @return error true if the member didn't exist or wasn't an int
 */
bool JSON::getInt(const char *member, int &value)
{
  bool error = false;
  if(document.HasMember(member))
  {
    rapidjson::Value &tempValue = document[member];
    if(tempValue.IsInt())
    {
      value = document[member].GetInt();
    }
    else
    {
      error = true;
    }
  }
  else
  {
    error = true;
  }
  return error;
}
/**
 * @brief gets a value from the document
 * @param member the member name
 * @param value the return value
 * @return error true if the member didn't exist or wasn't a double
 */
bool JSON::getDouble(const char *member, double &value)
{
  bool error = false;
  if(document.HasMember(member))
  {
    rapidjson::Value &tempValue = document[member];
    if(tempValue.IsDouble())
    {
      value = document[member].GetDouble();
    }
    else
    {
      error = true;
    }
  }
  else
  {
    error = true;
  }
  return error;
}/**
 * @brief gets a value from the document
 * @param member the member name
 * @param value the return value
 * @return error true if the member didn't exist or wasn't a bool
 */
bool JSON::getBool(const char *member, bool &value)
{
  bool error = false;
  if(document.HasMember(member))
  {
    rapidjson::Value &tempValue = document[member];
    if(tempValue.IsBool())
    {
      value = document[member].GetBool();
    }
    else
    {
      error = true;
    }
  }
  else
  {
    error = true;
  }
  return error;
}/**
 * @brief gets a value from the document
 * @param member the member name
 * @param value the return value
 * @return error true if the member didn't exist or wasn't a string
 */
bool JSON::getString(const char *member, std::string &value)
{
  bool error = false;
  if(document.HasMember(member))
  {
    rapidjson::Value &tempValue = document[member];
    if(tempValue.IsString())
    {
      value = std::string(document[member].GetString());
    }
    else
    {
      error = true;
    }
  }
  else
  {
    error = true;
  }
  return error;
}
