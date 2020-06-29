#include "tools.hpp"

// static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
// {
//     ((std::string*)userp)->append((char*)contents, size * nmemb);
//     return size * nmemb;
// }
// 
// 
// string tools::web::get_contents(string url) 
// {
// 	string contents;
//   	const char * site = url.c_str();
//  	CURL *curl;
// 	CURLcode res;
// // 	string buffer;
// 	curl = curl_easy_init();
// 	if (curl) {
// 		curl_easy_setopt(curl,CURLOPT_URL,site);
// 		// Tell libcurl what function to call when it has data
//         	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback); 
// 		/* example.com is redirected, so we tell libcurl to follow redirection */ 
//    		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
// 		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contents);
// 		
// 		/* Perform the request, res will get the return code */ 
//     		res = curl_easy_perform(curl);
// 		/* always cleanup */ 
//     		curl_easy_cleanup(curl);
//     		/* Check for errors */ 
//     		if(res != CURLE_OK)	{
// 			cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl; // 
// 			cout << "Curl not OK" << endl;
// 			return "";
// 		}
// 	}
// 	else {
// 	  	cout << "curl_easy_init() failed\n" << endl;
// 		return "";
// 	}
// 	return contents; 
// }
