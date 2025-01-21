#include <wil/com.h>
#include "WebView2.h"
#include <optional>
#include <filesystem>
#include <regex>
#include <iostream>

#include "../../sk_common.hxx"
#include "../../utils/sk_str_utils.hxx"





// Enum to represent resource context with MIME types
 enum class ResourceContext
 {
   Document,
   Stylesheet,
   Image,
   Media,
   Font,
   Script,
   XMLHttpRequest,
   Fetch,
   TextTrack,
   EventSource,
   WebSocket,
   Manifest,
   Other
 };

 // Map enum values to corresponding MIME types or descriptions
 static const std::map<ResourceContext, std::string> ResourceContextMimeMap = {{ResourceContext::Document, "text/html"},
                                                                        {ResourceContext::Stylesheet, "text/css"},
                                                                        {ResourceContext::Image, "image/*"},
                                                                        {ResourceContext::Media, "video/*, audio/*"},
                                                                        {ResourceContext::Font, "font/*"},
                                                                        {ResourceContext::Script, "application/javascript"},
                                                                        {ResourceContext::XMLHttpRequest, "application/json"},
                                                                        {ResourceContext::Fetch, "application/json"},
                                                                        {ResourceContext::TextTrack, "text/vtt"},
                                                                        {ResourceContext::EventSource, "text/event-stream"},
                                                                        {ResourceContext::WebSocket, "application/websocket"},
                                                                        {ResourceContext::Manifest, "application/json"},
                                                                        {ResourceContext::Other, "application/octet-stream"}};

 // Function to convert COREWEBVIEW2_WEB_RESOURCE_CONTEXT to ResourceContext
 static ResourceContext ConvertToResourceContext(COREWEBVIEW2_WEB_RESOURCE_CONTEXT context)
 {
   switch (context)
   {
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_DOCUMENT:
     return ResourceContext::Document;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_STYLESHEET:
     return ResourceContext::Stylesheet;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_IMAGE:
     return ResourceContext::Image;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_MEDIA:
     return ResourceContext::Media;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_FONT:
     return ResourceContext::Font;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_SCRIPT:
     return ResourceContext::Script;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_XML_HTTP_REQUEST:
     return ResourceContext::XMLHttpRequest;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_FETCH:
     return ResourceContext::Fetch;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_TEXT_TRACK:
     return ResourceContext::TextTrack;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_EVENT_SOURCE:
     return ResourceContext::EventSource;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_WEBSOCKET:
     return ResourceContext::WebSocket;
   case COREWEBVIEW2_WEB_RESOURCE_CONTEXT_MANIFEST:
     return ResourceContext::Manifest;
   default:
     return ResourceContext::Other;
   }
 }


 static std::string getMimeTypeFromFilename(const std::string& filename)
 {
   static const std::unordered_map<std::string, std::string> mimeTypes = {
     {"html", "text/html"},        {"htm", "text/html"},       {"css", "text/css"},   {"js", "application/javascript"},
     {"json", "application/json"}, {"png", "image/png"},       {"jpg", "image/jpeg"}, {"jpeg", "image/jpeg"},
     {"gif", "image/gif"},         {"svg", "image/svg+xml"},   {"txt", "text/plain"}, {"xml", "application/xml"},
     {"pdf", "application/pdf"},   {"zip", "application/zip"}, {"mp3", "audio/mpeg"}, {"mp4", "video/mp4"},
     {"wav", "audio/wav"}
     // Add more MIME types as needed
   };

   // Find the last dot to get the file extension
   size_t dotPos = filename.find_last_of('.');
   if (dotPos != std::string::npos && dotPos + 1 < filename.size())
   {
     std::string extension = filename.substr(dotPos + 1);
     auto it = mimeTypes.find(extension);
     if (it != mimeTypes.end())
     {
       return it->second;
     }
   }

   // Default MIME type if no match is found
   return "application/octet-stream";
}

static std::string ReadStream(IStream* stream)
{
  if (!stream)
    return "";
  char buffer[4096];
  ULONG bytesRead;
  std::string result;
  while (SUCCEEDED(stream->Read(buffer, sizeof(buffer), &bytesRead)) && bytesRead > 0)
  {
    result.append(buffer, bytesRead);
  }
  return result;
}

// Function to get MIME type from ResourceContext
static std::string GetMimeType(ResourceContext context)
{
  auto it = ResourceContextMimeMap.find(context);
  return (it != ResourceContextMimeMap.end()) ? it->second : "unknown";
}

static std::string resourceContextToString(COREWEBVIEW2_WEB_RESOURCE_CONTEXT webResourceContext)
{
  ResourceContext context = ConvertToResourceContext(webResourceContext);
  return GetMimeType(context);
}






static nlohmann::json ExtractHeadersToJson(ICoreWebView2HttpRequestHeaders* headers)
 {
   nlohmann::json headersJson;

   // Start by getting the iterator for the headers
   wil::com_ptr<ICoreWebView2HttpHeadersCollectionIterator> iterator;
   if (SUCCEEDED(headers->GetIterator(&iterator)))
   {
     while (true)
     {
       // Get the current header's name and value
       wil::unique_cotaskmem_string headerName, headerValue;
       HRESULT hr = iterator->GetCurrentHeader(&headerName, &headerValue);

       if (SUCCEEDED(hr))
       {
         // Add the header name and value to the JSON object
         headersJson[wstringToString(headerName.get())] = wstringToString(headerValue.get());
       }

       else
       {
         break; // Stop if GetCurrentHeader fails
       }


       // Move to the next header; stop if no more headers
       BOOL hasNext = FALSE;
       if (FAILED(iterator->MoveNext(&hasNext)) || !hasNext)
       {
         break;
       }
     }
   }

   return headersJson;
 }







class SK_WebViewResource_Request
 {
 public:
   std::string url;
   std::string protocol;
   std::string host;
   std::string path;
   std::string method;
   std::string resourceContext;
   nlohmann::json parameters;
   nlohmann::json headers;
   nlohmann::json body;



   SK_WebViewResource_Request::SK_WebViewResource_Request(ICoreWebView2WebResourceRequestedEventArgs* args) { parse(args); }

   void parse(ICoreWebView2WebResourceRequestedEventArgs* args)
   {
     wil::com_ptr<ICoreWebView2WebResourceRequest> request;
     args->get_Request(&request);


     // Extract URI
     wil::unique_cotaskmem_string _url;
     if (SUCCEEDED(request->get_Uri(&_url))){
       url = wstringToString(_url.get());
       parseParameters(url);
     }

     // Extract Method
     wil::unique_cotaskmem_string _method;
     if (SUCCEEDED(request->get_Method(&_method))){
       method = wstringToString(_method.get());
     }

     // Extract Headers
     wil::com_ptr<ICoreWebView2HttpRequestHeaders> _headers;
     if (SUCCEEDED(request->get_Headers(&_headers)))
     {
       headers = ExtractHeadersToJson(_headers.get());
     }

     COREWEBVIEW2_WEB_RESOURCE_CONTEXT _resourceContext;
     if (SUCCEEDED(args->get_ResourceContext(&_resourceContext)))
     {
       resourceContext = resourceContextToString(_resourceContext);
     }

     // Extract Body
     wil::com_ptr<IStream> bodyStream;
     if (SUCCEEDED(request->get_Content(&bodyStream)))
     {
       std::string _body = ReadStream(bodyStream.get());
       if (!_body.empty())
       {
         try
         {
           body = nlohmann::json::parse(_body);
         }
         catch (const nlohmann::json::exception& e)
         {
           std::cerr << "Failed to parse JSON body: " << e.what() << std::endl;
           body = _body; // Fallback to raw body
         }
       }
     }
   };

   void parseParameters(const std::string& url)
   {
     std::regex urlRegex(R"(^(https?)://([^/?#]+)([^?#]*)(\?([^#]*))?)");
     std::smatch urlMatch;

     if (std::regex_match(url, urlMatch, urlRegex))
     {
       protocol = urlMatch[1]; // Protocol
       host = urlMatch[2];     // Host
       path = urlMatch[3];     // Path

       // Parse query parameters if they exist
       std::string query = urlMatch[5];
       if (!query.empty())
       {
         std::regex queryRegex(R"(([^&=]+)=([^&]*)?)");
         auto queryBegin = std::sregex_iterator(query.begin(), query.end(), queryRegex);
         auto queryEnd = std::sregex_iterator();

         for (auto it = queryBegin; it != queryEnd; ++it)
         {
           parameters[(*it)[1].str()] = (*it)[2].str();
         }
       }
     }
     else
     {
       throw std::invalid_argument("Invalid URL format");
     }
   }
 };


class SK_WebViewResource_Response {
public:
  wil::com_ptr<ICoreWebView2Environment> webviewEnvironment;


  int statusCode = 404;
  std::string statusMessage = "Not found";
  nlohmann::json headers{{"Content-Type", "application/json"}};
  std::vector<BYTE> data;

  wil::com_ptr<ICoreWebView2WebResourceResponse> response;



  void setAsOK() {
    statusCode = 200;
    statusMessage = "OK";
  }


  SK_WebViewResource_Response::SK_WebViewResource_Response()
  {
    std::string defaultData = "{\"error\":\"404\",\"message\":\"Not found\"}";
    data = std::vector<BYTE>(defaultData.begin(), defaultData.end());
  }




  bool JSON(nlohmann::json json) {
    std::string resAsString = std::any_cast<nlohmann::json>(json).dump(0);
    //DBGMSG(resAsString.c_str());

    statusMessage = "";

    data = std::vector<BYTE>(resAsString.begin(), resAsString.end());

    headers["Content-Type"] = "application/json";

    setAsOK();

    return true;
  }

  bool string(std::string str, std::string mimeType = "plain/text")
  {
    //DBGMSG(str.c_str());

    statusMessage = "";

    data = std::vector<BYTE>(str.begin(), str.end());
    headers["Content-Type"] = mimeType;

    setAsOK();

    return true;
  }

  bool file(std::string path, std::string mimeType = "auto")
  {
    //DBGMSG(path.c_str());

    FILE* file = fopen(path.c_str(), "rb");
    if (file)
    {
      fseek(file, 0, SEEK_END);
      long dataSize = ftell(file);
      char* buffer = (char*)malloc(dataSize + 1);
      fseek(file, 0, SEEK_SET);
      fread(buffer, 1, dataSize, file);


      data.resize(dataSize);
      std::memcpy(data.data(), buffer, dataSize);


      free(buffer);

      fclose(file);

      if (mimeType == "auto"){
        headers["Content-Type"] = getMimeTypeFromFilename(path);
      } else {
        headers["Content-Type"] = mimeType;
      }

      setAsOK();

      return true;
    }

    error(); //something went wrong reading the file so we return a 404

    return false;
  }


  bool error(int code = 404, std::string msg = "Not Found")
  {
    statusCode = code;
    statusMessage = msg;

    JSON({
      {"error", code},
      { "message", msg }
    });

    return this;
  }




  wil::com_ptr<ICoreWebView2WebResourceResponse> get()
  {
    // Create a memory stream from the byte array using CreateStreamOnHGlobal
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, data.size());
    if (hGlobal == nullptr)
    {
      // Handle error if memory allocation fails
      return nullptr;
    }

    // Lock the global memory and copy data to it
    void* pData = GlobalLock(hGlobal);
    if (pData != nullptr)
    {
      memcpy(pData, data.data(), data.size());
      GlobalUnlock(hGlobal);

                 

       // Create an IStream from the global memory handle
      wil::com_ptr<IStream> contentStream;
      HRESULT hr = ::CreateStreamOnHGlobal(hGlobal, TRUE, &contentStream);
      if (FAILED(hr))
      {
        // Handle the error properly if needed
        GlobalFree(hGlobal);
        return nullptr;
      }




      wil::unique_cotaskmem_string _statusMessage = stringToUniqueCoTaskMemString(statusMessage);
      wil::unique_cotaskmem_string _responseHeaders = stringToUniqueCoTaskMemString(jsonToDelimitedString(headers));

      // Create the response with status code, headers, and content stream
      hr = webviewEnvironment->CreateWebResourceResponse(contentStream.get(),   // The stream containing the custom content
                                                          statusCode,            // HTTP status code
                                                          _statusMessage.get(),   // Status message
                                                          _responseHeaders.get(), // MIME type
                                                          &response              // Output response
      );
      
      if (SUCCEEDED(hr))
      {
        return response.get();
      }
    }
  }
};



using SK_WebViewResourceRequest_Callback = std::function<void(SK_WebViewResource_Request& request, SK_WebViewResource_Response& respondWith)>;

class SK_WebViewResourceHandler {
public:
    SK_WebViewResourceRequest_Callback onWebViewResourceRequested;

    SK_WebViewResourceHandler::SK_WebViewResourceHandler() {

        /*onWebViewResourceRequested = [&](SK_WebViewResource_Request& request, SK_WebViewResource_Response& respondWith) {
        
        };*/
    }

    void handleRequest(SK_WebViewResource_Request& request, SK_WebViewResource_Response& respondWith) {
        std::string url = request.url;

        /*response.fromJSON({
          {"key", "value"}
        });*/

        //respondWith.string("Hello :)", "text/html");

        std::string filePath = "c:/test.html";
        respondWith.file(filePath);
    };
};